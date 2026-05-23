#include "Socket.hpp"

#include <memory>
#include <functional>
#include "Logger.hpp"
#include <unistd.h>
#include <sys/wait.h>

typedef std::function<void(std::shared_ptr<Socket>&, const std::string& ip, uint16_t port)> service_t;

class TcpServer {
public:
    TcpServer(uint16_t port) : _port(port) {
        _listenSocketPtr->BuildTcpServerListenSocket(_port);
    }

    void Start(service_t Service) {
        _isrunning = true;
        while(_isrunning) {
            std::string client_ip;
            uint16_t client_port;
            auto sock = _listenSocketPtr->Accept(client_ip, client_port);
            if(sock == nullptr) continue;
            else {
                LOG_INFO("accept success");
            }

            //创建子进程，处理连接
            pid_t pid = fork();
            if(pid < 0) {
                LOG_FATAL("fork error");
                exit(1);
            } else if(pid == 0) {
                //子进程，不关心监听套接字，关闭
                _listenSocketPtr->Close();    

                //再创建子进程处理，防止父进程阻塞
                if(fork() > 0) exit(0);

                //孙子进程 -- 孤儿进程
                Service(sock, client_ip, client_port);
                sock->Close();
                exit(0);
            }
            else {
                //父进程
                sock->Close();
                waitpid(pid, nullptr, 0);
            }
        }
        _isrunning = false;
    }
private:
    bool _isrunning = false;
    std::unique_ptr<Socket> _listenSocketPtr = std::make_unique<TcpSocket>();
    uint16_t _port;
};