
#include "stdafx.h"
#include "Projection.h"
#include "../Render/RenderDevice.h"

namespace Lazy
{

    Projection::Projection(void)
        : m_projection(matIdentity)
        , m_matrixDirty(true)
        , m_isPerspective(true)
        , m_znear(1.0f)
        , m_zfar(100.0f)
        , m_fov(D3DX_PI / 4)
        , m_aspect(1.0f)
        , m_zoomX(640)
        , m_zoomY(480)
    {
    }

    Projection::~Projection(void)
    {}

    const Matrix & Projection::getProjection() const
    {
        if (m_matrixDirty)
        {
            if (m_isPerspective)
                m_projection.makePerspective(m_fov, m_aspect, m_znear, m_zfar);
            else
                m_projection.makeOrtho(m_zoomX, m_zoomY, m_znear, m_zfar);
        }
        return m_projection;
    }

    void  Projection::setNearFar(float fNear, float fFar)
    {
        m_znear = fNear;
        m_zfar = fFar;
        m_matrixDirty = 1;
    }
    
    //设置3D摄像机	(透视投影)
    void Projection::setPerspective(float fov, float aspect, float znear, float zfar)
    {
        m_isPerspective = 1;
        setFov(fov);
        setAspect(aspect);
        setNearFar(znear, zfar);
    }

    void  Projection::setFov(float fov)
    {
        m_fov = fov;
        m_matrixDirty = 1;
    }

    void  Projection::setAspect(float aspect)
    {
        m_aspect = aspect;
        m_matrixDirty = 1;
    }

    //设置2D摄像机(正交投影)
    void Projection::setOrtho(float zoomX, float zoomY, float znear, float zfar)
    {
        m_isPerspective = 0;
        setZoomX(zoomX);
        setZoomY(zoomY);
        setNearFar(znear, zfar);
    }

    void  Projection::setZoomX(float zoom)
    {
        m_zoomX = zoom;
        m_matrixDirty = 1;
    }

    void  Projection::setZoomY(float zoom)
    {
        m_zoomY = zoom;
        m_matrixDirty = 1;
    }

} // end namespace Lazy
