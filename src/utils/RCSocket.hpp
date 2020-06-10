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


class RCServerSocket;


class RCSocket
{
public:
    RCSocket();
    ~RCSocket();

    RCSocket(const RCSocket &) = delete;
    RCSocket(RCSocket &&) = delete;
    RCSocket &operator=(const RCSocket &) = delete;
    RCSocket &operator=(RCSocket &&) = delete;

    RCRet listen(const UINT16 port);
    RCRet accept(RCSocket *socket);

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


class RCListenSocket
{
public:
    RCListenSocket() = default;
    ~RCListenSocket() = default;

    RCListenSocket(const RCListenSocket &) = delete;
    RCListenSocket(RCListenSocket &&) = delete;
    RCListenSocket &operator=(const RCListenSocket &) = delete;
    RCListenSocket &operator=(RCListenSocket &&) = delete;

    RCRet listen(const UINT16 port);
    RCRet accept(RCServerSocket *serverSocket);

    void close();

private:
    RCSocket m_socket;
};


class RCServerSocket
{
public:
    RCServerSocket() = default;
    ~RCServerSocket();

    RCServerSocket(const RCServerSocket &socket) = delete;
    RCServerSocket(RCServerSocket &&) = delete;
    RCServerSocket &operator=(const RCServerSocket &) = delete;
    RCServerSocket &operator=(RCServerSocket &&) = delete;

    RCRet send(const char buf[], int length) const;
    RCRet recv(char buf[], int length, int *bytesReceived) const;

    void close();

private:
    RCSocket m_socket;

    friend class RCListenSocket;
};


class RCClientSocket
{
public:
    RCClientSocket() = default;
    ~RCClientSocket();

    RCClientSocket(const RCClientSocket &) = delete;
    RCClientSocket(RCClientSocket &&) = delete;
    RCClientSocket &operator=(const RCClientSocket &) = delete;
    RCClientSocket &operator=(RCClientSocket &&) = delete;

    RCRet send(const char buf[], int length) const;
    RCRet recv(char buf[], int length, int *bytesReceived) const;

    RCRet connect(const char addr_str[], const UINT16 port);

private:
    RCSocket m_socket;
};

