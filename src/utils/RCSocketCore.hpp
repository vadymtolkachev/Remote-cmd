#pragma once
#pragma warning(push, 3)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <cstdio>
#include <cassert>
#pragma warning(pop)
#include "RCDefines.hpp"

#pragma comment(lib, "Ws2_32.lib")


class RCSocketCore
{
public:
    RCSocketCore();
    ~RCSocketCore();

    RCSocketCore(const RCSocketCore &) = delete;
    RCSocketCore(RCSocketCore &&) = delete;
    RCSocketCore &operator=(const RCSocketCore &) = delete;
    RCSocketCore &operator=(RCSocketCore &&) = delete;

    RCRet listen(const UINT16 port);
    RCRet accept(RCSocketCore *socket);

    RCRet connect(const char addr_str[], const UINT16 port);

    RCRet send(const char buf[], int length) const;
    RCRet recv(char buf[], int length, int *bytesReceived) const;

    void shutdown();
    void shutdownSafe();
    void closeSocket();
    void closeSocketSafe();

private:
    static void wsaDestr() noexcept;
    static WSADATA m_wsaData;
    static bool m_wsaIsInitialized;

    SOCKET m_socket;
};