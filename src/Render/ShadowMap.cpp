#include "stdafx.h"
#include "ShadowMap.h"

#include "RenderDevice.h"

#define ShadowMap_SIZE 512

namespace Lazy
{
    IMPLEMENT_SINGLETON(ShadowMap);

    ShadowMap::ShadowMap()
        : m_pDepthSurface(nullptr)
        , m_pRenderTexture(nullptr)
        , m_isUsing(FALSE)
        , m_lightPosition(200.f, 100.f, 0.f)
        , m_width(0)
        , m_height(0)
    {
        m_lightDirection = m_lightPosition;
        m_lightDirection.normalize();
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

        m_width = ShadowMap_SIZE;// pp->BackBufferWidth;
        m_height = ShadowMap_SIZE;// pp->BackBufferHeight;

        hr = pDevice->CreateTexture(m_width, m_height, 1, D3DUSAGE_RENDERTARGET,
            D3DFMT_R32F, D3DPOOL_DEFAULT, &m_pRenderTexture, NULL);
        if (FAILED(hr))
        {
            return false;
        }

        hr = pDevice->CreateDepthStencilSurface(m_width, m_height,
            pp->AutoDepthStencilFormat, D3DMULTISAMPLE_NONE, 0, TRUE, &m_pDepthSurface, NULL);
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
        onCloseDevice();
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
        SafeRelease(m_pOldDepthSurface);
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
        SafeRelease(m_pOldDepthSurface);
    }

    void ShadowMap::genLightMatrix(Matrix & matrix)
    {
        matrix.makeLookAt(m_lightPosition - m_lightDirection * 100.0f,
            m_lightPosition, Vector3(0, 1, 0));

        Matrix proj;
        proj.makePerspective(D3DX_PI / 4.0f, float(m_width) / m_height, 1.0f, 10000.0f);
        matrix.postMultiply(proj);
    }

} // end namespace Lazy
