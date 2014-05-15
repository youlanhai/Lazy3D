#pragma once

namespace Lazy
{

    ///com对象智能指针
    template<class Com>
    class ComPtr
    {
    public:
        ComPtr()
            : m_ptr(nullptr)
        {}

        ComPtr(Com *ptr)
            : m_ptr(ptr)
        {
            SAFE_ADDREF_COM(m_ptr);
        }

        ComPtr(const ComPtr & ptr)
            : m_ptr(ptr)
        {
            SAFE_ADDREF_COM(m_ptr);
        }

        ~ComPtr()
        {
            SAFE_DELREF_COM(m_ptr);
            m_ptr = nullptr;
        }

    public:

        Com & operator * ()
        {
            return *m_ptr;
        }

        Com * operator->()
        {
            return m_ptr;
        }

        const ComPtr & operator = (Com *ptr)
        {
            if (ptr != m_ptr)
            {
                SAFE_ADDREF_COM(ptr);
                if (m_ptr) m_ptr->Release();
                m_ptr = ptr;
            }
            return *this;
        }

        const ComPtr & operator = (const ComPtr & ptr)
        {
            return *this = ptr.m_ptr;
        }

        operator bool()
        {
            return m_ptr != nullptr;
        }


    public:

        Com* get() const { return m_ptr; }

        template<class U>
        U* to(){ return (U*) m_ptr; }

        void addRef(){ SAFE_ADDREF_COM(m_ptr); }
        void delRef(){ SAFE_DELREF_COM(m_ptr); }

    private:
        Com *m_ptr;
    };

}