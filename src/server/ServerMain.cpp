#include "RCServer.hpp"


int main()
{
    RCServer server;
    server.start(serverPort);

    printf("EXIT_SUCCESS\n");
    return EXIT_SUCCESS;
}