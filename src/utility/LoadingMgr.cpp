#include "StdAfx.h"
#include "LoadingMgr.h"
#include "Misc.h"
#include "LoadingThread.h"

namespace
{
    int NbMaxThread = 32767;
}

//#define DISABLE_MULTI_THREAD

namespace Lazy
{
    /*static*/ LoadingMgr * LoadingMgr::instance()
    {
        static LoadingMgr s_instance;
        return &s_instance;
    }

    LoadingMgr::LoadingMgr()
        : m_nbMaxDispatch(-1)
        , m_running(false)
        , m_dispatchStep(0)
    {
    }


    LoadingMgr::~LoadingMgr(void)
    {
    }

    void LoadingMgr::init(int numThread/* = 4*/, int maxSend/*=-1*/)
    {
        debugMessage(_T("LoadingMgr 0x%8.8x init."), this);

        if (m_running)
        {
            throw(std::runtime_error("Must call fini first!"));
        }

        if (numThread <= 0 || numThread > NbMaxThread)
        {
            throw(std::invalid_argument("numThread is too large or small!"));
        }

        m_nbMaxDispatch = maxSend;
        m_running = true;

        if (m_nbMaxDispatch < 0)  m_nbMaxDispatch = 0x7fffffff;

        m_threadPool.resize(numThread);
        for (int i = 0; i < numThread; ++i)
        {
            m_threadPool[i] = new LoadingThread(i, this);
        }
    }

    void LoadingMgr::fini()
    {
        m_running = false;

        m_threadPool.clear();

        debugMessage(_T("LoadingMgr 0x%8.8x fini."), this);
    }


    void LoadingMgr::dispatchFinishTask()
    {
        ++m_dispatchStep;

        for (int i = 0; i < m_nbMaxDispatch; ++i)
        {
            TaskPtr task = m_finishMgr.getTask();
            if (!task) break; //已经没有任务了
            
            task->onTaskFinish();
        }
    }

    TaskPtr LoadingMgr::getTask()
    {
        return m_workingMgr.getTask();
    }

    void LoadingMgr::addTask(TaskPtr task)
    {
        if (!task) throw(std::invalid_argument("Task must not be null!"));

#ifdef DISABLE_MULTI_THREAD
        task->doTask();
        task->onTaskFinish();
        return;
#endif

        m_workingMgr.addTask(task);
    }

    void LoadingMgr::addFinishTask(TaskPtr task)
    {
        if (!task) throw(std::invalid_argument("Finished task must not be null!"));

        m_finishMgr.addTask(task);
    }


}//namespace Lazy