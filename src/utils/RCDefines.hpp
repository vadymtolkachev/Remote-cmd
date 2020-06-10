#pragma once
#pragma warning(push, 3)
#include <string>
#pragma warning(pop)


const UINT16 serverPort = 3387;


#define MAX_UINT16_LEN 5

const std::string server_stop_str = "stop";
const char cmd_exit_str[] = "exit";
const char cmd_exit_str_cmd[] = "exit\n";
const UINT readBufSize = 256;


enum class RCRet
{
    SUCCESS,
    FAILURE,
    PIPE_CLOSED,
    CONNECTION_CLOSED
};


#define DBG_CALL(func_call)                         \
{                                                   \
    RCRet dbg_call_ret = func_call;                 \
    if(dbg_call_ret != RCRet::SUCCESS)              \
    {                                               \
        printf("%s: %ld: " #func_call " failed\n",  \
            __FILE__, __LINE__);                    \
    }                                               \
}


#define DBG_CALL_CRITICAL(func_call)                \
{                                                   \
    RCRet dbg_call_ret = func_call;                 \
    if(dbg_call_ret != RCRet::SUCCESS)              \
    {                                               \
        printf("%s: %ld: " #func_call " failed\n",  \
            __FILE__, __LINE__);                    \
        exit(EXIT_FAILURE);                         \
    }                                               \
}


//#define NO_DBG_PRINT


#ifndef NO_DBG_PRINT
#define DBG_PUTS(str)       \
{                           \
    puts(str);              \
    fflush(stdout);         \
}
#define DBG_PRINTF(...) printf(__VA_ARGS__) 
#else
#define DBG_PUTS(str)
#define DBG_PRINTF(...)
#endif // NO_DBG_PRINT



