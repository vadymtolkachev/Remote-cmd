#pragma once
#include "RCSocketCore.hpp"


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
    RCSocketCore m_socketCore;

    friend class RCListenSocket;
};
