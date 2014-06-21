#pragma once

#include "ZLock.h"
#include "Singleton.h"

namespace Lazy
{

    //构造函数中无法得知对象的真实类型，所以需要延迟获取类型
    class ITypeProxy
    {
    public:
        ITypeProxy(void *p)
            : m_ptr(p)
        {}

        virtual ~ITypeProxy()
        {}

        virtual std::string getTypeName() = 0;

        void * get() const { return m_ptr; }

    protected:
        void *m_ptr;
    };

    template<typename T>
    class ImpTypeProxy : public ITypeProxy
    {
    public:
        ImpTypeProxy(T * p)
            : ITypeProxy(p)
        {}

        virtual ~ImpTypeProxy()
        {}

        virtual std::string getTypeName() override
        {
            return typeid(*((T*) m_ptr)).name();
        }
    };


    //统计实例个数，以校验那些类型的对象出现内存泄漏
    class MemoryChecker :
        public Singleton<MemoryChecker>
    {
    public:

        MemoryChecker();
        ~MemoryChecker();

        template<typename T>
        void addObj(T *p)
        {
            ZLockHolder holder(&m_lock);
            m_objects.insert(new ImpTypeProxy<T>(p));
        }

        template<typename T>
        void delObj(T *p)
        {
            ZLockHolder holder(&m_lock);

            auto cmpFun = [p](ITypeProxy * v)->bool{ return v->get() == p; };

            auto it = std::find_if(m_objects.begin(), m_objects.end(), cmpFun);
            if (it != m_objects.end())
            {
                ITypeProxy * p = *it;
                m_objects.erase(it);

                //delete p;
            }
        }

        void dumpAll();

    private:

        std::set<ITypeProxy*> m_objects;
        ZCritical m_lock;
    };

#ifdef XXXXXXXXXXXX
#   define MEMORY_CHECK_CONS(p) \
    if(::Lazy::MemoryChecker::hasInstance()) ::Lazy::MemoryChecker::instance()->addObj(p)

#   define MEMORY_CHECK_DEST(p) \
    if (::Lazy::MemoryChecker::hasInstance()) ::Lazy::MemoryChecker::instance()->delObj(p)
#else
#   define MEMORY_CHECK_CONS(p)
#   define MEMORY_CHECK_DEST(p)
#endif

}//namespace Lazy