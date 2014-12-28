#include "stdafx.h"
#include "ShadowMap.h"

#include "RenderDevice.h"

namespace Lazy
{
    IMPLEMENT_SINGLETON(ShadowMap);

    ShadowMap::ShadowMap()
        : m_pDepthSurface(nullptr)
        , m_pRenderTexture(nullptr)
        , m_isUsing(FALSE)
    {
    }

    ShadowMap::~ShadowMap()
    {
        onCloseDevice();
    }

    bool ShadowMap::create()
    {
        assert(!m_isUsing);
        onCloseDevice();

        const D3DPRESENT_PARAMETERS * pp = rcDevice()->getPresentParameter();

        HRESULT hr;
        dx::Device * pDevice = rcDevice()->getDevice();

        hr = pDevice->CreateTexture(pp->BackBufferWidth, pp->BackBufferHeight, 1, D3DUSAGE_RENDERTARGET,
            D3DFMT_A8, D3DPOOL_DEFAULT, &m_pRenderTexture, NULL);
        if (FAILED(hr))
        {
            return false;
        }

        hr = pDevice->CreateDepthStencilSurface(pp->BackBufferWidth, pp->BackBufferHeight, 
            pp->AutoDepthStencilFormat, D3DMULTISAMPLE_NONE, 0, FALSE, &m_pDepthSurface, NULL);
        if (FAILED(hr))
        {
            return false;
        }

        return true;
    }

    void ShadowMap::onCreateDevice()
    {
        create();
    }

    void ShadowMap::onCloseDevice()
    {
        assert(!m_isUsing);

        SAFE_RELEASE(m_pDepthSurface);
        SAFE_RELEASE(m_pRenderTexture);
    }

    void ShadowMap::onLostDevice()
    {
        assert(!m_isUsing);

        SAFE_RELEASE(m_pDepthSurface);
        SAFE_RELEASE(m_pRenderTexture);
    }

    void ShadowMap::onResetDevice()
    {
        create();
    }

    bool ShadowMap::begin()
    {
        assert(!m_isUsing);
        assert(m_pRenderTexture && m_pDepthSurface);

        dx::Device * pDevice = rcDevice()->getDevice();

        if (FAILED(pDevice->GetRenderTarget(0, &m_pOldRenderSurface)))
            return false;

        if (FAILED(pDevice->GetDepthStencilSurface(&m_pOldDepthSurface)))
        {
            m_pOldRenderSurface->Release();
            return false;
        }

        dx::Surface * pRenderSurface = NULL;
        do
        {
            if (FAILED(m_pRenderTexture->GetSurfaceLevel(0, &pRenderSurface)))
                break;

            if (FAILED(pDevice->SetRenderTarget(0, pRenderSurface)))
                break;

            if (FAILED(pDevice->SetDepthStencilSurface(m_pDepthSurface)))
                break;

            pRenderSurface->Release();
            m_isUsing = TRUE;
            return true;
        } while (0);

        pDevice->SetRenderTarget(0, m_pOldRenderSurface);
        pDevice->SetDepthStencilSurface(m_pOldDepthSurface);

        SafeRelease(m_pOldRenderSurface);
        SafeRelease(m_pOldRenderSurface);
        SafeRelease(pRenderSurface);
        return false;
    }

    void ShadowMap::end()
    {
        assert(m_isUsing);
        m_isUsing = FALSE;

        dx::Device * pDevice = rcDevice()->getDevice();
        pDevice->SetRenderTarget(0, m_pOldRenderSurface);
        pDevice->SetDepthStencilSurface(m_pOldDepthSurface);

        SafeRelease(m_pOldRenderSurface);
        SafeRelease(m_pOldRenderSurface);
    }

} // end namespace Lazy