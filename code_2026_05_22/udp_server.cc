#include "udp_server.hpp"
#include <memory>

std::string callback(const std::string& in) {
    std::string out = "hello!" + in;
    return out; 
}

int main() {
    std::unique_ptr<UdpServer> server = std::make_unique<UdpServer>(8080, callback);
    server->Init();
    server->Start();
    return 0;
}