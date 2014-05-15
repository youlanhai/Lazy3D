//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ZLock.h"



namespace Lazy
{

    ZSimpleLock::ZSimpleLock()
        : m_mark(0)
    {}

    void ZSimpleLock::lock()
    {
        while (InterlockedExchange(&m_mark, 1) == 1)
        {
            Sleep(0);
        }
    }

    void ZSimpleLock::unlock()
    {
        InterlockedExchange(&m_mark, 0);
    }



    ZCritical::ZCritical()
    {
        InitializeCriticalSection(&m_cs);
    }

    ZCritical::~ZCritical()
    {
        DeleteCriticalSection(&m_cs);
    }

    void ZCritical::lock()
    {
        EnterCriticalSection(&m_cs);
    }

    void ZCritical::unlock()
    {
        LeaveCriticalSection(&m_cs);
    }




    ZMutex::ZMutex(void)
        : m_hMutex(CreateMutex(NULL, FALSE, NULL))
    {}

    ZMutex::~ZMutex(void)
    {
        CloseHandle(m_hMutex);
    }

    void ZMutex::lock()
    {
        WaitForSingleObject(m_hMutex, INFINITE);
    }

    void ZMutex::unlock()
    {
        ReleaseMutex(m_hMutex);
    }



    ZSemaphore::ZSemaphore()
        : m_hSemaphore(CreateSemaphore(NULL, 0, 32767, NULL))
    {}

    ZSemaphore:: ~ZSemaphore()
    {
        CloseHandle(m_hSemaphore);
    }

    void ZSemaphore::push()
    {
        ReleaseSemaphore(m_hSemaphore, 1, NULL);
    }

    void ZSemaphore::pop()
    {
        WaitForSingleObject(m_hSemaphore, INFINITE);
    }
}