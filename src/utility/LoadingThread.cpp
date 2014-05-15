#include "stdafx.h"
#include "LoadingThread.h"
#include "Misc.h"

namespace Lazy
{
    LoadingThread::LoadingThread(int id, ILoadingMgr * pMgr)
        : m_id(id)
        , m_pMgr(pMgr)
    { }

    LoadingThread::~LoadingThread()
    { }

    void LoadingThread::run()
    {
        debugMessage(_T("LoadingThread %d start..."), m_id);

        while (m_pMgr->isRunning())
        {
            TaskPtr task = m_pMgr->getTask();
            if (task)
            {
                task->doTask();
                m_pMgr->addFinishTask(task);
                Sleep(10);
            }
            else
            {
                Sleep(100);
            }
        }

        debugMessage(_T("LoadingThread %d exit."), m_id);
    }


}