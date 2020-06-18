#include "RCWorker.hpp"


void RCWorker::start()
{
    DBG_PUTS("RCWorker::start start");

    DBG_CALL_CRITICAL(m_cmd.start(cmd_location));

    void *lpvThis = static_cast<void *>(this);
    RCThreadPool threadPool;
    DBG_CALL_CRITICAL(threadPool.start(StdinThreadFunc, lpvThis, StdoutThreadFunc, lpvThis));

    threadPool.join();

    DBG_PUTS("RCWorker::start end");
}


RCServerSocket *RCWorker::getServerSocket()
{
    return &m_serverSocket;
}


void RCWorker::syncCmd() const
{
    Sleep(10);
}


DWORD WINAPI RCWorker::StdinThreadFunc(LPVOID lpvParam) noexcept
{
    DBG_PUTS("RCWorker::StdinThreadFunc start");

    assert(lpvParam != nullptr);
    if(lpvParam == nullptr)
        return 0;
    
    RCWorker *worker = static_cast<RCWorker *>(lpvParam);

    char buf[readBufSize] = "";
    int bytesReceived = 0;

    while(true)
    {
        DBG_PUTS("recv");
        RCRet ret = worker->m_serverSocket.recv(buf, readBufSize, &bytesReceived);
        if(ret == RCRet::CONNECTION_CLOSED)
        {
            DBG_PUTS("RCWorker::StdinThreadFunc: recv: connection closed");
            worker->m_cmd.write(cmd_exit_str_cmd, static_cast<DWORD>(strlen(cmd_exit_str_cmd)+1));

            break;
        }

        DBG_PUTS("write");
        ret = worker->m_cmd.write(buf, static_cast<DWORD>(bytesReceived));
        if(ret != RCRet::SUCCESS)
        {
            DBG_PUTS("cmd->write(command_str) failure");
        }

        if(bytesReceived > static_cast<int>(strlen(cmd_exit_str)) && strncmp(buf, cmd_exit_str, strlen(cmd_exit_str)) == 0)
        {
            break;
        }
    }

    DBG_PUTS("RCWorker::StdinThreadFunc end");
    return 0;
}


DWORD WINAPI RCWorker::StdoutThreadFunc(LPVOID lpvParam) noexcept
{
    DBG_PUTS("RCWorker::StdoutThreadFunc start");

    assert(lpvParam != nullptr);
    if(lpvParam == nullptr)
        return 0;

    RCWorker *worker = static_cast<RCWorker *>(lpvParam);

    char buf[readBufSize] = "";
    DWORD bytesReceived = 0;

    while(true)
    {
        DBG_PUTS("read");
        RCRet ret = worker->m_cmd.read(buf, readBufSize, &bytesReceived);
        if(ret == RCRet::FAILURE || ret == RCRet::PIPE_CLOSED)
        {
            DBG_PUTS("break1");
            break;
        }

        DBG_PUTS("send");
        ret = worker->m_serverSocket.send(buf, static_cast<int>(bytesReceived));
        if(ret != RCRet::SUCCESS)
        {
            DBG_PUTS("server->m_socket.send failure");
        }

        if(ret == RCRet::CONNECTION_CLOSED)
        {
            DBG_PUTS("break2");
            break;
        }

        DBG_PUTS("StdoutThreadFunc sync");
        worker->syncCmd();
    }

    DBG_PUTS("RCWorker::StdoutThreadFunc end");
    return 0;
}
