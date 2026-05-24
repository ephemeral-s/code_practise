#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <cstring>

int main(int argc, char* argv[]) {
    if(argc != 3) {
        std::cout << "Usage: " << argv[0] << " <ip> <port>" << std::endl;
        exit(1);
    }

    std::string ip = argv[1];
    uint16_t port = std::stoul(argv[2]);

    //创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        std::cout << "socket error" << std::endl;
        exit(1);
    }
    std::cout << "socket success" << std::endl;

    //连接服务器
    sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    int n = connect(sockfd, (sockaddr*)&addr, (socklen_t)sizeof addr);
    if(n < 0) {
        std::cout << "connect error" << std::endl;
        exit(1);
    }

    //发送消息
    while(true) {
        std::string msg;
        std::cout << "input:";
        std::getline(std::cin, msg);
        if(msg.empty()) {
            break;
        }
        send(sockfd, msg.c_str(), msg.size(), 0);

        char buff[2048];
        int ret = recv(sockfd, buff, sizeof buff - 1, 0);
        if(ret > 0) {
            buff[ret] = '\0';
            std::cout << buff << std::endl;
        }
        else {
            std::cout << "recv error" << std::endl;
        }
    }
    close(sockfd);
    return 0;
}