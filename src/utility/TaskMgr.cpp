#include "StdAfx.h"
#include "TaskMgr.h"
#include "Misc.h"

namespace Lazy
{

    TaskInterface::TaskInterface()
    {}

    TaskInterface::~TaskInterface()
    {}



    TaskMgr::TaskMgr(void)
    {
    }


    TaskMgr::~TaskMgr(void)
    {
    }

    void TaskMgr::addTask(TaskPtr task)
    {
        m_locker.lock();

        m_tasks.push_back(task);

        m_locker.unlock();
    }

    //如果没有任务，此方法会返回NULL。
    TaskPtr TaskMgr::getTask()
    {
        ZLockHolder holder(&m_locker);

        TaskList::iterator it = m_tasks.begin();
        for (; it != m_tasks.end(); ++it)
        {
            if ((*it)->isReady())
            {
                break;
            }
        }
        if (it == m_tasks.end())
        {
            return NULL;
        }

        TaskPtr task = *it;
        m_tasks.erase(it);

        return task;
    }

}//namespace Lazy