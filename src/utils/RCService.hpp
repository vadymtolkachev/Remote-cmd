#pragma once
#pragma warning(push, 3)
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <cstdlib>
#include <cassert>
#include "SrvError.h"
#pragma warning(pop)
#include <utils/RCDefines.hpp>

#pragma comment(lib, "advapi32.lib")


class RCService
{
public:
    RCService() = default;
    ~RCService() = default;

    RCService(const RCService & socket) = delete;
    RCService(RCService &&) = delete;
    RCService &operator=(const RCService &) = delete;
    RCService &operator=(RCService &&) = delete;

    static void init(void (*atStart)(void *fStartBuf, HANDLE svcStopPendingEvent), void *startBuf, void (*atExit)(void *fExitBuf), void *exitBuf);


private:
    static VOID WINAPI svcMain(DWORD, LPWSTR *) noexcept;
    static VOID WINAPI svcCtrlHandler(DWORD) noexcept;

    static void reportSvcStatus(DWORD, DWORD, DWORD);
    static void svcReportEvent(LPCWSTR);

    static void closeHandles();

    static SERVICE_STATUS m_svcStatus;
    static SERVICE_STATUS_HANDLE m_svcStatusHandle;
    static HANDLE m_svcStopPendingEvent;
    
    static bool m_srvStarted;

    static void (*m_atStart)(void *buf, HANDLE svcStopPendingEvent);
    static void (*m_atExit)(void *buf);

    static void *m_startBuf;
    static void *m_exitBuf;
};