#include "RCClient.hpp"


int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("usage: client.exe server-ip\n");
        return EXIT_FAILURE;
    }

    RCClient().start(argv[1], serverPort);

    printf("EXIT_SUCCESS\n");
    return EXIT_SUCCESS;
}