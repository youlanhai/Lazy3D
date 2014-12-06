#pragma once

#include "../utility/Event.h"
#include <functional>

namespace Lazy
{

    class FunctionDelegate : public IEventReceiver
    {
    public:
        typedef bool(*TEventFun)(const SEvent &);

        FunctionDelegate(TEventFun pFun)
            : m_pFun(pFun)
        {}

        virtual bool onEvent(const SEvent& event)
        {
            return m_pFun(event);
        }

    private:
        TEventFun m_pFun;
    };

    template<typename T>
    class MethodDelegate : public IEventReceiver
    {
    public:
        typedef bool(T::*TEventMethod)(const SEvent &);

        MethodDelegate(T * pOwner, TEventMethod pMethod)
            : m_pOwner(pOwner)
            , m_pMethod(pMethod)
        {}

        virtual bool onEvent(const SEvent& event)
        {
            return (m_pOwner->*m_pMethod)(event);
        }

    private:
        T * m_pOwner;
        TEventMethod m_pMethod;
    };

    class DelegateGroup : public IEventReceiver
    {
    public:
        DelegateGroup();
        ~DelegateGroup();

        size_t addDelegate(IEventReceiver *p);

        size_t addDelegate(FunctionDelegate::TEventFun pFun);
        
        template<typename T>
        size_t addDelegate(T * pOwner, bool(T::*pMethod)(const SEvent &))
        {
            return this->addDelegate(new MethodDelegate<T>(pOwner, pMethod));
        }

        void delDelegate(size_t id);

        void clear();

        virtual bool onEvent(const SEvent& event);

    private:
        size_t m_idCounter;

        typedef std::pair<size_t, IEventReceiver *> TNode;
        std::vector<TNode> m_delegates;
    };


} // end namespace Lazy