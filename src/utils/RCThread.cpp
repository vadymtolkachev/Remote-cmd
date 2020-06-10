#include "utils/RCThread.hpp"


RCThread::RCThread():
    m_hThread(NULL),
    m_threadId(0)
{}


RCThread::~RCThread()
{
    if(m_hThread != NULL)
        CloseHandle(m_hThread);
}

#pragma warning(push)
#pragma warning(disable: 5039) // LPTHREAD_START_ROUTINE is not noexcept functon
RCRet RCThread::start(LPTHREAD_START_ROUTINE routine, void *data)
{
    m_hThread = CreateThread(NULL, 0, routine, data, 0, &m_threadId);
    if(!m_hThread)
    {
        printf("CreateThread failure\n");
        return RCRet::FAILURE;
    }

    return RCRet::SUCCESS;
}
#pragma warning( pop ) 


void RCThread::join()
{
    WaitForSingleObject(m_hThread, INFINITE);
    CloseHandle(m_hThread);
    m_hThread = NULL;
    m_threadId = 0;
}


DWORD RCThread::getThreadId() const
{
    return m_threadId;
}


RCRet RCThreadPool::start(LPTHREAD_START_ROUTINE routine1, void *data1, LPTHREAD_START_ROUTINE routine2, void *data2)
{
    RCRet ret = m_threads[0].start(routine1, data1);
    if(ret != RCRet::SUCCESS)
        return ret;

    ret = m_threads[1].start(routine2, data2);
    return ret;
}


void RCThreadPool::join()
{
    m_threads[0].join();
    m_threads[1].join();
}



