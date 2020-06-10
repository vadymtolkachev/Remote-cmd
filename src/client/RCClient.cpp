#include "RCClient.hpp"


void RCClient::start(const char server_addr[], const UINT16 server_port)
{
    DBG_CALL_CRITICAL(m_socket.connect(server_addr, server_port));

    void *lpvThis = static_cast<void *>(this);
    RCThread stdinThread, stdoutThread;

    DBG_CALL_CRITICAL(stdinThread.start(StdinThreadFunc, lpvThis));
    DBG_CALL_CRITICAL(stdoutThread.start(StdoutThreadFunc, lpvThis));

    stdinThread.join();
    stdoutThread.join();
}


void RCClient::sendReturnToStdin()
{
    DWORD dwTmp;
    INPUT_RECORD ir[2];
    ir[0].EventType = KEY_EVENT;
    ir[0].Event.KeyEvent.bKeyDown = TRUE;
    ir[0].Event.KeyEvent.dwControlKeyState = 0;
    ir[0].Event.KeyEvent.uChar.UnicodeChar = VK_RETURN;
    ir[0].Event.KeyEvent.wRepeatCount = 1;
    ir[0].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
    ir[0].Event.KeyEvent.wVirtualScanCode = static_cast<WORD>(MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC));
    ir[1] = ir[0];
    ir[1].Event.KeyEvent.bKeyDown = FALSE;
    WriteConsoleInput(GetStdHandle(STD_INPUT_HANDLE), ir, 2, &dwTmp);
}


DWORD WINAPI RCClient::StdinThreadFunc(LPVOID lpvParam) noexcept
{
    assert(lpvParam != nullptr);
    if(lpvParam == nullptr)
        return 0;

    RCClient *client = reinterpret_cast<RCClient *>(lpvParam);

    while(true)
    {
        std::string command_str;
        getline(std::cin, command_str);
        command_str.append("\n");

        RCRet ret = client->m_socket.send(command_str.c_str(), static_cast<int>(command_str.size()));
        if(ret != RCRet::SUCCESS)
        {
            if(ret == RCRet::CONNECTION_CLOSED)
                break;
        
            DBG_PUTS("cmd->write(command_str) failure");
        }

        if(strncmp(command_str.c_str(), cmd_exit_str, strlen(cmd_exit_str)) == 0)
            break;
    }

    DBG_PUTS("StdinThreadFunc end");
    return 0;
}


DWORD WINAPI RCClient::StdoutThreadFunc(LPVOID lpvParam) noexcept
{
    assert(lpvParam != nullptr);
    if(lpvParam == nullptr)
        return 0;

    RCClient *client = reinterpret_cast<RCClient *>(lpvParam);
    char buf[readBufSize] = "";

    while(true)
    {
        RCRet ret = client->m_socket.recv(buf, readBufSize, NULL);
        if(ret == RCRet::FAILURE || ret == RCRet::PIPE_CLOSED || ret == RCRet::CONNECTION_CLOSED)
        {
            client->sendReturnToStdin();
            break;
        }

        printf("%s", buf);
    }

    DBG_PUTS("StdoutThreadFunc end");
    return 0;
}

