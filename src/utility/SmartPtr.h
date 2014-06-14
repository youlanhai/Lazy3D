//SmartPtr.h 智能指针
// author: 游蓝海  
// blog: http://blog.csdn.net/you_lan_hai

#ifndef LAZY3D_SMARTPTR_H
#define LAZY3D_SMARTPTR_H


namespace Lazy
{

    /** 引用计数类*/
    class IBase
    {
    public:
        IBase(void);
        IBase(const IBase &);
        virtual ~IBase(void);

        const IBase & operator = (const IBase &);

        /** 增加引用计数*/
        void addRef(void);

        /** 减少引用计数*/
        void delRef(void);

        /** 获得引用计数*/
        int getRef(void) const;

        bool isRefUnique() const;

        virtual void destroyThis(void){ delete this; }

    private:
        int     m_nRefCounter_;//引用计数
    };

#define SAFE_ADD_REF(p) if(p) p->addRef()
#define SAFE_DEL_REF(p) if(p) p->delRef()


    /** 侵入式智能指针*/
    template<typename T>
    class RefPtr
    {
    public:
        typedef T type;
        typedef RefPtr<type> ThisType;

        RefPtr()
            : m_ptr(nullptr)
        {
        }

        RefPtr(type* ptr)
            : m_ptr(ptr)
        {
            safeAddRef(m_ptr);
        }

        RefPtr(const ThisType & ptr)
            : m_ptr(ptr.m_ptr)
        {
            safeAddRef(m_ptr);
        }

        template<class U>
        RefPtr(const RefPtr<U> & ptr)
        {
            m_ptr = dynamic_cast<T*>(ptr.get());
            safeAddRef(m_ptr);
        }

        ~RefPtr(void)
        {
            safeDelRef(m_ptr);
        }

    public:

        type* operator->() const
        {
            assert(m_ptr && "smartptr is null!");

            return m_ptr;
        }

        type& operator*() const
        {
            assert(m_ptr && "smartptr is null!");

            return *m_ptr;
        }

        operator bool() const
        {
            return (m_ptr != nullptr);
        }

        type* get() const
        {
            return m_ptr;
        }

        template<class U>
        U* cast() const
        {
            if (m_ptr == NULL) return NULL;

            U* p = dynamic_cast<U*>(m_ptr);
            assert(p != NULL && "can't cast type smartptr to U!");
            return p;
        }

        const ThisType & operator=(type* ptr)
        {
            if (ptr != m_ptr) //防止自复制
            {
                safeAddRef(ptr);
                safeDelRef(m_ptr);
                m_ptr = ptr;
            }
            return *this;
        }

        const ThisType & operator=(const ThisType & ptr)
        {
            return *this = ptr.m_ptr;
        }

        ///强制类型转换
        template<typename U>
        const ThisType & operator=(U* ptr)
        {
            if (ptr == NULL)
                return *this = (type*) NULL;

            type *p = dynamic_cast<type*>(ptr);
            assert(p != NULL && "can't cast type U to smartptr!");
            return *this = p;
        }

        ///强制类型转换
        template<typename U>
        const ThisType & operator=(const RefPtr<U> & ptr)
        {
            return *this = ptr.get();
        }

        bool operator < (const ThisType & v) const
        {
            return m_ptr < v.m_ptr;
        }

        bool operator >(const ThisType & v) const
        {
            return m_ptr > v.m_ptr;
        }

    private:

        inline void safeAddRef(type * ptr)
        {
            if (ptr != nullptr) ptr->addRef();
        }

        inline void safeDelRef(type * ptr)
        {
            if (ptr != nullptr) ptr->delRef();
        }

        type* m_ptr;
    };

    template<typename T, typename U>
    bool operator==(const RefPtr<T>& a, const RefPtr<U>& b)
    {
        return (a.get() == b.get());
    }

    template<typename T>
    bool operator==(const RefPtr<T>& a, const T* b)
    {
        return (a.get() == b);
    }

    template<typename T>
    bool operator==(const T* a, const RefPtr<T>& b)
    {
        return (a == b.get());
    }


    template<typename T, typename U>
    bool operator!=(const RefPtr<T>& a, const RefPtr<U>& b)
    {
        return (a.get() != b.get());
    }

    template<typename T>
    bool operator!=(const RefPtr<T>& a, const T* b)
    {
        return (a.get() != b);
    }

    template<typename T>
    bool operator!=(const T* a, const RefPtr<T>& b)
    {
        return (a != b.get());
    }

}//namespace Lazy

#endif //LAZY3D_SMARTPTR_H