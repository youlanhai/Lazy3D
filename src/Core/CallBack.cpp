
#include "stdafx.h"
#include "CallBack.h"

namespace Lazy
{


    namespace
    {
        struct PredDead
        {
            bool operator()(CallObjPtr ptr)
            {
                return ptr->isDead();
            }
        };
    }


    /*static*/ CallbackMgr * CallbackMgr::instance()
    {
        static CallbackMgr s_mgr;
        return &s_mgr;
    }

    CallbackMgr::CallbackMgr()
        : m_idAllocator(1)
        , m_updating(false)
    {

    }



    size_t CallbackMgr::add(CallObjPtr call)
    {
        assert(call && "CallbackMgr::add");

        size_t id = m_idAllocator++;

        call->m_id = id;
        if (m_updating) m_cbCache.push_back(call);
        else m_cbPool.push_back(call);

        return id;
    }

    //仅标记为死亡态，并不真正删除。
    void CallbackMgr::remove(size_t id)
    {
        removeCB(m_cbPool, id);
        removeCB(m_cbCache, id);
    }

    void CallbackMgr::update(float elapse)
    {
        m_updating = true;

        for (CallObjPtr call : m_cbPool)
        {
            if (call->isDead()) continue;

            call->update(elapse);

            if (call->isDead()) call->onDead();
        }

        m_cbPool.merge(m_cbCache);
        m_cbCache.clear();

        m_cbPool.remove_if(PredDead());

        m_updating = false;
    }

    void CallbackMgr::removeCB(CallBackPool & pool, size_t id)
    {
        for (CallObjPtr call : pool)
        {
            if (call->m_id == id)
            {
                call->m_time = -1.0f;
                return;
            }
        }
    }

}