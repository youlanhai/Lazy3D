
#include "stdafx.h"
#include "I3DObject.h"
#include "CursorCamera.h"
#include "TerrinMap.h"
#include "App.h"

#include "Physics.h"
#include "Collision.h"

float g_maxDistToNpc = 300.0f;
bool g_drawCollision = false;

const float ModelWidth  = 40.0f;
const float ModelHeight = 80.0f;
const float ModelDepth  = 30.0f;
const float ClimbHeight = 20.0f;

const float JumpUpSpeed = 800.0f;
const float JumpDownSpeed = 400.0f;
const float JumpDuration = 0.4f;

const float heighError = 5.0f; //高度误差值

const Physics::CollisionConfig CConfig(ModelWidth, ModelHeight, ModelDepth, ClimbHeight);

Physics::CollisionPrevent g_debugCP(Physics::Vector3(0, 0, 0), 
    Physics::Vector3(10, 10, 10), false, CConfig);
Physics::CollisionPrevent g_debugCP2 = g_debugCP;


LZDLL_API void enableDrawCollision(bool e)
{
    g_drawCollision = e;
}

LZDLL_API bool isDrawCollisionEnable()
{
    return g_drawCollision;
}

LZDLL_API float getMaxDistToNpc()
{
    return g_maxDistToNpc;
}

LZDLL_API void setMaxDistToNpc(float dist)
{
    g_maxDistToNpc = dist;
}


iPhysics::iPhysics(void)
{
    new(this)iPhysics(NULL);
    
}

iPhysics::iPhysics(I3DObject* pSource)
    : m_pSource(pSource)
{
    m_enable = true;
    m_autoMove = false;

    m_bMoveToEntity = false;
    m_pMoveToTarget = NULL;
    m_state = PS_FREE;

    m_aiElapse = 0;
    m_aiInterval = 1.0;
    m_aiEnable = false;

    m_lockHInMap = true;
    m_lockHeight = 0.0f;

    m_isJumping = false;
    m_jumpingTime = 0.0f;
    m_collid = false;
    m_isFaling = false;

    m_moveByPath = false;

}

iPhysics::~iPhysics(void)
{
}

void iPhysics::render(IDirect3DDevice9 * pDevice)
{
    if(isPlayers() && g_drawCollision)
    {
        DWORD oldLight, oldAlph;
        pDevice->GetRenderState(D3DRS_LIGHTING, &oldLight);
        pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

        pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &oldAlph);
        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

        g_debugCP.drawDebug(pDevice);
        g_debugCP2.drawDebug(pDevice);

        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, oldAlph);
        pDevice->SetRenderState(D3DRS_LIGHTING, oldLight);
    }
}

void iPhysics::update(float fElapse)
{
    if (!isEnabled() || !m_pSource)
    {
        return ;
    }

    if(isPlayers())
    {
        updatePlayer(fElapse);
    }

    if(m_aiEnable)
    {
        m_aiElapse += fElapse;
        if (m_aiElapse >= m_aiInterval)
        {
            m_aiElapse = 0;
            onAITrigger();
        }
    }

    if (m_bMoveToEntity && m_pMoveToTarget)
    {
        m_autoMove = true;
        moveTo(m_pMoveToTarget->getPos());
    }

    updateAutoMove(fElapse);

    if(!m_autoMove && m_moveByPath)
    {
        updateByPath();
    }

    updateHeight(fElapse);
}

bool iPhysics::isPlayers()
{ 
    if (m_pSource) return m_pSource->isPlayer();

    return false; 
}

void iPhysics::updatePlayer(float fElapse)
{
    if (getCamera()->getCameraType() == CCamera::FREE)
    {
        return;
    }
    else if (getCamera()->getCameraType() == CCamera::FIRST)
    {
        m_pSource->show(false);
    }

    RefPtr<CKeyboard> keyboard = getApp()->getKeyboard();

    bool moved = true;
    m_collid = false;

    if (keyboard->isKeyPress(VK_UP))
    {
        m_pSource->m_vPos.y += JumpUpSpeed * fElapse;
    }
    else if (keyboard->isKeyPress(VK_DOWN))
    {
        m_pSource->m_vPos.y -= JumpUpSpeed * fElapse;
    }

    if (!m_isFaling && !m_isJumping && keyboard->isKeyDown(VK_SPACE))
    {
        m_isJumping = true;
        m_jumpingTime = 0.0f;
    }

    if (m_jumpingTime >= JumpDuration)
    {
        if (m_isJumping)
        {
            m_isFaling = true;
        }
        m_isJumping = false;
    }

    if (m_isJumping)
    {
        float upTime = fElapse;
        if (m_jumpingTime + upTime > JumpDuration)
        {
            upTime = JumpDuration - m_jumpingTime;
        }

        if (upTime > 0.0f)
        {
            m_jumpingTime += upTime;
            //m_pSource->m_vPos.y += JumpUpSpeed * upTime;

            Physics::Vector3 start = m_pSource->m_vPos;
            start.y += ModelHeight;
            Physics::Vector3 end = start;
            end.y += JumpUpSpeed * upTime;
                
            //降低起点，防止钻入模型。
            start.y -= heighError;

            Physics::CollisionConfig config(ModelWidth, ModelWidth, 0, 0);
            Physics::CollisionPrevent cp(start, end, true, config);
            if (Physics::preventCollision(cp))
            {
                float distance = max(0, cp.m_distance - heighError);

                m_pSource->m_vPos.y += distance;

                //跳跃中断
                m_isJumping = false;
                m_isFaling = true;
            }
            else
            {
                m_pSource->m_vPos.y += JumpUpSpeed * upTime;
            }
        }
    }


    //移动
    if(keyboard->isKeyPress('A'))
    {
        faceToDir(-getCamera()->right());
    }
    else if(keyboard->isKeyPress('D'))
    {
        faceToDir(getCamera()->right());
    }
    else if(keyboard->isKeyPress('W'))
    {
        faceToDir(getCamera()->look());
    }
    else if(keyboard->isKeyPress('S'))
    {
        faceToDir(-getCamera()->look());
    }
    else
    {
        moved = false;
    }

    if (moved)
    {
        Physics::Vector3 startPos = m_pSource->m_vPos;
        m_pSource->moveLook(fElapse);
        Physics::Vector3 endPos = m_pSource->m_vPos;

        if (!startPos.tooClose(endPos))
        {

            Physics::CollisionPrevent cp(startPos, endPos, false, CConfig);
            g_debugCP = cp;

            if(Physics::preventCollision(cp))
            {
                m_collid = true;

                float distance = max(0.0f, cp.m_distance - CConfig.modelDepth);
                startPos = cp.m_start + cp.m_look * distance;
#if 1
                endPos = cp.m_end;

                //如果发生了碰撞，让模型向两边滑行，而不是卡在原地不动。

                Physics::Vector3 normal = cp.m_hitTriangle.normal();
                normal.y = 0;
                normal.normalize();

                Physics::Vector3 delta = endPos - startPos;
                delta -= normal * delta.dot(normal) ;
                Physics::Vector3 realDir = delta;
                realDir.normalize();

                endPos = startPos + delta*0.2f;

                //继续检测一下，放防止滑行时钻入障碍物。
                Physics::CollisionConfig config(ModelWidth, ModelHeight, ModelDepth, 0);
                Physics::CollisionPrevent rcp(startPos, endPos, false, config);
                if (Physics::preventCollision(rcp))
                {
                    distance = max(0.0f, rcp.m_distance - config.modelDepth);
                    endPos = startPos + realDir * distance;
                }
#else
                endPos = startPos;
#endif
                m_pSource->m_vPos = endPos;
            }
        }

        if (m_state != PS_MOVE || m_autoMove)
        {
            breakAutoMove();
            setState(PS_MOVE);
        }
    }
    else if (!m_autoMove && !m_moveByPath && (getState()==PS_MOVE || getState()==PS_ROTATE) )
    {
        setState(PS_FREE);
    }
}

void iPhysics::updateHeight(float elapse)
{
    if (!m_lockHInMap || NULL==m_pSource)
    {
        return;
    }

    TerrainMap* pMap = TerrainMap::instance();
    if (!pMap->isUserfull())
    {
        return;
    }

    Physics::Vector3 start = m_pSource->getPos();

    if (start.x < pMap->xMin())
    {
        start.x = pMap->xMin();
    }
    else if (start.x > pMap->xMax())
    {
        start.x = pMap->xMax();
    }

    if (start.z < pMap->zMin())
    {
        start.z = pMap->zMin();
    }
    else if (start.z > pMap->zMax())
    {
        start.z = pMap->zMax();
    }

    float mh = pMap->getHeight(start.x, start.z);

    float h = mh + m_lockHeight;

    if (!isPlayers())
    {
        m_pSource->m_vPos.y = h;
        return;
    }

    Physics::Vector3 end = start;

    //贴地
    float biasHeight = elapse * JumpDownSpeed;
    if (m_collid)//如果发生过碰撞，y位置必会上移，贴地处理需要减去此值。
    {
        biasHeight += ClimbHeight;
    }
    end.y -= biasHeight;

    if (end.y < h)
    {
        end.y = h;
    }


    if (end.y < start.y)
    {
        //抬高起点，防止钻入模型。
        start.y += heighError;

        Physics::CollisionConfig config(ModelWidth, ModelWidth, 0, 0);
        Physics::CollisionPrevent cp(start, end, true, config);
        if (Physics::preventCollision(cp))
        {
            end = cp.m_start + cp.m_look * cp.m_distance;

            m_isFaling = false;
        }
        
        g_debugCP2 = cp;
    }
    else
    {
         m_isFaling = false;
    }

    m_pSource->m_vPos = end;
}

void iPhysics::updateByPath()
{
    if(!m_moveByPath) return;

    if(m_wayPath.empty())
    {
        m_moveByPath = false;
        return;
    }

    Physics::Vector3 pos = m_wayPath.back();
    m_wayPath.pop_back();

    moveTo(pos);
}

bool iPhysics::searchPathEx(Physics::WpPtrArray & way, const Physics::Vector3 & dest)
{
    m_moveByPath = false;
    if(!m_pSource) return false;

    if(!Physics::WayChunkMgr::instance()->findWay(way, m_pSource->getPos(), dest))
        return false;

    Physics::wayPointToPosition(m_wayPath, way);
    m_moveByPath = true;
    
    return true;
}

bool iPhysics::searchPath(const Physics::Vector3 & dest)
{
    Physics::WpPtrArray way;
    return searchPathEx(way, dest);
}

void iPhysics::faceToDir(const Physics::Vector3 & dir)
{
    if (!m_pSource) return;
    
    m_pSource->m_vLook = dir;
    m_pSource->m_vLook.y = 0.0f;
    m_pSource->m_vLook.normalize();

    m_pSource->m_vUp.set(0, 1, 0);
    m_pSource->m_vUp.cross(m_pSource->m_vRight, m_pSource->m_vLook);
    m_pSource->m_vRight.normalize();
}

///朝向某点
void iPhysics::faceTo(const Physics::Vector3 & dest)
{
    faceToDir(dest - m_pSource->getPos());
}

void iPhysics::moveTo(const Physics::Vector3 & dest)
{
    if (m_pSource->m_vPos == dest)
    {
        m_autoMove = false;
        return;
    }

    m_prevPos = m_pSource->m_vPos;
    m_nextPos = dest;
    faceTo(dest);

    m_autoMove = true;
    setState(PS_MOVE);
}

///移动到entity
void iPhysics::moveToEntity(I3DObject* target)
{
    m_pMoveToTarget = target;
    m_bMoveToEntity = true;
}

void iPhysics::updateAutoMove(float elapse)
{
    if (!m_autoMove) return;

    m_pSource->moveLook(elapse);
    if (m_bMoveToEntity)
    {
        //如果进入entity的范围，则认为就到达了
        float inRange = getMaxDistToNpc() - 1.0f;
        float distSq = m_pSource->m_vPos.distToSq(m_pMoveToTarget->m_vPos);

        if(distSq <= inRange*inRange)
        {
            m_bMoveToEntity = false;
            m_pMoveToTarget = NULL;
            m_autoMove = false;
            setState(PS_FREE);
            onMoveToEntityFinish(true);
        }
    }
    else
    {
        float walkDist = m_pSource->m_vPos.distToSq(m_prevPos);//已走过距离
        float totalDist = m_nextPos.distToSq(m_prevPos);//总距离

        if (walkDist >= totalDist)
        {
            m_autoMove = false;
            if(!m_moveByPath || m_wayPath.empty())
            {
                setState(PS_FREE);
                onMoveToFinished(true);
            }
        }
    }
}


void iPhysics::breakAutoMove()
{
    m_moveByPath = false;

    if (!m_autoMove && !m_bMoveToEntity)
    {
        return ;
    }
    m_autoMove = false;
    setState(PS_FREE);
    if (m_bMoveToEntity)
    {
        m_bMoveToEntity = false;
        m_pMoveToTarget = NULL;
        onMoveToEntityFinish(false);
    }
    else
    {
        onMoveToFinished(false);
    }
}

void iPhysics::setState(DWORD state)
{ 
    if (m_state == state) return ;
    
    DWORD oldState = m_state;
    m_state = state; 
    onStateChange(oldState);
}

#include "Entity.h"

///状态发生变化
void iPhysics::onStateChange(DWORD)
{
    if (!m_pSource) return;

    ModelPtr model = ((iEntity*)m_pSource)->getModel();
    if (!model) return;

    if (m_state == PS_FREE)
    {
        model->playAction(L"stand", true);
    }
    else if (m_state == PS_MOVE || m_state == PS_JUMP)
    {
        model->playAction(L"walk1", true);
    }

}

///移动完毕
void iPhysics::onMoveToFinished(bool)
{
}

void iPhysics::onMoveToEntityFinish(bool)
{
}

void iPhysics::onSearchToFinished(bool)
{
}