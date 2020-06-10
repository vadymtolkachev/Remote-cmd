#pragma once
#pragma warning(push, 3)
#include <string>
#include <iostream>
#include <vector>
#pragma warning(pop)
#include "RCWorker.hpp"


class RCServer
{
public:
    RCServer() = default;
    ~RCServer() = default;

    RCServer(const RCServer &) = delete;
    RCServer(RCServer &&) = delete;
    RCServer &operator=(const RCServer &) = delete;
    RCServer &operator=(RCServer &&) = delete;

    void start(const UINT16 port);

    void freeHandlers();

private:
    static DWORD WINAPI WorkerHandler(LPVOID lpvParam) noexcept;
    static DWORD WINAPI EventListener(LPVOID lpvParam) noexcept;

    RCListenSocket m_listenSocket;
    std::vector<RCThread *> m_workerHandlers;
};