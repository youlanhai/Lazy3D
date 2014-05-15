#pragma once

#include "TaskMgr.h"

namespace Lazy
{

    struct ILoadingMgr
    {
        ///是否正在运行
        virtual bool isRunning() = 0;

        ///添加任务
        virtual void addTask(TaskPtr task) = 0;

        ///提取任务
        virtual TaskPtr getTask() = 0;

        ///添加完成任务
        virtual void addFinishTask(TaskPtr task) = 0;
    };
}