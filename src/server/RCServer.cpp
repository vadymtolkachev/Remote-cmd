#include "RCServer.hpp"


void RCServer::start(const UINT16 port)
{
    DBG_PUTS("RCServer::start start");

    RCWorker *worker;
    void *lpvWorker;
    RCServerSocket *serverSocket;
    RCThread *workerHandlerThread;
    

    DBG_CALL_CRITICAL(m_listenSocket.listen(port));

    RCThread eventListener;

    eventListener.start(EventListener, static_cast<void *>(this));

    while(true)
    {
        worker = new RCWorker();
        workerHandlerThread = new RCThread();
        lpvWorker = static_cast<void *>(worker);
        serverSocket = worker->getServerSocket();

        DBG_PUTS("RCServer::start accept");
        RCRet ret = m_listenSocket.accept(serverSocket);
        if(ret != RCRet::SUCCESS)
            break;

        DBG_PUTS("RCServer::start  workerHandlerThread->start");
        workerHandlerThread->start(WorkerHandler, lpvWorker);

        DBG_PUTS("RCServer::start  m_workerHandlers.push_back");
        m_workerHandlers.push_back(workerHandlerThread);
    }

    freeHandlers();

    DBG_PUTS("RCServer::start end");
}


void RCServer::freeHandlers()
{
    DBG_PUTS("RCServer::freeHandlers start");

    for(auto &workerHandler : m_workerHandlers)
    {
        workerHandler->join();
        delete workerHandler;
        workerHandler = nullptr;
    }

    DBG_PUTS("RCServer::freeHandlers end");
}


DWORD WINAPI RCServer::WorkerHandler(LPVOID lpvParam) noexcept
{
    DBG_PUTS("RCServer::WorkerHandler start");
    assert(lpvParam != nullptr);
    if(lpvParam == nullptr)
        return 0;

    RCWorker *worker = static_cast<RCWorker *>(lpvParam);

    worker->start();

    delete worker;

    DBG_PUTS("RCServer::WorkerHandler end");
    return 0;
}


DWORD WINAPI RCServer::EventListener(LPVOID lpvParam) noexcept
{
    assert(lpvParam != nullptr);
    if(lpvParam == nullptr)
        return 0;

    RCServer *server = static_cast<RCServer *>(lpvParam);

    std::string str;
    while(true)
    {
        std::cin >> str;
        if(str == server_stop_str)
        {
            server->m_listenSocket.close();
            printf("server stopped accepting connections\n");
            break;
        }

        printf("invalid command\n");
    }

    return 0;
}