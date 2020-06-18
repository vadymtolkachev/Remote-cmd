#pragma once
#include "RCSocketCore.hpp"


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
    RCSocketCore m_socketCore;
};

