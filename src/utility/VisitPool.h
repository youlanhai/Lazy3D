#pragma once

#include <list>

#include "SmartPtr.h"

namespace Lazy
{

    namespace OPCode
    {
        const int addFront      = 1;
        const int addBack       = 2;
        const int del           = 3;
        const int clear         = 4;
    }


    ///安全访问数据。注意：该类不支持多线程。
    template<typename VisitType>
    class VisitPool : public IBase
    {
    public:

        //缓冲结点
        struct CacheNode
        {
            VisitType   m_obj;
            int         m_opCode;

            CacheNode()
            {}

            CacheNode(int code)
                : m_opCode(code)
            {}

            CacheNode(VisitType obj, int code)
                : m_obj(obj), m_opCode(code)
            {}
        };

        typedef std::list<CacheNode> CacheList;

    public:

        typedef std::list<VisitType>                    Container;
        typedef typename Container::iterator            iterator;
        typedef typename Container::const_iterator      const_iterator;
        typedef typename Container::reverse_iterator    reverse_iterator;
        typedef typename Container::const_reverse_iterator    const_reverse_iterator;

        VisitPool() : m_lockIndex(0)
        {
        }

        ~VisitPool()
        {
            destroy();
        }

        ///查找
        iterator find(const VisitType & obj)
        {
            return std::find(begin(), end(), obj);
        }

        ///根据条件查找
        template<typename Pred>
        iterator find_if(Pred pred)
        {
            return std::find_if(begin(), end(), pred);
        }

        template<typename Pred>
        void sort(Pred pred)
        {
            m_pool.sort(pred);
        }

        ///添加一个元素
        void addFront(const VisitType & obj)
        {
            if (isLocking())
            {
                m_cache.push_back(CacheNode(obj, OPCode::addFront));
            }
            else
            {
                m_pool.push_front(obj);
            }
        }

        ///添加一个元素
        void addBack(const VisitType & obj)
        {
            if (isLocking())
            {
                m_cache.push_back(CacheNode(obj, OPCode::addBack));
            }
            else
            {
                m_pool.push_back(obj);
            }
        }

        void add(const VisitType & obj)
        {
            addBack(obj);
        }

        ///移除一个元素
        void remove(const VisitType & obj)
        {
            iterator it = find(obj);
            if (it == end()) return;

            if(isLocking())
            {
                m_cache.push_back(CacheNode(obj, OPCode::del));
            }
            else
            {
                m_pool.erase(it);
            }
        }

        void clear()
        {
            if (isLocking())
            {
                m_cache.push_back(CacheNode(OPCode::clear));
            }
            else
            {
                m_pool.clear();
            }
        }

        void destroy()
        {
            m_lockIndex = 0;
            m_cache.clear();
            m_pool.clear();
        }

        ///刷新状态。
        void refreshState(void);

        ///锁定链表。进行迭代操作之前一定要先加锁。
        void lock()
        {
            ++m_lockIndex;
        }

        ///解除锁定。迭代完毕后解锁。
        void unlock()
        {
            assert(m_lockIndex > 0 && "invalid unlock!");

            --m_lockIndex;

            refreshState();
        }

        ///是否锁定
        inline bool isLocking(void) const { return m_lockIndex > 0; }

        inline iterator begin(void) { return m_pool.begin(); }

        inline iterator end(void) { return m_pool.end(); }

        inline const_iterator begin(void) const { return m_pool.begin(); }

        inline const_iterator end(void) const { return m_pool.end(); }

        inline reverse_iterator rbegin(void) { return m_pool.rbegin(); }

        inline reverse_iterator rend(void) { return m_pool.rend(); }

        inline bool empty() const { return m_pool.empty(); }

    private:
        Container   m_pool;
        CacheList   m_cache;

        int         m_lockIndex;//锁定计数。防止在遍历的时候进行删除和添加操作而造成错误。
    };

    template<typename VisitType>
    void VisitPool<VisitType>::refreshState(void)
    {
        if (isLocking()) return;
        if (empty()) return;

        for (CacheList::iterator it = m_cache.begin(); it != m_cache.end(); ++it)
        {
            switch (it->m_opCode)
            {
            case OPCode::addFront:
                m_pool.push_front(it->m_obj);
                break;

            case OPCode::addBack:
                m_pool.push_back(it->m_obj);
                break;

            case OPCode::del:
                m_pool.remove(it->m_obj);
                break;

            case OPCode::clear:
                m_pool.clear();
                break;

            default:
                break;
            }
        }
        m_cache.clear();
    }

}//namespace Lazy