#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <functional>
#include <string>
#include <cstring>
#include <thread>

typedef std::function<std::string(const std::string&)> callback_t;

class NoCopy {
public:
    NoCopy(const NoCopy& no) = delete;
    NoCopy& operator=(const NoCopy& no) = delete;
    NoCopy() = default;
};

class TcpServer : public NoCopy{
public:
    TcpServer(uint16_t port, callback_t callback) : _port(port), _callback(callback) {}

    void Init() {
        //创建套接字
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(_sockfd < 0) {
            std::cout << "socket error" << std::endl;
            exit(1);
        }
        std::cout << "socket success" << std::endl;

        //绑定套接字
        sockaddr_in addr;
        memset(&addr, 0, sizeof addr);
        addr.sin_family = AF_INET;
        addr.sin_port = htons(_port);
        addr.sin_addr.s_addr = INADDR_ANY;

        int n = bind(_sockfd, (sockaddr*)&addr, (socklen_t)sizeof addr);
        if(n < 0) {
            std::cout << "bind error" << std::endl;
            exit(1);
        }
        std::cout << "bind success" << std::endl;

        //监听
        n = listen(_sockfd, 8);
        if(n < 0) {
            std::cout << "listen error" << std::endl;
            exit(1);
        }
        std::cout << "listening..." << std::endl;
    }

    void Service(int sockfd, const std::string& ip, uint16_t port) {
        char buff[2048];
        while(true) {
            //获取消息
            ssize_t n = recv(sockfd, buff, sizeof buff - 1, 0);
            if(n > 0) {
                buff[n] = '\0';
                std::cout << ip << ":" << port << " say: " << buff << std::endl;
                std::string res = _callback(buff);
                send(sockfd, res.c_str(), res.size(), 0);
            } else if(n == 0) {
                //客户端关闭连接
                std::cout << ip << ":" << port << " break" << std::endl;
                break;
            } else {
                //出现错误
                std::cout << ip << ":" << port << " read error" << std::endl;
            }
        }
    }

    void Start() {
        _isrunning = true;
        while(_isrunning) {
            sockaddr_in peer;
            socklen_t len = sizeof peer;
            int sockfd = accept(_sockfd, (sockaddr*)&peer, &len);
            if(sockfd < 0) {
                std::cout << "accept error" << std::endl;
                continue; // 获取连接失败不做任何处理，客户端自行重新连接
            }

            //连接成功
            std::string ip = inet_ntoa(peer.sin_addr);
            uint16_t port = ntohs(peer.sin_port);
            std::cout << "accept success : [" << ip << ":" << port << "]" << std::endl;

            //创建新线程，处理消息
            std::thread t([this, sockfd, ip, port]() {
                Service(sockfd, ip, port);
            });
            t.detach(); // 分离线程，不等待线程结束
        }
        _isrunning = false;
    }
private:
    int _sockfd = -1;
    bool _isrunning = false;
    uint16_t _port;
    callback_t _callback;
};