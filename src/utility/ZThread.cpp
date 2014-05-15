//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ZThread.h"

namespace Lazy
{

    /*static*/ uint32 __stdcall ZThreadBase::_threadCall(void* param)
    {
        ZThreadBase *pThis = (ZThreadBase*) param;
        pThis->run();
        return 0;
    }

    ZThreadBase::ZThreadBase(bool start/* = true*/)
    {
        uint32 flag = 0;
        if (!start) flag = CREATE_SUSPENDED;
        
        m_hThread = (HANDLE) _beginthreadex(NULL, 0, _threadCall, this, flag, NULL);
    }

    ZThreadBase::~ZThreadBase(void)
    {
        WaitForSingleObject(m_hThread, INFINITE);
        CloseHandle(m_hThread);
    }

    void ZThreadBase::start()
    {
        ResumeThread(m_hThread);
    }
    void ZThreadBase::pause()
    {
        SuspendThread(m_hThread);
    }

}