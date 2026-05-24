#include "Logger.hpp"
#include "Socket.hpp"
#include <cstdint>
#include <sys/select.h>
#include <memory>

class SelectServer {
public:
    SelectServer(uint16_t port) {
        _listensock->BuildTcpServerListenSocket(port);
        for(auto& fd : _fd_arr) fd = -1;
        _fd_arr[0] = _listensock->Fd();
    }

    void Start() {
        _isrunning = true;
        while(_isrunning) {
            fd_set fds;
            FD_ZERO(&fds);
            int maxfd = -1;

            //将描述符加入fds，并找出最大值
            for(auto& fd : _fd_arr) {
                if(fd != -1) {
                    FD_SET(fd, &fds);
                    if(maxfd < fd) {
                        maxfd = fd;
                    }
                }
            }

            //设置超时时间
            timeval timeout = {2, 0};

            //等待就绪
            int n = select(maxfd + 1, &fds, nullptr, nullptr, &timeout);
            if(n > 0) {
                HeaderEvents(fds);
                LOG_INFO("select success");
            } else if (n == 0) {
                //超时
                LOG_INFO("select timeout");
            } else {
                //出错
                LOG_ERROR("select error");
            }
        }
        _isrunning = false;
    }

    //事件配发器
    void HeaderEvents(fd_set fds) {
        //判断就绪的文件描述符是监听还是通信，进行分发处理
        for(int i = 0; i < 8; i++) {
            //先筛选合法且已经就绪的fd
            if(_fd_arr[i] != -1 && FD_ISSET(_fd_arr[i], &fds)) {
                if(_fd_arr[i] == _listensock->Fd()) {
                    //监听fd
                    Accepter();
                } else {
                    //通信fd
                    Recver(_fd_arr[i], i);
                }
            }
        }
    }

    //连接管理器 -- 处理就绪的新连接，并将新fd添加到辅助数组中
    void Accepter() {
        std::string ip;
        uint16_t port;
        auto sock = _listensock->Accept(ip, port);
        bool is_set = false;
        if(sock != nullptr) {
            //将新fd加入辅助数组
            for(int i = 0; i < 8; i++) {
                if(_fd_arr[i] == -1) {
                    _fd_arr[i] = sock->Fd();
                    is_set = true;

                    LOG_INFO("New connection added at index ", i, ", fd: ", sock->Fd(), ", ip: ", ip, ", port: ", port);
                    break;
                }
            }
            if(!is_set) {
                //此时已经到达最大连接数
                LOG_WARN("select full");
                close(sock->Fd());
            }
        }
    }

    //IO处理器 -- 根据传入的通信fd进行消息交互，并管理通信fd
    void Recver(int fd, int pos) {
        std::string buff;
        std::unique_ptr<Socket> sock = std::make_unique<TcpSocket>(fd);
        int n = sock->Recv(buff);
        if(n > 0) {
            LOG_INFO("client say: ", buff);
            std::string out = "hello! " + buff;
            sock->Send(out);
        } else if(n == 0) {
            //客户端关闭连接
            LOG_INFO("client break: ", fd);
            _fd_arr[pos] = -1;
            sock->Close();
        } else {
            LOG_WARN("read warning");
            _fd_arr[pos] = -1;
            sock->Close();
        }
    }
private:
    bool _isrunning = false;
    std::unique_ptr<Socket> _listensock = std::make_unique<TcpSocket>();
    int _fd_arr[8]; // 维护select描述符
};