#pragma once

#include "Logger.hpp"
#include "Socket.hpp"
#include <cstdint>
#include <memory>
#include <sys/epoll.h>

class EpollServer {
    const static int defaultfd = -1;
    const static int max_size = 128;
public:
    EpollServer(uint16_t port) {
        _listensock->BuildTcpServerListenSocket(port);

        //创建epoll模型
        _epfd = epoll_create(1);
        if(_epfd < 0) {
            LOG_FATAL("epoll create error");
            exit(1);
        }

        //创建监听事件
        epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = _listensock->Fd();
        int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, _listensock->Fd(), &event);
        if(n < 0) {
            LOG_FATAL("epoll ctl error");
            exit(1);
        }
    }

    void Start() {
        _isrunning = true;
        int timeout = 5000;
        while(_isrunning) {
            //等待文件就绪
            int n = epoll_wait(_epfd, _events, max_size, timeout);
            if(n > 0) {
                HeaderEvents(n);
            } else if (n == 0) {
                //超时
                LOG_INFO("epoll timeout");
            } else {
                //出错
                LOG_ERROR("epoll wait error");
            }
        }
        _isrunning = false;
    }

    //事件配发器
    void HeaderEvents(int n) {
        for(int i = 0; i < n; i++) {
            if(_events[i].events & EPOLLIN) // 读事件就绪
            {
                //判断是监听还是通信
                if(_events[i].data.fd == _listensock->Fd()) {
                    Accepter();
                }
                else {
                    Recver(_events[i].data.fd);
                }
            }
        }
    }

    //连接管理器
    void Accepter() {
        //与客户端建立连接
        std::string ip;
        uint16_t port;
        auto sock = _listensock->Accept(ip, port);
        if(sock != nullptr) {
            //将新的文件加入epoll模型
            epoll_event event;
            event.data.fd = sock->Fd();
            event.events = EPOLLIN;

            int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, sock->Fd(), &event);
            if(n < 0) {
                LOG_ERROR("epoll ctl tcpsocket error");
            }
            LOG_INFO("New connection added at fd: ", sock->Fd(), ", ip: ", ip, ", port: ", port);
        }
    }

    //IO处理器
    void Recver(int fd) {
        std::string buff;
        std::unique_ptr<Socket> sock = std::make_unique<TcpSocket>(fd);
        int n = sock->Recv(buff);
        if(n > 0) {
            LOG_INFO("client say: ", buff);
            std::string out = "hello! " + buff;
            sock->Send(out);
        } else if(n == 0) {
            //客户端退出
            LOG_INFO("client break: ", fd);

            //先解绑再关闭文件
            epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
            sock->Close();
        } else {
            //出现错误
            LOG_WARN("read warning", fd);

            //先解绑再关闭文件
            epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
            sock->Close();
        }
    }
private:
    std::unique_ptr<Socket> _listensock = std::make_unique<TcpSocket>();
    bool _isrunning = false;

    int _epfd;
    epoll_event _events[max_size];
};