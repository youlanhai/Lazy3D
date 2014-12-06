#include "stdafx.h"
#include "UIDelegate.h"

namespace Lazy
{
    DelegateGroup::DelegateGroup()
        : m_idCounter(0)
    {}

    DelegateGroup::~DelegateGroup()
    {
        clear();
    }

    size_t DelegateGroup::addDelegate(IEventReceiver *p)
    {
        ++m_idCounter;
        m_delegates.push_back(TNode(m_idCounter, p));
        return m_idCounter;
    }

    size_t DelegateGroup::addDelegate(FunctionDelegate::TEventFun pFun)
    {
        return this->addDelegate(new FunctionDelegate(pFun));
    }

    void DelegateGroup::delDelegate(size_t id)
    {
        for (std::vector<TNode>::iterator it = m_delegates.begin();
            it != m_delegates.end(); ++it)
        {
            if (it->first == id)
            {
                m_delegates.erase(it);
                delete it->second;
                return;
            }
        }
    }

    void DelegateGroup::clear()
    {
        while (!m_delegates.empty())
        {
            TNode node = m_delegates.back();
            m_delegates.pop_back();

            delete node.second;
        }
    }

    bool DelegateGroup::onEvent(const SEvent& event)
    {
        bool processed = false;

        for (size_t i = 0; i < m_delegates.size(); ++i)
        {
            processed |= m_delegates[i].second->onEvent(event);
        }

        return processed;
    }

}// end namespace Lazy
