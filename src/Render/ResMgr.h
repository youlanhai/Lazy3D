#pragma once

#include "Resource.h"

namespace Lazy
{

    template<typename T, typename ResType>
    class ResMgr
    {
        MAKE_UNCOPY(ResMgr)

    public:
        typedef std::map<tstring, ResType> ResPool;

        ResMgr(){}

        ///单例
        static T * instance();

        void setResFacotry(ResourceFactoryPtr factory){ m_resFactory = factory; }
        ResourceFactoryPtr getResFacotry(){ return m_resFactory; }

        ///获取资源
        ResType get(const tstring & name);

        virtual int getType() = 0;

    protected:

        ///加载资源
        ResType load(const tstring & fname);

    protected:
        ResPool             m_pool;
        ResourceFactoryPtr  m_resFactory;

#ifdef USE_MULTI_THREAD
        ///如果开启了多线程加载。必须对容器m_resourses进行加锁。
        Lazy::ZCritical     m_locker;
#else
        Lazy::DummyLock     m_locker;
#endif
    };


    template<typename T, typename ResType>
    /*static*/ T * ResMgr<T, ResType>::instance()
    {
        static T s_mgr;
        return &s_mgr;
    }

    template<typename T, typename ResType>
    ResType ResMgr<T, ResType>::get(const tstring & name)
    {
        if (name.empty()) return nullptr;

        ZLockHolder holder(&m_locker);

        ResPool::iterator it = m_pool.find(name);
        if (it == m_pool.end())
        {
            ResType pRes = load(name);
            m_pool[name] = pRes;
            return pRes;
        }

        return it->second;
    }

    template<typename T, typename ResType>
    ResType ResMgr<T, ResType>::load(const tstring & fname)
    {
        ResType ptr = m_resFactory->create(this->getType(), fname);
        if (!ptr) return nullptr;

        if (!ptr->load())
        {
            LOG_ERROR(L"Failed to load resource '%s' ", fname.c_str());
            return nullptr;
        }

        return ptr;
    }


}//namespace Lazy