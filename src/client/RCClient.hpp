#pragma once
#pragma warning(push, 3)
#include <string>
#include <iostream>
#pragma warning(pop)
#include "utils/RCSocket.hpp"
#include "utils/RCThread.hpp"


const int recvBufSize = 40;


class RCClient
{
public:
    RCClient() = default;
    ~RCClient() = default;

    RCClient(const RCClient &) = delete;
    RCClient(RCClient &&) = delete;
    RCClient &operator=(const RCClient &) = delete;
    RCClient &operator=(RCClient &&) = delete;

    void start(const char server_addr[], const UINT16 server_port);

private:
    void sendReturnToStdin();
    static DWORD WINAPI StdinThreadFunc(LPVOID lpvParam) noexcept;
    static DWORD WINAPI StdoutThreadFunc(LPVOID lpvParam) noexcept;

    RCClientSocket m_socket;
};