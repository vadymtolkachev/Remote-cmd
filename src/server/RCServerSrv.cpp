#include "RCServerSrv.hpp"


void RCServerSrv::init(const UINT16 port)
{
    RCServer *server = new RCServer(port);

    void *lpvBuf = static_cast<void *>(server);
    RCService::init(atStart, lpvBuf, atExit, lpvBuf);
}


void RCServerSrv::atStart(void *buf, HANDLE svcStopPendingEvent)
{
    assert(buf != nullptr);
    if(!buf)
        exit(EXIT_FAILURE);

    RCServer *server = static_cast<RCServer *>(buf);

    server->start(svcStopPendingEvent);
}


void RCServerSrv::atExit(void *buf)
{
    assert(buf != nullptr);
    if(!buf)
        exit(EXIT_FAILURE);

    RCServer *server = static_cast<RCServer *>(buf);

    delete server;
}