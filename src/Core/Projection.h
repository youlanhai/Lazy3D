#pragma once

namespace Lazy
{
    class Projection
    {
    public:
        Projection(void);
        virtual ~Projection(void);

        const Matrix & getProjection() const;

        void  setNearFar(float fNear, float fFar);
        float getNear() const{ return m_znear; }
        float getFar() const { return m_zfar; }

    public:
        //设置3D摄像机	(透视投影)
        void setPerspective(float fov, float aspect, float znear, float zfar);

        void  setFov(float fov);
        float getFov() const { return m_fov; }

        void  setAspect(float aspect);
        float getAspect() const { return m_aspect; }

    public:
        //设置2D摄像机(正交投影)
        void setOrtho(float zoomX, float zoomY, float znear, float zfar);

        void  setZoomX(float zoom);
        float getZoomX() const { return m_zoomX; }

        void  setZoomY(float zoom);
        float getZoomY() const { return m_zoomY; }

    protected:
        mutable Matrix  m_projection;
        int             m_matrixDirty;
        int             m_isPerspective;
        float		    m_znear;
        float		    m_zfar;

        float           m_fov;
        float           m_aspect;

        float           m_zoomX;
        float           m_zoomY;
    };

} // end namespace Lazy
