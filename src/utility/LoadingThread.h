#pragma once

#include "ZThread.h"
#include "LoadingInterface.h"

namespace Lazy
{
    ///任务加载资源线程
    class LoadingThread : public ZThreadBase
    {
    public:
        LoadingThread(int id, ILoadingMgr * pMgr);
        ~LoadingThread();

        virtual void run() override;

    private:
        int          m_id;      ///< 标识id
        ILoadingMgr  *m_pMgr;   ///< loading管理器
    };

}