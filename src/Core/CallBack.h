#pragma once

#include "Base.h"
#include "RenderObj.h"

namespace Lazy
{

    ///回调对象基类。
    class LZDLL_API CallableNode: public IBase
    {
    public:
        explicit CallableNode(float time)
            : m_time(time)
            , m_id(0)
        {}

        void update(float elapse)
        {
            m_time -= elapse;
        }

        size_t id() const { return m_id; }
        bool isDead() const { return m_time <= 0.0f; }

        virtual void onDead(void) = 0;

    private:
        size_t  m_id;
        float   m_time;

        friend class CallbackMgr;
    };

    typedef RefPtr<CallableNode> CallObjPtr;


    ///回调管理器
    class LZDLL_API CallbackMgr : public IBase, public IUpdate
    {
    public:
        typedef std::list<CallObjPtr>   CallBackPool;
        typedef CallBackPool::iterator  CallBackIter;

        static CallbackMgr * instance();

        size_t add(CallObjPtr call);

        void remove(size_t id);

        virtual void update(float elapse) override;

    private:

        CallbackMgr();

        void removeCB(CallBackPool & pool, size_t id);

    private:
        int             m_idAllocator;
        bool            m_updating;
        CallBackPool    m_cbPool;
        CallBackPool    m_cbCache;
    };

    //////////////////////////////////////////////////////////////////////////
    ///默认回调函数结构
    template<typename Type>
    class cDefaultCall: public CallableNode
    {
    public:
        cDefaultCall(float time, Type call)
            : CallableNode(time)
            , m_callBack(call)
        {}

        virtual void onDead()
        {
            m_callBack();
        }

    protected:
        Type m_callBack;
    };


    template<typename Type>
    size_t callBack(float time, Type call)
    {
        return CallbackMgr::instance()->add(new cDefaultCall<Type>(time, call));
    }

    //////////////////////////////////////////////////////////////////////////
}
