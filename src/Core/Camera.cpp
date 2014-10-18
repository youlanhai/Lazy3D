//Camera.cpp

#include "stdafx.h"
#include "SceneNode.h"
#include "Camera.h"

namespace Lazy
{

//////////////////////////////////////////////////////////////////////////
    Camera::Camera(CameraType type/* = THIRD*/)
        : m_pSource(nullptr)
    {
        initCamera(type);
    }

    Camera::~Camera(void)
    {

    }

    void Camera::initCamera(CameraType type/* = THIRD*/)
    {
        setCamareType(type);
        m_position.set( 0.0f, 20.0f, -200.0f );
        m_fDistToPlayer = 10.0f;
        setDistRange(2.0f, 30.0f);
        setNearFar(1.0f, 10000.0f);
    }


    void Camera::setCamareType(CameraType cameraType)
    {
        m_cameraType = cameraType;
        if (m_pSource)
        {
            if (m_cameraType == FIRST )
            {
                //m_pSource->show(false);
            }
            else
            {
                //m_pSource->show(true);
            }
        }
    }

    void Camera::setDistRange(float mind, float maxd)
    {
        m_distMin = mind;
        m_distMax = maxd;
        correctDist();
    }

    void Camera::setDistance(float fDistance)
    {
        m_fDistToPlayer = fDistance;
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
