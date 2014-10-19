#pragma once

#include "Base.h"
#include "../utility/Singleton.h"

#include <queue>
#include <functional>

namespace Lazy
{

    class LZDLL_API ITimerDelegate : public IBase
    {
    public:
        virtual void onCall() = 0;
        virtual void onCancel() {}
        virtual void onExit() {}
    };

    typedef RefPtr<ITimerDelegate> TimerDelegatePtr;

    typedef double TTimeValue;
    typedef uint64 TTimeHandle;

    class TimerNode
    {
    public:
        TimerNode(TTimeValue time, TTimeHandle handle, TimerDelegatePtr delegate_)
            : m_time(time)
            , m_handle(handle)
            , m_delegate(delegate_)
        {}

        bool operator < (const TimerNode & node) const
        {
            if (m_time == node.m_time)
                return m_handle < node.m_handle;

            return m_time < node.m_time;
        }

    private:
        TTimeValue          m_time;
        TTimeHandle         m_handle;
        TimerDelegatePtr    m_delegate;

        friend class TimerMgr;
    };


    /** 定时器*/
    class LZDLL_API TimerMgr : public Singleton<TimerMgr>
    {
    public:
        typedef std::priority_queue<TimerNode>   Timers;

        TimerMgr();
        ~TimerMgr();

        TTimeHandle addTimer(float time, TimerDelegatePtr delegate_);
        TTimeHandle addCallback(float time, const std::function<void()> & callback);
        void        remove(TTimeHandle handle);

        void        update(float elapse);

    private:
        TTimeValue      m_time;
        TTimeHandle     m_idAllocator;
        Timers          m_timers;
        std::set<TTimeHandle> m_deleted;
    };

}
