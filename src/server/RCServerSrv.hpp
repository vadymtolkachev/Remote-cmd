#pragma once
#include "RCServer.hpp"
#include "utils/RCService.hpp"


class RCServerSrv
{
public:
    RCServerSrv() = default;
    ~RCServerSrv() = default;

    RCServerSrv(const RCServerSrv &) = delete;
    RCServerSrv(RCServerSrv &&) = delete;
    RCServerSrv &operator=(const RCServerSrv &) = delete;
    RCServerSrv &operator=(RCServerSrv &&) = delete;

    static void init(const UINT16 port);

private:
    static void atStart(void *buf, HANDLE svcStopPendingEvent);
    static void atExit(void *buf);
};