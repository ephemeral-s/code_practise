#include "tcp_server.hpp"
#include <memory>

int main() {
    std::unique_ptr<TcpServer> server = std::make_unique<TcpServer>(8080, [](const std::string& msg) {
        return "hello " + msg;
    });
    server->Init();
    server->Start();
    return 0;
}