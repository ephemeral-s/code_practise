#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <cstring>
#include <functional>

typedef std::function<std::string(const std::string&)> callback_t;

class UdpServer {
public:
    UdpServer(uint16_t port, callback_t callback)
        :_port(port), _callback(callback)
    {}

    void Init() {
        //创建套接字
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(_sockfd < 0) {
            std::cout << "socket error" << std::endl;
            exit(1);
        }

        //绑定套接字
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = INADDR_ANY;
        int n = bind(_sockfd, (const sockaddr*)&local, sizeof(local));
        if(n < 0) {
            std::cout << "bind error" << std::endl;
            exit(1);
        }
        std::cout << "bind success" << std::endl;
    }

    void Start() {
        _isrunning = true;
        while(_isrunning) {
            char buff[2048];
            struct sockaddr_in peer;
            socklen_t len = sizeof peer;
            ssize_t n = recvfrom(_sockfd, buff, sizeof buff - 1, 0, (struct sockaddr*)&peer, &len);
            if(n > 0) {
                //收到消息
                int peer_port = ntohs(peer.sin_port);
                std::string peer_ip = inet_ntoa(peer.sin_addr);
                buff[n] = '\0';
                std::cout << peer_ip << ":" << peer_port << " say: " << buff << std::endl;

                //处理消息并发给客户端
                std::string ret =  _callback(buff);
                sendto(_sockfd, ret.c_str(), ret.size(), 0, (struct sockaddr*)&peer, len);   
            }
        }
        _isrunning = false;
    }
private:
    int _sockfd = -1;
    uint16_t _port = 0;
    bool _isrunning = false;
    callback_t _callback;
};

