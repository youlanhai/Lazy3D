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

    private:
        dx::Surface * m_pRenderSurface;
        dx::Surface * m_pDepthSurface;
        dx::Texture * m_pTexture;
    };

    typedef RefPtr<RenderTarget> RenderTargetPtr;

}
