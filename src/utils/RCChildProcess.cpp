#pragma once
#include "RCChildProcess.hpp"


RCChildProcess::RCChildProcess():
    m_hChildStd_IN_Rd(NULL),
    m_hChildStd_IN_Wr(NULL),
    m_hChildStd_OUT_Rd(NULL),
    m_hChildStd_OUT_Wr(NULL)
{}


RCChildProcess::~RCChildProcess()
{
    if(m_hChildStd_IN_Wr != NULL)
        RCCloseHandle(m_hChildStd_IN_Wr);

    if(m_hChildStd_OUT_Rd != NULL)
        RCCloseHandle(m_hChildStd_OUT_Rd);

    assert(m_hChildStd_IN_Rd == NULL);
    assert(m_hChildStd_OUT_Wr == NULL);
}


RCRet RCChildProcess::start(const wchar_t process_location[])
{
    RCRet ret = createPipes();
    if(ret != RCRet::SUCCESS)
    {
        assert(m_hChildStd_IN_Rd == NULL);
        assert(m_hChildStd_IN_Wr == NULL);
        assert(m_hChildStd_OUT_Rd == NULL);
        assert(m_hChildStd_OUT_Wr == NULL);

        return ret;
    }

    assert(m_hChildStd_IN_Rd != NULL);
    assert(m_hChildStd_IN_Wr != NULL);
    assert(m_hChildStd_OUT_Rd != NULL);
    assert(m_hChildStd_OUT_Wr != NULL);

    std::wstring wstr(process_location);

    ret = createChildProcess(wstr);
    if(ret != RCRet::SUCCESS)
    {
        RCCloseHandle(m_hChildStd_IN_Rd);
        RCCloseHandle(m_hChildStd_IN_Wr);
        RCCloseHandle(m_hChildStd_OUT_Rd);
        RCCloseHandle(m_hChildStd_OUT_Wr);
        
        return ret;
    }

    assert(m_hChildStd_IN_Rd == NULL);
    assert(m_hChildStd_IN_Wr != NULL);
    assert(m_hChildStd_OUT_Rd != NULL);
    assert(m_hChildStd_OUT_Wr == NULL);

    return RCRet::SUCCESS;
}


RCRet RCChildProcess::write(const void *buf, DWORD size)
{
    DWORD dwWritten;

    //DBG_PRINTF("RCChildProcess::write: buf = %s\n");
    BOOL bSuccess = WriteFile(m_hChildStd_IN_Wr, buf, size, &dwWritten, NULL);
    if(!bSuccess)
    {
        printf("WriteFile failure\n");
        return RCRet::FAILURE;
    }

    if(size != dwWritten)
    {
        printf("RCChildProcess::write: size != dwWritten\nsize = %lu; dwWritten = %lu\n", size, dwWritten);
    }

    return RCRet::SUCCESS;
}


RCRet RCChildProcess::write(const std::string &str)
{
    return write(str.c_str(), static_cast<DWORD>(str.size()));
}


RCRet RCChildProcess::read(void *buf, DWORD size, DWORD *bytesReceived)
{
    DWORD dwRead;
    BOOL bRet = ReadFile(m_hChildStd_OUT_Rd, buf, size-1, &dwRead, NULL);
    if(!bRet)
        return RCRet::FAILURE;

    if(dwRead == 0)
        return RCRet::PIPE_CLOSED;

    char *buf_str = reinterpret_cast<char *>(buf);
    buf_str[dwRead] = '\0';

    if(bytesReceived)
        *bytesReceived = dwRead + 1;

    return RCRet::SUCCESS;
}


RCRet RCChildProcess::createPipes()
{
    SECURITY_ATTRIBUTES saAttr;

    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE; // Set the bInheritHandle flag so pipe handles are inherited. 
    saAttr.lpSecurityDescriptor = NULL;

    if(!CreatePipe(&m_hChildStd_OUT_Rd, &m_hChildStd_OUT_Wr, &saAttr, 0))
    {
        printf("Stdout CreatePipe failed\n");
        goto StdoutCreatePipeFailure;
    }

    // Ensure the read handle to the pipe for STDOUT is not inherited.
    if(!SetHandleInformation(m_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
    {
        printf("Stdout SetHandleInformation failed\n");
        goto StdoutSetHandleInformationFailure;
    }

    if(!CreatePipe(&m_hChildStd_IN_Rd, &m_hChildStd_IN_Wr, &saAttr, 0))
    {
        printf("Stdin CreatePipe failed\n");
        goto StdinCreatePipeFailure;
    }

    // Ensure the write handle to the pipe for STDIN is not inherited. 
    if(!SetHandleInformation(m_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
    {
        printf("Stdin SetHandleInformation failed\n");
        goto StdinSetHandleInformationFailure;
    }

    return RCRet::SUCCESS;


StdinSetHandleInformationFailure:
    RCCloseHandle(m_hChildStd_IN_Rd);
    RCCloseHandle(m_hChildStd_IN_Wr);

StdinCreatePipeFailure:
    m_hChildStd_IN_Rd = NULL; 
    m_hChildStd_IN_Wr = NULL;

StdoutSetHandleInformationFailure:
    RCCloseHandle(m_hChildStd_OUT_Rd);
    RCCloseHandle(m_hChildStd_OUT_Wr);

StdoutCreatePipeFailure:
    m_hChildStd_OUT_Rd = NULL;
    m_hChildStd_OUT_Wr = NULL;

    return RCRet::FAILURE;
}


RCRet RCChildProcess::createChildProcess(std::wstring &process_location)
{
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));

    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = m_hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = m_hChildStd_OUT_Wr;
    siStartInfo.hStdInput = m_hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;



    BOOL bRet = CreateProcessW(NULL, &process_location[0], NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo);
    if(!bRet)
    {
        printf("CreateProcessW failure\n");
        return RCRet::FAILURE;
    }

    RCCloseHandle(piProcInfo.hProcess);
    RCCloseHandle(piProcInfo.hThread);
    RCCloseHandle(m_hChildStd_OUT_Wr);
    RCCloseHandle(m_hChildStd_IN_Rd);

    return RCRet::SUCCESS;
}
