
#include "stdafx.h"
#include "TerrainMap.h"
#include "Pick.h"
#include "Physics.h"
#include "Camera.h"

#include "Entity.h"
#include "App.h"

#define FLOAT_MAX 999999.0f

namespace Lazy
{

    bool g_entitySphereEnable = true;

    LZDLL_API bool isEntitySphereEnable()
    {
        return g_entitySphereEnable;
    }

    LZDLL_API void setEntitySphereEnable(bool s)
    {
        g_entitySphereEnable = s;
    }

    LZDLL_API UINT convertMsg(UINT w_msg)
    {
        switch(w_msg)
        {
        case  WM_LBUTTONDOWN :
            return CM_LDOWN;

        case WM_LBUTTONUP :
            return CM_LUP;

        case WM_RBUTTONUP:
            return CM_RUP;

        case WM_RBUTTONDOWN:
            return CM_RDOWN;

        case WM_MOUSELEAVE:
            return CM_LEAVE;

        default:
            break;
        }

        return w_msg;
    }

    //////////////////////////////////////////////////////////////////////////
    static int  g_idAllocator = 1000;
    ///生成id
    int generateID(void)
    {
        return g_idAllocator++;
    }

    //////////////////////////////////////////////////////////////////////////
    IEntity::IEntity()
        : m_bEnableSphereShow(true)
        , m_showDistance(0.0f)
        , m_speed(2, 2, 2)
    {
        m_id = generateID();
    }

    IEntity::~IEntity()
    {
        m_model = NULL;
        m_physics = NULL;
        m_topboard = NULL;
    }

    void IEntity::update(float elapse)
    {
        SceneNode::update(elapse);

        if (m_physics)
            m_physics->update(elapse);
    }

    void IEntity::render(IDirect3DDevice9* pDevice)
    {
        SceneNode::render(pDevice);

        if (m_topboard)
            m_topboard->render(pDevice);

#ifdef _DEBUG
        if(m_physics)
            m_physics->render(pDevice);
#endif
    }

    void IEntity::setPhysics(IPhysics* p)
    {
        if (m_physics == p) return;

        if (m_physics) m_physics->setSource(nullptr);
        m_physics = p;
        if (m_physics) m_physics->setSource(this);
    }

    void IEntity::setModel(IModel* pModel)
    {
        if (m_model)
            this->delChild(m_model.get());

        m_model = pModel;
        if (m_model)
        {
            m_aabb = m_model->getWorldBoundingBox();
            this->addChild(m_model.get());
        }
    }

    bool IEntity::isActive(void) const
    {
        return EntityMgr::instance()->getActiveEntity() == this;
    }

    void IEntity::focusCursor(UINT msg)
    {
        if (m_model && isEntitySphereEnable())
        {
            if (msg == CM_ENTER || msg == CM_LUP || msg == CM_RUP)
            {
                m_model->setBBVisible(true);
            }
            else if (msg == CM_LEAVE || msg == CM_LDOWN || msg == CM_RDOWN)
            {
                m_model->setBBVisible(false);
            }
        }
        onFocusCursor(msg);
    }

    void IEntity::onFocusCursor(UINT)
    {

    }


    float IEntity::distToCamera(void) const
    {
        if (!getCamera()) return FLOAT_MAX;

        return getPosition().distTo(getCamera()->getPosition());
    }

    float IEntity::distToPlayer(void) const
    {
        EntityPtr player = EntityMgr::instance()->player();
        if (!player) return FLOAT_MAX;

        return getPosition().distTo(player->getPosition());
    }

    void IEntity::faceToDir(const Vector3 & dir)
    {
        float angleDir = atan2f(dir.z, dir.x);
        float angleLook = D3DX_PI * 0.5f;

        Quaternion rotation;
        rotation.setRotationAxis(MathConst::vec3y, angleLook - angleDir);
        setRotation(rotation);
    }

    //////////////////////////////////////////////////////////////////////////

    struct PredicateID
    {
        PredicateID(int id)
            : m_id(id)
        {}

        bool operator()(const EntityPtr & ent) const
        {
            return ent->getID() == m_id;
        }

    private:
        int m_id;

    };

    //////////////////////////////////////////////////////////////////////////
    ///
    //////////////////////////////////////////////////////////////////////////
    /*static*/ EntityMgr * EntityMgr::instance()
    {
        static EntityMgr s_mgr;
        return &s_mgr;
    }

    EntityMgr::EntityMgr(void)
    {
    }

    EntityMgr::~EntityMgr(void)
    {
        m_pool.clear();
    }

    void EntityMgr::add(EntityPtr pEnt)
    {
        m_pool.add(pEnt);
    }

    void EntityMgr::removeById(int id)
    {
        EntityPtr ent = entity(id);
        if (ent) remove(ent);
    }

    void EntityMgr::remove(EntityPtr pEnt)
    {
        if (pEnt == m_pActiveEntity)
        {
            m_pActiveEntity = nullptr;
        }

        if (pEnt == m_player)
        {
            m_player = nullptr;
        }

        m_pool.remove(pEnt);
    }

    void EntityMgr::clear()
    {
        m_pActiveEntity = nullptr;
        m_player = nullptr;
        m_pool.clear();
    }

    EntityPtr EntityMgr::entity(int id)
    {
        PoolIterator it = m_pool.find_if(PredicateID(id));
        if (it == m_pool.end()) return nullptr;

        return *it;
    }

    void EntityMgr::setActiveEntity(EntityPtr pEnt)
    {
        if (m_pActiveEntity == pEnt)  return;

        if (m_pActiveEntity)
        {
            m_pActiveEntity->focusCursor(CM_LEAVE);
        }

        m_pActiveEntity = pEnt;

        if (m_pActiveEntity)
        {
            m_pActiveEntity->focusCursor(CM_ENTER);
        }
    }

    UINT EntityMgr::handleMouseEvent(UINT msg, WPARAM, LPARAM)
    {
        if (msg == WM_LBUTTONDOWN || \
                msg == WM_LBUTTONUP || \
                msg == WM_RBUTTONDOWN || \
                msg == WM_RBUTTONUP)
        {
            if (getActiveEntity() != NULL)
            {
                getActiveEntity()->focusCursor(convertMsg(msg));
                return 1;
            }
        }
        return 0;
    }

    void EntityMgr::update(float fElapse)
    {
        m_pool.lock();

        for (EntityPtr ent : m_pool)
        {
            ent->update(fElapse);
        }

        m_pool.unlock();
    }

    void EntityMgr::render(IDirect3DDevice9 * pDevice)
    {
        m_pool.lock();

        for (EntityPtr ent : m_pool)
        {
            ent->render(pDevice);
        }

        m_pool.unlock();
    }

    void EntityMgr::setPlayer(EntityPtr p)
    {
        m_player = p;

        if (m_player && !m_player->ifPlayer())
        {
            LOG_ERROR(L"EntityMgr::setPlayer, entity is not a player!");
        }
    }

} // end namespace Lazy
