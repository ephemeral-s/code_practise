#include "Logger.hpp"
#include "SelectServer.hpp"

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " port" << std::endl;
        exit(1);
    }
    Logger::Instance().SetLevel(LogLevel::INFO);
    uint16_t port = std::stoi(argv[1]);

    std::unique_ptr<SelectServer> svr = std::make_unique<SelectServer>(port);
    svr->Start();

    return 0;
}