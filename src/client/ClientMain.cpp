#include "RCClient.hpp"

const char addr_str[] = "192.168.1.10";


int main()
{
    RCClient client;
    client.start(addr_str, serverPort);

    printf("EXIT_SUCCESS\n");
    return EXIT_SUCCESS;
}