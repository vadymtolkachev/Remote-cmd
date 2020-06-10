#pragma once
#pragma warning(push, 3)
#include <windows.h>
#include <string>
#include <cstdio>
#include <cassert>
#pragma warning(pop)
#include "utils/RCDefines.hpp"


#define RCCloseHandle(handle)                                   \
{                                                               \
    BOOL closeHandleRet = CloseHandle(handle);                  \
    if(!closeHandleRet)                                         \
        printf("%s: %ld: CloseHandle(" #handle ") failure\n",   \
            __FILE__, __LINE__);                                \
                                                                \
    handle = NULL;                                              \
}


class RCChildProcess
{
public:
    RCChildProcess();
    ~RCChildProcess();

    RCChildProcess(const RCChildProcess &) = delete;
    RCChildProcess(RCChildProcess &&) = delete;
    RCChildProcess &operator=(const RCChildProcess &) = delete;
    RCChildProcess &operator=(RCChildProcess &&) = delete;

    RCRet start(const wchar_t process_location[]);

    RCRet write(const void *buf, DWORD size);
    RCRet write(const std::string &str);
    RCRet read(void *buf, DWORD size, DWORD *bytesReceived);

private:
    RCRet createPipes();
    RCRet createChildProcess(std::wstring &process_location);

    HANDLE m_hChildStd_IN_Rd; 
    HANDLE m_hChildStd_IN_Wr; 
    HANDLE m_hChildStd_OUT_Rd; 
    HANDLE m_hChildStd_OUT_Wr;
};

