﻿
#include "stdafx.h"
#include "Camera.h"
#include "App.h"
#include "TerrainMap.h"
#include "Collision.h"

#include "../Render/EffectConstant.h"

namespace Lazy
{

    namespace
    {
        const float CollisionError = 0.5f; //碰撞误差
        const float HeightError = 0.5f; //贴地误差
    }

    Camera* g_pCamera_ = NULL;
    LZDLL_API Camera* getCamera(void)
    {
        return g_pCamera_;
    }

    void effectApplyCameraPosition(EffectConstant *pConst)
    {
        pConst->bindValue(getCamera()->getPosition());
    }

    //////////////////////////////////////////////////////////////////////////

    Camera::Camera(CameraType type /*= THIRD*/)
        : m_pSource(nullptr)
        , m_cameraType(type)
        , m_fDistToPlayer(5.0f)
        , m_realDistToPlayer(5.0f)
        , m_distMin(1.0f)
        , m_distMax(12.0f)
        , m_speed(10.0f)
        , m_lastElapse(0.0f)
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


    Camera::~Camera(void)
    {
    }

    void Camera::setCamareType(Camera::CameraType cameraType)
    {
        m_cameraType = cameraType;
    }

    bool Camera::handleEvent(const SEvent & event)
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
                    float dir = z < 0.0f ? 1.0f : -1.0f;
                    moveLook(dir * m_lastElapse * m_speed);
                }
                else
                {
                    float dt = (m_distMax - m_distMin) * z * 0.2f;
                    m_fDistToPlayer -= dt;

                    correctDist();
                }
                return true;
            }
        }

        return false;
    }

    void Camera::drag(CPoint pt)
    {
        CPoint dp = pt - m_ptDown;
        if (abs(dp.x) < 3 && abs(dp.y) < 3)  return;

        rotationY( dp.x * m_curSpeedX );
        rotationRight( dp.y * m_curSpeedY);

#if 1
        showCursor(false);
        getApp()->setCursorPos(m_ptDown);
#else
        m_ptDown = pt;
#endif

        m_draged = true;
    }


    void Camera::update(float fElapse)
    {
        m_lastElapse = fElapse;

        if (m_realDistToPlayer > 2 * m_distMax)
        {
            m_realDistToPlayer = 2 * m_distMax;
        }

        if (NULL == m_pSource || getCameraType() == FREE)
        {
            RefPtr<Keyboard> keyboard = getApp()->getKeyboard();
            //左右旋转
            if(keyboard->isKeyPress('A'))
            {
                //rotYaw(-3.14f*fElapse);
                moveRight(-fElapse * m_speed);
            }
            else if(keyboard->isKeyPress('D'))
            {
                //rotYaw(3.14f*fElapse);
                moveRight(fElapse * m_speed);
            }
            //前进后退
            if(keyboard->isKeyPress('W'))
            {
                moveLook(fElapse * m_speed);
            }
            else if(keyboard->isKeyPress('S'))
            {
                moveLook(-fElapse * m_speed);
            }
            return ;
        }

        //摄像机跟随
        Vector3 vecPos = m_pSource->getPosition();
        vecPos.y += m_height;
        if (getCameraType() == FIRST)
        {
            //m_pSource->setLook(m_look);
            //m_pSource->setUp(m_up);
            //m_pSource->setRight(m_right);
            setPosition(vecPos);
        }
        else if (getCameraType() == THIRD)
        {
            //镜头距离变化缓冲模式
            float delta = m_fDistToPlayer - m_realDistToPlayer;

            float factor = 1.0f / 0.3f; // 衰减程度/s
            float decay = min(1.0f, factor * fElapse);
            delta *= decay;

            if (fabs(delta) < 0.001f)
                m_realDistToPlayer = m_fDistToPlayer;
            else
                m_realDistToPlayer += delta;

            Vector3 dstPos = vecPos - getLook() * m_realDistToPlayer;
#if 0
            // y方向也做平滑衰减
            decay = min(1.0f, 10.0f * fElapse);
            dstPos.y = m_position.y + (dstPos.y - m_position.y) * decay;
#endif

            //反向射线拾取，避免有物体格挡在相机与玩家之间。
            Vector3 start = vecPos;
            Vector3 end = dstPos;
            Vector3 dir = end - start;

            float realDistance = dir.length();
            float distance = realDistance + CollisionError;
            dir.normalize();

            RayCollision rc(start, dir, distance);
            if (pickRay(rc))
            {
                distance = min(realDistance, rc.m_hitDistance);
                dstPos = start + dir * distance;
                m_realDistToPlayer = distance;
            }

            setPosition(dstPos);
        }


#if 1
        //贴地处理
        TerrainMap* pMap = TerrainMap::instance();
        if (pMap->isUserfull())
        {
            Vector3 pos = getPosition();
            float h = pMap->getHeight(pos.x, pos.z) + HeightError;
            if (pos.y < h)
            {
                pos.y = h;
                setPosition(pos);
            }
        }

#endif
    }


    void Camera::setCurRoSpeed(float speed)
    {
        m_curSpeedX = speed;
        m_curSpeedY = speed;

    }

    void Camera::showCursor(bool show)
    {
        if (show == m_bCurShow)
        {
            return;
        }
        m_bCurShow = show;
        ShowCursor(show);
    }


    Matrix Camera::getViewMatrix() const
    {
        Matrix view;
        genViewMatrix(view);
        return view;
    }

    void Camera::setDistRange(float mind, float maxd)
    {
        m_distMin = mind;
        m_distMax = maxd;
        correctDist();
    }

    /** 矫正距离玩家的距离。*/
    void Camera::correctDist(void)
    {
        if (m_fDistToPlayer < m_distMin)
        {
            m_fDistToPlayer = m_distMin;
        }
        else if (m_fDistToPlayer > m_distMax)
        {
            m_fDistToPlayer = m_distMax;
        }
    }


} // end namespace Lazy
