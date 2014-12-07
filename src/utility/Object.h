#pragma once

namespace Lazy
{

    /** 引用计数基类*/
    class Object
    {
    public:
        Object();
        Object(const Object & o);
        virtual ~Object();

        /** 使用复制操作，可能会导致错误的引用计数，慎用！*/
        const Object & operator = (const Object &)
        {
            return *this;
        }

        /** 增加引用计数*/
        virtual void incRef(void)
        {
            ++m_nRefCounter;
        }

        /** 减少引用计数*/
        virtual void decRef(void)
        {
            if (--m_nRefCounter <= 0)
                destroyThis();
        }

        int getRef(void) const
        {
            return m_nRefCounter;
        }

        bool isRefUnique() const
        {
            return m_nRefCounter == 1;
        }

        virtual void destroyThis(void) 
        { 
            delete this;
        }

    protected:
        long     m_nRefCounter;//引用计数
    };

    /** 引用计数操作线程安全*/
    class SafeObject : public Object
    {
    public:
        SafeObject()
        {}

        virtual ~SafeObject()
        {}

        /** 增加引用计数*/
        virtual void incRef(void)
        {
            InterlockedIncrement(&m_nRefCounter);
        }

        /** 减少引用计数*/
        virtual void decRef(void)
        {
            if (InterlockedDecrement(&m_nRefCounter) <= 0)
                destroyThis();
        }
    };

    //兼容旧接口
    typedef Object IBase;

#define SAFE_ADD_REF(p) if(p) p->incRef()
#define SAFE_DEL_REF(p) if(p) p->decRef()
}
