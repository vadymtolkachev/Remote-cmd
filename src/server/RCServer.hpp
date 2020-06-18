#pragma once
#pragma warning(push, 3)
#include <string>
#include <iostream>
#include <vector>
#pragma warning(pop)
#include "RCWorker.hpp"
#include "utils/RCService.hpp"


class RCServer
{
public:
    RCServer() = delete;
    RCServer(const UINT16 port);
    ~RCServer() = default;

    RCServer(const RCServer &) = delete;
    RCServer(RCServer &&) = delete;
    RCServer &operator=(const RCServer &) = delete;
    RCServer &operator=(RCServer &&) = delete;

    void start(HANDLE pSvcStopPendingEvent);

private:
    static DWORD WINAPI WorkerHandler(LPVOID lpvParam) noexcept;
    static DWORD WINAPI SrvEventListener(LPVOID lpvParam) noexcept;
    static DWORD WINAPI CmdEventListener(LPVOID lpvParam) noexcept;
    
    void freeHandlers();

    RCListenSocket m_listenSocket;
    std::vector<RCThread *> m_workerHandlers;

    HANDLE m_svcStopPendingEvent;
    UINT16 m_port;
};