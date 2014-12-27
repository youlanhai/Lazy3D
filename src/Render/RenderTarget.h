#pragma once

#include "DXConfig.h"
#include "Device.h"
#include "../utility/SmartPtr.h"

namespace Lazy
{

    class RenderTarget : public Object, public IDevice
    {
    public:
        RenderTarget();
        ~RenderTarget();

        bool valid() const { return m_pRenderSurface != nullptr; }

    private:

        virtual void onCloseDevice();
        virtual void onLostDevice();
        virtual void onResetDevice();

    private:
        dx::Surface * m_pRenderSurface;
        dx::Surface * m_pDepthSurface;
        dx::Texture * m_pTexture;
    };

    typedef RefPtr<RenderTarget> RenderTargetPtr;

}
