#pragma once

#include "SmartPtr.h"
#include "ZLock.h"

namespace Lazy
{
    class TaskInterface : public IBase
    {
    public:

        TaskInterface();
        ~TaskInterface();

        virtual void doTask(void) = 0;
        virtual void onTaskFinish(void) = 0;
        virtual bool isReady(void){ return true; }
    };

    typedef RefPtr<TaskInterface> TaskPtr;
    typedef RefPtr<class TaskMgr> TaskMgrPtr;


    template<typename T>
    class TaskWithClass : public TaskInterface
    {
    public:
        typedef void(T::*ZFuncPtr)(void);

        TaskWithClass(T* pThis, ZFuncPtr pWork, ZFuncPtr pFinish)
            : m_pThis(pThis)
            , m_pFWork(pWork)
            , m_pFFinish(pFinish)
        {}

        virtual void doTask(void) override
        {
            (m_pThis->*m_pFWork)();
        }

        virtual void onTaskFinish(void) override
        {
            if (m_pFFinish != nullptr)
                (m_pThis->*m_pFFinish)();
        }

    private:
        T*          m_pThis;
        ZFuncPtr    m_pFWork;
        ZFuncPtr    m_pFFinish;
    };

    
    template<typename WorkFun, typename FinishFun>
    class TaskWithFun : public TaskInterface
    {
    public:
        TaskWithFun(WorkFun pWork, FinishFun pFinish)
            : m_pWork(pWork)
            , m_pFinish(pFinish)
        {}

        virtual void doTask(void) override
        {
            if (this->m_pWork != nullptr)
                (this->m_pWork)();
        }

        virtual void onTaskFinish(void) override
        {
            if (this->m_pFinish != nullptr)
                (this->m_pFinish)();
        }

    private:
        WorkFun m_pWork;
        FinishFun m_pFinish;
    };



    ///任务管理器
    class TaskMgr : public IBase
    {
    public:
        typedef std::list<TaskPtr> TaskList;

        TaskMgr(void);
        ~TaskMgr(void);

        ///添加一个任务
        void addTask(TaskPtr task);

        ///如果没有任务，此方法会返回NULL。
        TaskPtr getTask();

    private:
        ZCritical   m_locker;
        TaskList    m_tasks;
    };



}//namespace Lazy
