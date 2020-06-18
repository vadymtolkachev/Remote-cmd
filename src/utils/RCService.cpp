#pragma warning(push, 3)
#include "SrvError.h"
#pragma warning(pop)
#include "RCService.hpp"


static WCHAR SVCNAME[] = L"SvcRCServer";

SERVICE_STATUS RCService::m_svcStatus = {};
SERVICE_STATUS_HANDLE RCService::m_svcStatusHandle = NULL;
bool RCService::m_srvStarted = false;
HANDLE RCService::m_svcStopPendingEvent = NULL;
void (*RCService::m_atStart)(void *buf, HANDLE svcStopPendingEvent) = nullptr;
void (*RCService::m_atExit)(void *buf) = nullptr;
void *RCService::m_startBuf = nullptr;
void *RCService::m_exitBuf = nullptr;


void RCService::init(void (*atStart)(void *fStartBuf, HANDLE svcStopPendingEvent), void *startBuf, void (*atExit)(void *fExitBuf), void *exitBuf)
{
    if(m_srvStarted)
    {
        assert(false);
        exit(EXIT_FAILURE);
    }

    m_srvStarted = true;

    m_svcStopPendingEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if(m_svcStopPendingEvent == NULL)
        exit(EXIT_FAILURE);

    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        { SVCNAME, (LPSERVICE_MAIN_FUNCTIONW)svcMain },
        { NULL, NULL }
    };

    m_atStart = atStart;
    m_atExit = atExit;
    m_startBuf = startBuf;
    m_exitBuf = exitBuf;

    if(!StartServiceCtrlDispatcherW(DispatchTable))
    {
        DBG_PRINTF("StartServiceCtrlDispatcherW failed with error %lu\n", GetLastError());
        svcReportEvent(L"StartServiceCtrlDispatcherW");
        exit(EXIT_FAILURE);
    }
}


VOID WINAPI RCService::svcMain(DWORD dwArgc, LPWSTR *lpszArgv) noexcept
{
    UNREFERENCED_PARAMETER(dwArgc);
    UNREFERENCED_PARAMETER(lpszArgv);

    m_svcStatusHandle = RegisterServiceCtrlHandlerW(SVCNAME, svcCtrlHandler);
    if(!m_svcStatusHandle)
    {
        svcReportEvent(L"RegisterServiceCtrlHandler");
        exit(EXIT_FAILURE);
    }

    m_svcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_svcStatus.dwServiceSpecificExitCode = 0;

    reportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);

    assert(m_atStart != nullptr);
    if(!m_atStart)
        exit(EXIT_FAILURE);

    assert(m_atExit != nullptr);
    if(!m_atExit)
        exit(EXIT_FAILURE);

    m_atStart(m_startBuf, m_svcStopPendingEvent);
    
    reportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);

    m_atExit(m_exitBuf);
    closeHandles();
}


VOID WINAPI RCService::svcCtrlHandler(DWORD dwCtrl) noexcept
{
    switch(dwCtrl)
    {
    case SERVICE_CONTROL_STOP:
        reportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
        SetEvent(m_svcStopPendingEvent);
        return;

    default:
        break;
    }
}


void RCService::reportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

    m_svcStatus.dwCurrentState = dwCurrentState;
    m_svcStatus.dwWin32ExitCode = dwWin32ExitCode;
    m_svcStatus.dwWaitHint = dwWaitHint;

    if(dwCurrentState == SERVICE_START_PENDING)
        m_svcStatus.dwControlsAccepted = 0;
    else
        m_svcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    if((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
        m_svcStatus.dwCheckPoint = 0;
    else
        m_svcStatus.dwCheckPoint = dwCheckPoint++;

    SetServiceStatus(m_svcStatusHandle, &m_svcStatus);
}


void RCService::svcReportEvent(LPCWSTR szFunction)
{
    HANDLE hEventSource;
    LPCTSTR lpszStrings[2];
    TCHAR Buffer[80];

    hEventSource = RegisterEventSource(NULL, SVCNAME);

    if(NULL != hEventSource)
    {
        StringCchPrintf(Buffer, 80, TEXT("%s failed with %d"), szFunction, GetLastError());

        lpszStrings[0] = SVCNAME;
        lpszStrings[1] = Buffer;

        ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, SVC_ERROR, NULL, 2, 0, lpszStrings, NULL);

        DeregisterEventSource(hEventSource);
    }
}


void RCService::closeHandles()
{
    if(m_svcStopPendingEvent)
    {
        CloseHandle(m_svcStopPendingEvent);
        m_svcStopPendingEvent = NULL;
    }
}