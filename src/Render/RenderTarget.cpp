#include "stdafx.h"
#include "RenderTarget.h"

#include "RenderDevice.h"

namespace Lazy
{

    RenderTarget::RenderTarget()
    {
    }


    RenderTarget::~RenderTarget()
    {
    }

    void RenderTarget::onCloseDevice()
    {
        SAFE_RELEASE(m_pRenderSurface);
        SAFE_RELEASE(m_pDepthSurface);
        SAFE_RELEASE(m_pTexture);
    }

    void RenderTarget::onLostDevice()
    {
        
    }

    void RenderTarget::onResetDevice()
    {
#if 0
        rcDevice()->getDevice()->CreateRenderTarget(
            rcDevice()->getBackBufferWidth(),
            rcDevice()->getBackBufferHeight(),
            rcDevice()->getBackBufferFormat(),
            0,
            0,
            FALSE,
            &m_pRenderSurface,
            NULL,
            );
#endif
    }

}