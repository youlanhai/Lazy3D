#pragma once

#include "DXConfig.h"
#include "Device.h"
#include "../utility/Singleton.h"

namespace Lazy
{

    class ShadowMap : public IDevice, public Singleton<ShadowMap>
    {
    public:
        ShadowMap();
        ~ShadowMap();

        bool begin();
        void end();

        bool isUsing() const { return m_isUsing != FALSE; }
        dx::Texture * getTexture() { return m_pRenderTexture; }
        dx::Surface * getDepthSurface() { return m_pDepthSurface; }

        void setLightPosition(const Vector3 & v){ m_lightPosition = v; }
        const Vector3 & getLightPosition() const { return m_lightPosition; }

        void setLightDirection(const Vector3 & v){ m_lightDirection = v; }
        const Vector3 & getLightDirection() const { return m_lightDirection; }

        void genLightMatrix(Matrix & matrix);

    private:
        bool create();

        virtual void onCreateDevice();
        virtual void onCloseDevice();
        virtual void onLostDevice();
        virtual void onResetDevice();

    private:
        dx::Surface *   m_pDepthSurface;
        dx::Texture *   m_pRenderTexture;
        BOOL            m_isUsing;

        dx::Surface *   m_pOldDepthSurface;
        dx::Surface *   m_pOldRenderSurface;

        Vector3         m_lightPosition;
        Vector3         m_lightDirection;
    };

} // end namespace Lazy
