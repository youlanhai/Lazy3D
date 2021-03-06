﻿
#include "stdafx.h"

#include "Camera.h"
#include "TerrainMap.h"
#include "App.h"

#include "Physics.h"
#include "Collision.h"
#include "Entity.h"

namespace Lazy
{

    float g_maxDistToNpc = 20.0f;
    bool g_drawCollision = false;

    const float ModelWidth  = 0.40f;
    const float ModelHeight = 1.80f;
    const float ModelDepth  = 0.30f;
    const float ClimbHeight = 0.30f;

    const float JumpUpSpeed = 10.0f;
    const float JumpDownSpeed = 5.0f;
    const float JumpDuration = 0.4f;

    const float heighError = 0.5f; //高度误差值

    const CollisionConfig CConfig(ModelWidth, ModelHeight, ModelDepth, ClimbHeight);

    CollisionPrevent g_debugCP(Vector3(0, 0, 0),
                               Vector3(10, 10, 10), false, CConfig);
    CollisionPrevent g_debugCP2 = g_debugCP;


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


    IPhysics::IPhysics()
        : m_pSource(NULL)
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

    IPhysics::~IPhysics(void)
    {
    }

    void IPhysics::render(IDirect3DDevice9 * pDevice)
    {
        if(ifPlayers() && g_drawCollision)
        {
            RSHolder rsHolder(pDevice, D3DRS_LIGHTING, FALSE);

            g_debugCP.drawDebug(pDevice);
            g_debugCP2.drawDebug(pDevice);
        }
    }

    void IPhysics::update(float fElapse)
    {
        if (!isEnabled() || !m_pSource)
        {
            return ;
        }

        if(ifPlayers())
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
            moveTo(m_pMoveToTarget->getPosition());
        }

        updateAutoMove(fElapse);

        if(!m_autoMove && m_moveByPath)
        {
            updateByPath();
        }

        updateHeight(fElapse);
    }

    bool IPhysics::ifPlayers()
    {
        return m_pSource ? m_pSource->ifPlayer() : false;
    }

    void IPhysics::updatePlayer(float fElapse)
    {
        if (getCamera()->getCameraType() == Camera::FREE)
        {
            return;
        }
        else if (getCamera()->getCameraType() == Camera::FIRST)
        {
            m_pSource->setVisible(false);
        }

        RefPtr<Keyboard> keyboard = getApp()->getKeyboard();

        bool moved = true;
        m_collid = false;

        if (keyboard->isKeyPress(VK_UP))
        {
            m_pSource->moveUp( JumpUpSpeed * fElapse );
        }
        else if (keyboard->isKeyPress(VK_DOWN))
        {
            m_pSource->moveUp( -JumpUpSpeed * fElapse);
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

                Vector3 start = m_pSource->getPosition();
                start.y += ModelHeight;
                Vector3 end = start;
                end.y += JumpUpSpeed * upTime;

                //降低起点，防止钻入模型。
                start.y -= heighError;

                float distance;
                CollisionConfig config(ModelWidth, ModelWidth, 0, 0);
                CollisionPrevent cp(start, end, true, config);
                if (preventCollision(cp))
                {
                    distance = max(0.0f, cp.m_distance - heighError);

                    //跳跃中断
                    m_isJumping = false;
                    m_isFaling = true;
                }
                else
                {
                    distance = JumpUpSpeed * upTime;
                }

                Vector3 pos = m_pSource->getPosition();
                pos.y += distance;
                m_pSource->setPosition(pos);
            }
        }


        //移动
        if(keyboard->isKeyPress('A'))
        {
            faceToDir(-getCamera()->getRight());
        }
        else if(keyboard->isKeyPress('D'))
        {
            faceToDir(getCamera()->getRight());
        }
        else if(keyboard->isKeyPress('W'))
        {
            faceToDir(getCamera()->getLook());
        }
        else if(keyboard->isKeyPress('S'))
        {
            faceToDir(-getCamera()->getLook());
        }
        else
        {
            moved = false;
        }

        if (moved)
        {
            Vector3 startPos = m_pSource->getPosition();
            m_pSource->moveLook(fElapse * m_pSource->getSpeed().x);
            Vector3 endPos = m_pSource->getPosition();

            if (!startPos.tooClose(endPos))
            {

                CollisionPrevent cp(startPos, endPos, false, CConfig);
                g_debugCP = cp;

                if(preventCollision(cp))
                {
                    m_collid = true;

                    float distance = max(0.0f, cp.m_distance - CConfig.modelDepth);
                    startPos = cp.m_start + cp.m_look * distance;
#if 1
                    endPos = cp.m_end;

                    //如果发生了碰撞，让模型向两边滑行，而不是卡在原地不动。

                    Vector3 normal = cp.m_hitTriangle.normal();
                    normal.y = 0;
                    normal.normalize();

                    Vector3 delta = endPos - startPos;
                    delta -= normal * delta.dot(normal) ;
                    Vector3 realDir = delta;
                    realDir.normalize();

                    endPos = startPos + delta * 0.2f;

                    //继续检测一下，放防止滑行时钻入障碍物。
                    CollisionConfig config(ModelWidth, ModelHeight, ModelDepth, 0);
                    CollisionPrevent rcp(startPos, endPos, false, config);
                    if (preventCollision(rcp))
                    {
                        distance = max(0.0f, rcp.m_distance - config.modelDepth);
                        endPos = startPos + realDir * distance;
                    }
#else
                    endPos = startPos;
#endif
                    m_pSource->setPosition(endPos);
                }
            }

            if (m_state != PS_MOVE || m_autoMove)
            {
                breakAutoMove();
                setState(PS_MOVE);
            }
        }
        else if (!m_autoMove && !m_moveByPath && (getState() == PS_MOVE || getState() == PS_ROTATE) )
        {
            setState(PS_FREE);
        }
    }

    void IPhysics::updateHeight(float elapse)
    {
        if (!m_lockHInMap || NULL == m_pSource)
            return;

        TerrainMap* pMap = TerrainMap::instance();
        if (!pMap->isUserfull())
            return;

        Vector3 start = m_pSource->getPosition();
        const FRect & rect = pMap->getRect();

        if (start.x < rect.left)
            start.x = rect.left;
        else if (start.x >= rect.right)
            start.x = rect.right - 0.001f;

        if (start.z < rect.top)
            start.z = rect.top;
        else if (start.z > rect.bottom)
            start.z = rect.bottom - 0.001f;

        if (!ifPlayers())
        {
            float mh = pMap->getHeight(start.x, start.z);
            m_pSource->move(Vector3(0, mh + m_lockHeight, 0));
            return;
        }

        const float radius = 0.2f;
        float h1 = pMap->getHeight(start.x - radius, start.z - radius);
        float h2 = pMap->getHeight(start.x - radius, start.z + radius);
        float h3 = pMap->getHeight(start.x + radius, start.z + radius);
        float h4 = pMap->getHeight(start.x + radius, start.z - radius);

        float h = (h1 + h2 + h3 + h4) * 0.25f;

        Vector3 end = start;

        //贴地
        float biasHeight = elapse * JumpDownSpeed;
        if (m_collid)//如果发生过碰撞，y位置必会上移，贴地处理需要减去此值。
            biasHeight += ClimbHeight;
        
        end.y = max2( end.y - biasHeight, h);

        if (end.y < start.y)
        {
            //抬高起点，防止钻入模型。
            start.y += heighError;

            CollisionConfig config(ModelWidth, ModelWidth, 0, 0);
            CollisionPrevent cp(start, end, true, config);
            if (preventCollision(cp))
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

        m_pSource->setPosition(end);
    }

    void IPhysics::updateByPath()
    {
        if(!m_moveByPath) return;

        if(m_wayPath.empty())
        {
            m_moveByPath = false;
            return;
        }

        Vector3 pos = m_wayPath.back();
        m_wayPath.pop_back();

        moveTo(pos);
    }

    bool IPhysics::searchPathEx(WpPtrArray & way, const Vector3 & dest)
    {
        m_moveByPath = false;
        if(!m_pSource) return false;

        if(!WayChunkMgr::instance()->findWay(way, m_pSource->getPosition(), dest))
            return false;

        wayPointToPosition(m_wayPath, way);
        m_moveByPath = true;

        return true;
    }

    bool IPhysics::searchPath(const Vector3 & dest)
    {
        WpPtrArray way;
        return searchPathEx(way, dest);
    }

    void IPhysics::faceToDir(const Vector3 & dir)
    {
        if (m_pSource)
            m_pSource->faceToDir(dir);
    }

    void IPhysics::faceTo(const Vector3 & dest)
    {
        faceToDir(dest - m_pSource->getPosition());
    }

    void IPhysics::moveTo(const Vector3 & dest)
    {
        if (m_pSource->getPosition().tooClose(dest))
        {
            m_autoMove = false;
            return;
        }

        m_prevPos = m_pSource->getPosition();
        m_nextPos = dest;
        faceTo(dest);

        m_autoMove = true;
        setState(PS_MOVE);
    }

    void IPhysics::moveToEntity(IEntity* target)
    {
        m_pMoveToTarget = target;
        m_bMoveToEntity = true;
    }

    void IPhysics::updateAutoMove(float elapse)
    {
        if (!m_autoMove) return;

        m_pSource->moveLook(elapse);
        if (m_bMoveToEntity)
        {
            //如果进入entity的范围，则认为就到达了
            float inRange = getMaxDistToNpc() - 1.0f;
            if(m_pSource->getPosition().tooClose(
                m_pMoveToTarget->getPosition(), inRange * inRange))
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
            float walkDist = m_pSource->getPosition().distToSq(m_prevPos);//已走过距离
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


    void IPhysics::breakAutoMove()
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

    void IPhysics::setState(DWORD state)
    {
        if (m_state == state) return ;

        DWORD oldState = m_state;
        m_state = state;
        onStateChange(oldState);
    }

    void IPhysics::onStateChange(DWORD)
    {
        if (!m_pSource) return;

        ModelPtr model = ((IEntity*)m_pSource)->getModel();
        if (!model) return;

        if (m_state == PS_FREE)
        {
            model->playAction("stand", true);
        }
        else if (m_state == PS_MOVE || m_state == PS_JUMP)
        {
            model->playAction("run", true);
        }

    }

    void IPhysics::onMoveToFinished(bool)
    {
    }

    void IPhysics::onMoveToEntityFinish(bool)
    {
    }

    void IPhysics::onSearchToFinished(bool)
    {
    }

} // end namespace Lazy
