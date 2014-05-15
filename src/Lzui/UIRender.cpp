#include "stdafx.h"
#include "UIRender.h"


namespace Lazy
{
    ///渲染参数缓存
    struct RSCache
    {
        DWORD zbuffer;
        DWORD lighting;
        DWORD alphaTest;
        DWORD alphaBlend;
        DWORD srcBlend;
        DWORD destBlend;
        DWORD shadeMode;
        DWORD magFilter;
        DWORD minFilter;
        DWORD op0Color;
        DWORD op0ColorArg1;
        DWORD op0ColorArg2;
        DWORD op0Alpha;
        DWORD op0AlphaArg1;
        DWORD op0AlphaArg2;
        DWORD op1Color;
        DWORD op1Alpha;
        D3DXMATRIX view;
        D3DXMATRIX projection;
        dx::Material material;
        dx::PixelShader *pixelShader;
        dx::VertexShader *vertexShader;
    };
    static RSCache s_rsCache;


    GUIRender::GUIRender(dx::Device * m_device, HWND hWnd)
        : m_device(m_device)
        , m_hWnd(hWnd)
    {
        if (m_device) m_device->AddRef();

        ZeroMemory(&s_rsCache, sizeof(s_rsCache));

        m_matWorldViewProj.makeIdentity();
    }

    GUIRender::~GUIRender()
    {
        if (m_device) m_device->Release();
    }

    bool GUIRender::isClipEnable()
    {
        DWORD clipEnabled = FALSE;
        m_device->GetRenderState(D3DRS_SCISSORTESTENABLE, &clipEnabled);
        return clipEnabled != FALSE;
    }

    void GUIRender::setClipEnalbe(bool enalbe)
    {
        m_device->SetRenderState(D3DRS_SCISSORTESTENABLE, enalbe);
    }

    void GUIRender::getClipRect(CRect & rect)
    {
        m_device->GetScissorRect(&rect);
    }

    void GUIRender::setClipRect(const CRect & rect)
    {
        m_device->SetScissorRect(&rect);
    }

    void GUIRender::setUIShader(const tstring & file)
    {
        m_uiEffect = EffectMgr::instance()->get(file);
    }

    void GUIRender::setFontShader(const tstring & file)
    {
        m_fontEffect = EffectMgr::instance()->get(file);
    }

    void GUIRender::drawRect(const CRect & rc, uint32 color, TexturePtr texture)
    {
        drawRect(rc, UVRect(0, 0, 1, 1), color, texture);
    }

    void GUIRender::drawRect(const CRect & rc, const UVRect & srcRc, uint32 color, TexturePtr texture)
    {
        UIVertex vertex [] = {
            { float(rc.left), float(rc.top), 1, color, srcRc.left, srcRc.top},
            { float(rc.right), float(rc.top), 1, color, srcRc.right, srcRc.top },
            { float(rc.left), float(rc.bottom), 1, color, srcRc.left, srcRc.bottom },
            { float(rc.right), float(rc.bottom), 1, color, srcRc.right, srcRc.bottom }
        };

        if (!m_uiEffect)
        {
            if (texture) m_device->SetTexture(0, texture->getTexture());
            else m_device->SetTexture(0, NULL);

            m_device->SetFVF(UIVertex::FVF);
            m_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (void*) vertex, UIVertex::SIZE);

            return;
        }

        m_uiEffect->setMatrix("g_worldViewProjection", m_matWorldViewProj);
        m_uiEffect->setTechnique("RenderWithTexture");

        if (texture)
            m_uiEffect->setTexture("g_texture", texture->getTexture());
        else
            m_uiEffect->setTexture("g_texture", NULL);

        UINT nPass;
        if (m_uiEffect->begin(nPass))
        {
            for (UINT i = 0; i < nPass; ++i)
            {
                if (m_uiEffect->beginPass(i))
                {
                    m_device->SetFVF(UIVertex::FVF);
                    m_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (void*) vertex, UIVertex::SIZE);

                    m_uiEffect->endPass();
                }
            }
            m_uiEffect->end();
        }
    }

    void GUIRender::drawRect(const CRect & rc, uint32 color)
    {
        UIVertex_UNTEX vertex [] = {
            {float(rc.left), float(rc.top), 1, color},
            { float(rc.right), float(rc.top), 1, color },
            { float(rc.left), float(rc.bottom), 1, color },
            { float(rc.right), float(rc.bottom), 1, color }
        };

        if (!m_uiEffect)
        {
            m_device->SetTexture(0, NULL);
            m_device->SetFVF(UIVertex::FVF);
            m_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (void*) vertex, UIVertex::SIZE);

            return;
        }

        m_uiEffect->setMatrix("g_worldViewProjection", m_matWorldViewProj);
        m_uiEffect->setTechnique("RenderNoTexture");
        m_uiEffect->setTexture("g_texture", NULL);

        UINT nPass;
        if (m_uiEffect->begin(nPass))
        {
            for (UINT i = 0; i < nPass; ++i)
            {
                if (m_uiEffect->beginPass(i))
                {
                    m_device->SetFVF(UIVertex_UNTEX::FVF);
                    m_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (void*) vertex, UIVertex_UNTEX::SIZE);

                    m_uiEffect->endPass();
                }
            }
            m_uiEffect->end();
        }
    }


    void GUIRender::drawRectFrame(const CRect & rc, int edgeSize, uint32 color)
    {
        edgeSize = max(1, edgeSize);

        CRect line(rc);
        line.right = line.left + edgeSize;
        drawRect(line, color);

        line = rc;
        line.left = line.right - edgeSize;
        drawRect(line, color);

        line = rc;
        line.bottom = line.top + edgeSize;
        drawRect(line, color);

        line = rc;
        line.top = line.bottom - edgeSize;
        drawRect(line, color);
    }

    bool GUIRender::textRenderBegin()
    {
        if (!m_fontEffect) return false;

        m_fontEffect->setMatrix("g_worldViewProjection", m_matWorldViewProj);

        if (!m_fontEffect->setTechnique("fontTech"))
            return false;

        UINT nPass;
        m_fontEffect->begin(nPass);
        m_fontEffect->beginPass(0);

        return true;
    }

    void GUIRender::textRenderEnd()
    {
        if (!m_fontEffect) return;

        m_fontEffect->endPass();
        m_fontEffect->end();
    }

    void GUIRender::drawWord(const CRect & dest, const UVRect & src, uint32 color, dx::Texture * texture)
    {
        if (!texture) return;

        UIVertex vertex[] = {
            { float(dest.left), float(dest.top), 1, color, src.left, src.top },
            { float(dest.right), float(dest.top), 1, color, src.right, src.top },
            { float(dest.left), float(dest.bottom), 1, color, src.left, src.bottom },
            { float(dest.right), float(dest.bottom), 1, color, src.right, src.bottom }
        };

        m_device->SetTexture(0, texture);
        m_device->SetFVF(UIVertex::FVF);
        m_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (void*) vertex, UIVertex::SIZE);
    }

    void GUIRender::renderBegin()
    {
        Math::Matrix4x4 matrix;
        matrix.makeIdentity();
        m_device->SetTransform(D3DTS_WORLD, &matrix);
        m_device->SetTransform(D3DTS_VIEW, &matrix);

        CRect rc;
        GetClientRect(m_hWnd, &rc);

        matrix.makeOrthoOffCenter((float) rc.left, (float) rc.right, (float) rc.bottom, (float) rc.top, 0, 1);
        m_device->SetTransform(D3DTS_PROJECTION, &matrix);

        m_matWorldViewProj = matrix;

        saveRenderState();
        setCommonRenderState();
    }

    void GUIRender::renderEnd()
    {
        restoreRenderState();
    }


    void GUIRender::saveRenderState()
    {
        m_device->GetRenderState(D3DRS_ZENABLE, &s_rsCache.zbuffer);
        m_device->GetRenderState(D3DRS_LIGHTING, &s_rsCache.lighting);
        m_device->GetRenderState(D3DRS_ALPHATESTENABLE, &s_rsCache.alphaTest);

        m_device->GetRenderState(D3DRS_SHADEMODE, &s_rsCache.shadeMode);

        m_device->GetRenderState(D3DRS_ALPHABLENDENABLE, &s_rsCache.alphaBlend);
        m_device->GetRenderState(D3DRS_SRCBLEND, &s_rsCache.srcBlend);
        m_device->GetRenderState(D3DRS_DESTBLEND, &s_rsCache.destBlend);

        m_device->GetSamplerState(0, D3DSAMP_MAGFILTER, &s_rsCache.magFilter);
        m_device->GetSamplerState(0, D3DSAMP_MINFILTER, &s_rsCache.minFilter);

        m_device->GetTextureStageState(0, D3DTSS_COLOROP, &s_rsCache.op0Color);
        m_device->GetTextureStageState(0, D3DTSS_COLORARG1, &s_rsCache.op0ColorArg1);
        m_device->GetTextureStageState(0, D3DTSS_COLORARG2, &s_rsCache.op0AlphaArg2);
        m_device->GetTextureStageState(0, D3DTSS_ALPHAOP, &s_rsCache.op0Alpha);
        m_device->GetTextureStageState(0, D3DTSS_ALPHAARG1, &s_rsCache.op0AlphaArg1);
        m_device->GetTextureStageState(0, D3DTSS_ALPHAARG2, &s_rsCache.op0AlphaArg2);
        m_device->GetTextureStageState(1, D3DTSS_COLOROP, &s_rsCache.op1Color);
        m_device->GetTextureStageState(1, D3DTSS_ALPHAOP, &s_rsCache.op1Alpha);
        m_device->GetMaterial(&s_rsCache.material);

        m_device->GetTransform(D3DTS_VIEW, &s_rsCache.view);
        m_device->GetTransform(D3DTS_PROJECTION, &s_rsCache.projection);
        m_device->GetPixelShader(&s_rsCache.pixelShader);
        m_device->GetVertexShader(&s_rsCache.vertexShader);
    }

    void GUIRender::restoreRenderState()
    {
        m_device->SetRenderState(D3DRS_ZENABLE, s_rsCache.zbuffer);
        m_device->SetRenderState(D3DRS_LIGHTING, s_rsCache.lighting);
        m_device->SetRenderState(D3DRS_ALPHATESTENABLE, s_rsCache.alphaTest);

        m_device->SetRenderState(D3DRS_SHADEMODE, s_rsCache.shadeMode);

        m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, s_rsCache.alphaBlend);
        m_device->SetRenderState(D3DRS_SRCBLEND, s_rsCache.srcBlend);
        m_device->SetRenderState(D3DRS_DESTBLEND, s_rsCache.destBlend);

        m_device->SetSamplerState(0, D3DSAMP_MAGFILTER, s_rsCache.magFilter);
        m_device->SetSamplerState(0, D3DSAMP_MINFILTER, s_rsCache.minFilter);

        m_device->SetTextureStageState(0, D3DTSS_COLOROP, s_rsCache.op0Color);
        m_device->SetTextureStageState(0, D3DTSS_COLORARG1, s_rsCache.op0ColorArg1);
        m_device->SetTextureStageState(0, D3DTSS_COLORARG2, s_rsCache.op0AlphaArg2);
        m_device->SetTextureStageState(0, D3DTSS_ALPHAOP, s_rsCache.op0Alpha);
        m_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, s_rsCache.op0AlphaArg1);
        m_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, s_rsCache.op0AlphaArg2);
        m_device->SetTextureStageState(1, D3DTSS_COLOROP, s_rsCache.op1Color);
        m_device->SetTextureStageState(1, D3DTSS_ALPHAOP, s_rsCache.op1Alpha);
        m_device->SetMaterial(&s_rsCache.material);
        m_device->SetPixelShader(s_rsCache.pixelShader);
        m_device->SetVertexShader(s_rsCache.vertexShader);

        m_device->SetTransform(D3DTS_VIEW, &s_rsCache.view);
        m_device->SetTransform(D3DTS_PROJECTION, &s_rsCache.projection);
    }

    void GUIRender::setCommonRenderState()
    {
        m_device->SetRenderState(D3DRS_ZENABLE, FALSE);
        m_device->SetRenderState(D3DRS_LIGHTING, FALSE);
        m_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

        m_device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

        m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

        m_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        m_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

        m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        m_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        m_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        m_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        m_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        m_device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
        m_device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    }


}//end namespace Lazy
