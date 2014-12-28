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

        dx::Texture * getTexture() { return m_pRenderTexture; }
        dx::Surface * getDepthSurface() { return m_pDepthSurface; }

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
    };

} // end namespace Lazy
