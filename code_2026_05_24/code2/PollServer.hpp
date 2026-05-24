#include "Logger.hpp"
#include "Socket.hpp"
#include <cstdint>
#include <poll.h>
#include <memory>

class PollServer {
    const static int max_size = 128;
    const static int defaultfd = -1;
public:
    PollServer(uint16_t port) {
        _listensock->BuildTcpServerListenSocket(port);
        
        //初始化poll描述符集
        for(int i = 0; i < max_size; i++) {
            _fds[i].fd = defaultfd;
            _fds[i].events = 0;
            _fds[i].revents = 0;
        }

        //初始化监听fd
        _fds[0].fd = _listensock->Fd();
        _fds[0].events = POLLIN;
    }

    void Start() {
        _isrunning = true;

        int timeout = 5000;
        while(_isrunning) {
            //等待就绪
            int n = poll(_fds, max_size, timeout);
            if(n > 0) {
                HeaderEvents();
                LOG_INFO("poll success");
            } else if (n == 0) {
                //超时
                LOG_INFO("poll timeout");
            } else {
                //出错
                LOG_ERROR("poll error");
            }
        }
        _isrunning = false;
    }

    //事件配发器
    void HeaderEvents() {
        //判断就绪的文件描述符是监听还是通信，进行分发处理
        for(int i = 0; i < max_size; i++) {
            //先筛选合法且已经就绪的fd
            if(_fds[i].fd != defaultfd && (_fds[i].revents & POLLIN)) {
                if(_fds[i].fd == _listensock->Fd()) {
                    //监听fd
                    Accepter();
                } else {
                    //通信fd
                    Recver(i);
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
            for(int i = 0; i < max_size; i++) {
                if(_fds[i].fd == defaultfd) {
                    _fds[i].fd = sock->Fd();
                    _fds[i].events = POLLIN;
                    is_set = true;

                    LOG_INFO("New connection added at index ", i, ", fd: ", sock->Fd(), ", ip: ", ip, ", port: ", port);
                    break;
                }
            }
            if(!is_set) {
                //此时已经到达最大连接数
                LOG_WARN("poll full");
                close(sock->Fd());
            }
        }
    }

    //IO处理器 -- 根据传入的通信fd进行消息交互，并管理通信fd
    void Recver(int pos) {
        std::string buff;
        std::unique_ptr<Socket> sock = std::make_unique<TcpSocket>(_fds[pos].fd);
        int n = sock->Recv(buff);
        if(n > 0) {
            LOG_INFO("client say: ", buff);
            std::string out = "hello! " + buff;
            sock->Send(out);
        } else if(n == 0) {
            //客户端关闭连接
            LOG_INFO("client break: ", _fds[pos].fd);
            _fds[pos].fd = defaultfd;
            sock->Close();
        } else {
            LOG_WARN("read warning");
            _fds[pos].fd = -1;
            sock->Close();
        }
    }
private:
    bool _isrunning = false;
    std::unique_ptr<Socket> _listensock = std::make_unique<TcpSocket>();
    
    pollfd _fds[max_size]; // poll描述符集
};