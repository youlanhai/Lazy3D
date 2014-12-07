#pragma once

struct _object;

namespace Lazy
{

    /** ���ü�������*/
    class Object
    {
    public:
        Object();
        Object(const Object & o);
        virtual ~Object();

        /** ʹ�ø��Ʋ��������ܻᵼ�´�������ü��������ã�*/
        const Object & operator = (const Object &)
        {
            return *this;
        }

        /** �������ü���*/
        virtual void incRef(void)
        {
            ++m_nRefCounter;
        }

        /** �������ü���*/
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

        void setScript(struct _object *p)
        {
            m_pScript = p;
        }

        struct _object * getScript() const
        {
            return const_cast<Object*>(this)->m_pScript;
        }

    protected:
        long                m_nRefCounter;  ///<���ü���
        struct _object*     m_pScript; ///< �ű�����ָ��
    };

    /** ���ü��������̰߳�ȫ*/
    class SafeObject : public Object
    {
    public:
        SafeObject()
        {}

        virtual ~SafeObject()
        {}

        /** �������ü���*/
        virtual void incRef(void)
        {
            InterlockedIncrement(&m_nRefCounter);
        }

        /** �������ü���*/
        virtual void decRef(void)
        {
            if (InterlockedDecrement(&m_nRefCounter) <= 0)
                destroyThis();
        }
    };

    //���ݾɽӿ�
    typedef Object IBase;

#define SAFE_ADD_REF(p) if(p) p->incRef()
#define SAFE_DEL_REF(p) if(p) p->decRef()
}
