#pragma once
#pragma warning(push, 3)
#include <windows.h>
#include <cstdio>
#pragma warning(pop)
#include "utils/RCDefines.hpp"


class RCThread
{
public:
    RCThread();
    ~RCThread();

    RCThread(const RCThread &) = delete;
    RCThread(RCThread &&) = delete;
    RCThread &operator=(const RCThread &) = delete;
    RCThread &operator=(RCThread &&) = delete;

    RCRet start(LPTHREAD_START_ROUTINE routine, void *data);
    void join();

    DWORD getThreadId() const;

private:
    HANDLE m_hThread;
    DWORD m_threadId;
};


class RCThreadPool
{
public:
    RCThreadPool() = default;
    ~RCThreadPool() = default;

    RCThreadPool(const RCThreadPool &) = delete;
    RCThreadPool(RCThreadPool &&) = delete;
    RCThreadPool &operator=(const RCThreadPool &) = delete;
    RCThreadPool &operator=(RCThreadPool &&) = delete;

    RCRet start(LPTHREAD_START_ROUTINE routine1, void *data1, LPTHREAD_START_ROUTINE routine2, void *data2);
    void join();

private:
    RCThread m_threads[2];
};
