
#include "stdafx.h"
#include "CursorCamera.h"
#include "App.h"
#include "TerrinMap.h"
#include "Collision.h"

namespace Lazy
{

    namespace
    {
        const float CollisionError = 0.5f; //碰撞误差
        const float HeightError = 0.5f; //贴地误差
    }

    CCursorCamera* g_pCamera_ = NULL;
    LZDLL_API CCursorCamera* getCamera(void)
    {
        return g_pCamera_;
    }
//////////////////////////////////////////////////////////////////////////

    CCursorCamera::CCursorCamera(CameraType type /*= THIRD*/)
        : Camera(type)
    {
        g_pCamera_ = this;

        m_bMouseDown = false;
        m_bCurShow = true;
        m_draged = false;
        m_height = 1.60f;
        m_realDistToPlayer = m_fDistToPlayer;

        float angleX = D3DX_PI / getApp()->getWidth() * 0.5f;
        setCurRoSpeed(angleX);
    }


    CCursorCamera::~CCursorCamera(void)
    {
    }

    bool CCursorCamera::handleEvent(const SEvent & event)
    {
        if (event.eventType == EET_MOUSE_EVENT)
        {
            CPoint pt(event.mouseEvent.x, event.mouseEvent.y);

            if (event.mouseEvent.event == EME_LMOUSE_DOWN ||
                    event.mouseEvent.event == EME_RMOUSE_DOWN)
            {
                m_bMouseDown = true;
                m_ptDown = pt;
                m_draged = false;
            }
            else if (event.mouseEvent.event == EME_LMOUSE_UP ||
                     event.mouseEvent.event == EME_RMOUSE_UP)
            {
                m_bMouseDown = false;
                if (!m_bCurShow)
                {
                    showCursor(true);
                    return true;
                }
            }
            else if (event.mouseEvent.event == EME_MOUSE_MOVE)
            {
                if (m_bMouseDown)
                {
                    drag(pt);
                    return true;
                }
            }
            else if (event.mouseEvent.event == EME_MOUSE_WHEEL)
            {
                float z = event.mouseEvent.wheel * 0.5f;

                if (NULL == m_pSource)
                {
                    if (z < 0.0f)
                    {
                        moveLook(false);
                    }
                    else if (z > 0.0f)
                    {
                        moveLook(true);
                    }
                }
                else
                {
                    float dt = (m_distMax - m_distMin) * z;
                    m_fDistToPlayer -= dt;

                    correctDist();
                }

                return true;
            }
        }

        return false;
    }

    void CCursorCamera::drag(CPoint pt)
    {
        CPoint dp = pt - m_ptDown;
        if (abs(dp.x) < 3 && abs(dp.y) < 3)  return;

        rotYaw( dp.x * m_curSpeedX );
        rotPitch( dp.y * m_curSpeedY);

#if 1
        showCursor(false);
        getApp()->setCursorPos(m_ptDown);
#else
        m_ptDown = pt;
#endif

        m_draged = true;
    }


    void CCursorCamera::update(float fElapse)
    {
        Camera::update(fElapse);

        if (m_realDistToPlayer > 2 * m_distMax)
        {
            m_realDistToPlayer = 2 * m_distMax;
        }

        if (NULL == m_pSource || getCameraType() == FREE)
        {
            RefPtr<CKeyboard> keyboard = getApp()->getKeyboard();
            //左右旋转
            if(keyboard->isKeyPress('A'))
            {
                //rotYaw(-3.14f*fElapse);
                moveRight(false);
            }
            else if(keyboard->isKeyPress('D'))
            {
                //rotYaw(3.14f*fElapse);
                moveRight(true);
            }
            //前进后退
            if(keyboard->isKeyPress('W'))
            {
                moveLook(true);
            }
            else if(keyboard->isKeyPress('S'))
            {
                moveLook(false);
            }
            return ;
        }

        //摄像机跟随
        D3DXVECTOR3 vecPos = m_pSource->getPos();
        vecPos.y += m_height;
        if (getCameraType() == Camera::FIRST)
        {
            m_pSource->setLook(m_look);
            m_pSource->setUp(m_up);
            m_pSource->setRight(m_right);
            m_position = vecPos;
        }
        else if (getCameraType() == Camera::THIRD)
        {
            //镜头距离变化缓冲模式
            float factor = 0.8f;
            float delta = m_realDistToPlayer - m_fDistToPlayer;
            float decay = delta * factor * fElapse;
            if (fabs(decay) > fabs(delta))
            {
                decay = delta;
            }

            if (fabs(decay) < 0.00001f)
            {
                decay = 0.0f;
            }

            m_realDistToPlayer -= decay;

            m_position = vecPos - m_look * m_realDistToPlayer;

            //反向射线拾取，避免有物体格挡在相机与玩家之间。
            Vector3 start = vecPos;
            Vector3 end = m_position;
            Vector3 dir = end - start;

            float realDistance = dir.length();
            float distance = realDistance + CollisionError;
            dir.normalize();

            RayCollision rc(start, dir, distance);
            if (pickRay(rc))
            {
                distance = min(realDistance, rc.m_hitDistance);
                end = start + dir * distance;
                end.toD3DXVec3(m_position);

                m_realDistToPlayer = distance;
            }

        }


#if 1
        //贴地处理
        TerrainMap* pMap = TerrainMap::instance();
        if (pMap->isUserfull())
        {
            float h = pMap->getHeight(m_position.x, m_position.z) + HeightError;
            if ( m_position.y < h)
            {
                m_position.y = h;
            }
        }

#endif

        updateViewMatrixRotation();

    }


    void CCursorCamera::setCurRoSpeed(float speed)
    {
        m_curSpeedX = speed;
        m_curSpeedY = speed;

    }

    void CCursorCamera::showCursor(bool show)
    {
        if (show == m_bCurShow)
        {
            return;
        }
        m_bCurShow = show;
        ShowCursor(show);
    }

} // end namespace Lazy
