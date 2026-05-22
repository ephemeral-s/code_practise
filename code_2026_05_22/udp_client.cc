#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <iostream>

int main(int argc, char* argv[])
{
    if(argc != 3) {
        std::cout << "ERROR: Usage: " << argv[0] << "[ip] [port]" << std::endl;
    }

    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);

    //创建套接字
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) {
        std::cout << "socket error!" << std::endl;
        exit(1);
    }
    //client无需绑定套接字，操作系统自行绑定

    //填写目标服务器信息
    sockaddr_in server;
    memset(&server, 0, sizeof server);
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    server.sin_addr.s_addr = inet_addr(server_ip.c_str());

    //发送消息
    while(true) {
        std::string input;
        std::cout << "input: ";
        std::getline(std::cin, input);

        sendto(sockfd, input.c_str(), input.size(), 0, (sockaddr*)&server, sizeof(server));

        //接收消息
        char buff[1024];
        struct sockaddr_in peer;
        socklen_t len = sizeof peer;
        int n = recvfrom(sockfd, buff, sizeof buff - 1, 0, (struct sockaddr*)&peer, &len);
        if(n > 0)
        {
            buff[n] = '\0';
            std::cout << buff << std::endl;
        }
    }
}