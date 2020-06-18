#pragma once
#include "RCServerSocket.hpp"


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
    RCSocketCore m_socketCore;
};