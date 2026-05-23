#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <memory>
#include "Logger.hpp"

class Socket {
public:
    virtual void doSocket() = 0;
    virtual void doBind(uint16_t) = 0;
    virtual void doListen(int) = 0;
    virtual std::shared_ptr<Socket> Accept(std::string&, uint16_t&) = 0;
    virtual void Close() = 0;
    virtual int Send(const std::string&) = 0;
    virtual int Recv(std::string&) = 0;
    virtual int Connect(const std::string&, uint16_t) = 0;
public:
    Socket() = default;
    virtual void BuildTcpServerListenSocket(uint16_t port, int backlog = 16) final {
        doSocket();
        doBind(port);
        doListen(backlog);
    }

    virtual void BuildTcpClientListenSocket() final {
        doSocket();
    }
};

class TcpSocket : public Socket {
public:
    TcpSocket() = default;
    TcpSocket(int fd) : _sockfd(fd) {}

    virtual void doSocket() override {
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(_sockfd < 0) {
            LOG_FATAL("socket error");
            exit(1);
        }
        LOG_INFO("socket success");
    }

    virtual void doBind(uint16_t port) override {
        sockaddr_in addr;
        memset(&addr, 0, sizeof addr);
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        int n = bind(_sockfd, (sockaddr*)&addr, (socklen_t)sizeof addr);
        if(n < 0) {
            LOG_FATAL("bind error");
            exit(1);
        }
        LOG_INFO("bind success");
    }

    virtual void doListen(int backlog) override {
        int n = listen(_sockfd, backlog);
        if(n < 0) {
            LOG_FATAL("listen error");
            exit(1);
        }
        LOG_INFO("listen success");
    }

    virtual std::shared_ptr<Socket> Accept(std::string& ip, uint16_t& port) override {
        sockaddr_in addr;
        socklen_t len = sizeof addr;
        int sockfd = accept(_sockfd, (sockaddr*)&addr, &len);
        if(sockfd < 0) {
            LOG_WARN("accept warning");
            return nullptr;
        }
        ip = inet_ntoa(addr.sin_addr);
        port = ntohs(addr.sin_port);
        return std::make_shared<TcpSocket>(sockfd);
    }

    virtual void Close() override {
        if(_sockfd > 0) close(_sockfd);
    }

    virtual int Send(const std::string& input) override {
        return send(_sockfd, input.c_str(), input.size(), 0);
    }

    virtual int Recv(std::string& output) override {
        char buff[8192];
        ssize_t len = recv(_sockfd, buff, sizeof buff - 1, 0);
        if(len > 0) {
            buff[len] = '\0';
            output += buff;
        }
        return len;
    }

    virtual int Connect(const std::string& ip, uint16_t port) override {
        sockaddr_in addr;
        memset(&addr, 0, sizeof addr);
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        return connect(_sockfd, (sockaddr*)&addr, (socklen_t)sizeof addr);
    }
private:
    int _sockfd = -1;
};
