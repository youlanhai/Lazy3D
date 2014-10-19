
#include "stdafx.h"
#include "Timer.h"
#include "Log.h"

namespace Lazy
{
    
    class FunctionTimerDelegate : public ITimerDelegate
    {
    public:
        FunctionTimerDelegate(std::function<void()> callback)
            : m_callback(callback)
        {}

        virtual void onCall() override
        {
            m_callback();
        }

    protected:
        std::function<void()> m_callback;
    };

    IMPLEMENT_SINGLETON(TimerMgr);
    TimerMgr::TimerMgr()
        : m_idAllocator(0)
        , m_time(0)
    {

    }

    TimerMgr::~TimerMgr()
    {}

    TTimeHandle TimerMgr::addTimer(float time, TimerDelegatePtr delegate_)
    {
        TTimeHandle handle = ++m_idAllocator;
        if (handle == 0)
        {
            LOG_ERROR(L"TimerMgr::addTimer - the handle is overflow!");
            handle = ++m_idAllocator;
        }

        m_timers.push(TimerNode(m_time + time, handle, delegate_));
        return handle;
    }

    TTimeHandle TimerMgr::addCallback(float time, const std::function<void()> & callback)
    {
        return addTimer(time, new FunctionTimerDelegate(callback));
    }

    void TimerMgr::remove(TTimeHandle handle)
    {
        m_deleted.insert(handle);
    }

    void TimerMgr::update(float elapse)
    {
        m_time += elapse;

        while (!m_timers.empty() && m_timers.top().m_time <= m_time)
        {
            TimerNode node = m_timers.top();
            m_timers.pop();

            std::set<TTimeHandle>::iterator it = m_deleted.find(node.m_handle);
            if (it != m_deleted.end())
            {
                m_deleted.erase(it);
                node.m_delegate->onCancel();
            }
            else
            {
                node.m_delegate->onCall();
            }
            node.m_delegate->onExit();
        }
    }

} // end namespace Lazy

