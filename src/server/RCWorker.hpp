#pragma once
#pragma warning(push, 3)
#include <string>
#include <iostream>
#pragma warning(pop)
#include "utils/RCSocket.hpp"
#include "utils/RCThread.hpp"
#include "RCChildProcess.hpp"



const wchar_t cmd_location[] = L"C:\\Windows\\System32\\cmd.exe";


class RCWorker
{
public:
    RCWorker() = default;
    ~RCWorker() = default;

    RCWorker(const RCWorker &) = delete;
    RCWorker(RCWorker &&) = delete;
    RCWorker &operator=(const RCWorker &) = delete;
    RCWorker &operator=(RCWorker &&) = delete;

    void start();
    RCServerSocket *getServerSocket();

private:
    void syncCmd() const;

    static DWORD WINAPI StdinThreadFunc(LPVOID lpvParam) noexcept;
    static DWORD WINAPI StdoutThreadFunc(LPVOID lpvParam) noexcept;

    RCServerSocket m_serverSocket;
    RCChildProcess m_cmd;
};
