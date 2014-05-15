//该模块实现多线程加载资源
//需要被加载的资源，实现TaskInterface接口，然后作为任务添加到LoadingMgr.

#pragma once

#include "ZThread.h"
#include "TaskMgr.h"
#include "LoadingInterface.h"

namespace Lazy
{
    ///通用的资源加载器。线程共享同一个任务队列，以达到任务平均分配的目的。
    class LoadingMgr : public ILoadingMgr
    {
        LoadingMgr();
        ~LoadingMgr(void);

    public:

        static LoadingMgr * instance();

        ///初始化线程池
        void init(int numThread = 4, int nbMaxSend = -1);

        ///释放线程池
        void fini();

        /** 处理完成任务。此方法调用Task的onTaskFinish方法，以通知调用者。
            注意：此方法请在主线程中调用，以免造成线程不安全。
        */
        void dispatchFinishTask();

        int getDispatchStep() const { return m_dispatchStep; }

    public:

        ///是否正在运行
        virtual bool isRunning() override { return m_running; }

        ///添加任务
        virtual void addTask(TaskPtr task) override;

        ///提取任务
        virtual TaskPtr getTask() override;

        ///添加完成任务
        virtual void addFinishTask(TaskPtr task) override;

    private:
        bool        m_running;
        int         m_nbMaxDispatch;    ///< 每帧最大派发个数
        int         m_dispatchStep;
        TaskMgr     m_workingMgr;
        TaskMgr     m_finishMgr;
        std::vector<ZThreadPtr>   m_threadPool;///< 线程池
    };

}//namespace Lazy