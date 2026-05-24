#include "Logger.hpp"
#include "PollServer.hpp"

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " port" << std::endl;
        exit(1);
    }
    Logger::Instance().SetLevel(LogLevel::INFO);
    uint16_t port = std::stoi(argv[1]);

    std::unique_ptr<PollServer> svr = std::make_unique<PollServer>(port);
    svr->Start();

    return 0;
}