﻿
#include "stdafx.h"
#include "TerrinMap.h"
#include "Pick.h"
#include "Physics.h"
#include "CursorCamera.h"

#include "Entity.h"
#include "App.h"

#define FLOAT_MAX 999999.0f

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
iEntity::iEntity()
{
    m_id = generateID();
    m_showDistance = 0;
    m_bEnableSphereShow = true;

}

iEntity::~iEntity()
{
    m_model = NULL;
    m_physics = NULL;
    m_topboard = NULL;
    
}

void iEntity::update(float elapse)
{
    if (m_physics) m_physics->update(elapse);
    
    if (m_model)  m_model->update(elapse);
}

void iEntity::render(IDirect3DDevice9* pDevice)
{
    if (!visible()) return ;
    
    if (m_topboard) m_topboard->render(pDevice);
    
    if (m_model)
    {
        m_model->setWorldMatrix(getRotationMatrix());
        m_model->render(pDevice);
    }

#ifdef _DEBUG
    if(m_physics) m_physics->render(pDevice);
    
#endif
}

void iEntity::setPhysics(PhysicsPtr p)
{ 
    if (m_physics == p) return;

    if (m_physics) m_physics->setSource(nullptr);
    m_physics = p; 
    if (m_physics) m_physics->setSource(this);
}

void iEntity::setModel(ModelPtr pModel)
{ 
    m_model = pModel;
    if (m_model) m_model->getTransformdAABB(m_aabb);
}

bool iEntity::isActive(void) const
{
    return EntityMgr::instance()->getActiveEntity() == this;
}

void iEntity::focusCursor(UINT msg)
{
    if (m_model && isEntitySphereEnable())
    {
        if (msg==CM_ENTER || msg==CM_LUP || msg ==CM_RUP)
        {
            m_model->showBound(true);
        }
        else if (msg==CM_LEAVE || msg==CM_LDOWN || msg==CM_RDOWN)
        {
            m_model->showBound(false);
        }
    }
    onFocusCursor(msg);
}

void iEntity::onFocusCursor(UINT)
{

}


float iEntity::distToCamera(void) const
{
    if (!getCamera()) return FLOAT_MAX;
    
    return m_vPos.distTo(getCamera()->position());
}

float iEntity::distToPlayer(void) const
{
    EntityPtr player = EntityMgr::instance()->player();
    if (!player) return FLOAT_MAX;

    return m_vPos.distTo(player->getPos());
}

void iEntity::lookAtPosition(const Math::Vector3 & pos)
{
    m_vLook = pos -  m_vPos;
    m_vLook.normalize();

    m_vUp.set(0, 1, 0);

    m_vUp.cross(m_vRight, m_vLook);
    m_vRight.normalize();
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
    if (msg==WM_LBUTTONDOWN || \
        msg==WM_LBUTTONUP || \
        msg==WM_RBUTTONDOWN || \
        msg==WM_RBUTTONUP)
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

    if (m_player && !m_player->isPlayer())
    {
        LOG_ERROR(L"EntityMgr::setPlayer, entity is not a player!");
    }
}