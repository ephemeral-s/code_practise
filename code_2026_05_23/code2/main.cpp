#include <iostream>
#include "HttpServer.hpp"

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " port" << std::endl;
        exit(1);
    }

    std::unique_ptr<HttpServer> http = std::make_unique<HttpServer>(std::stoi(argv[1]));

    http->Start();
    return 0;
}
