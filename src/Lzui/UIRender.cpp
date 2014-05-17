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
        DWORD clip;
        CRect clipRect;
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

    int UIVertex::SIZE = sizeof(UIVertex);
    DWORD UIVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

    class SpiritBatcher : public SimpleSingleton<SpiritBatcher>
    {
        TexturePtr  m_texture;
        EffectPtr   m_shader;
        std::vector<UIVertex> m_vertices;
        dx::Device  *m_device;
        Math::Matrix4x4   m_matWorldViewProj;

    public:
        SpiritBatcher()
            : m_device(nullptr)
        {}

        ~SpiritBatcher()
        {}

        void setDevice(dx::Device * pDevice)
        {
            m_device = pDevice;
        }

        void setMatrix(const Math::Matrix4x4 & mat)
        {
            m_matWorldViewProj = mat;
        }

        void draw(const UVRect & dstRect, const UVRect & srcRect, uint32 color, TexturePtr tex, EffectPtr shader)
        {
            assert(shader);

            if (tex != m_texture || shader != m_shader)
            {
                realDraw();
            }

            m_texture = tex;
            m_shader = shader;
            addVertex(dstRect, srcRect, color);
        }

        void realDraw()
        {
            if(m_vertices.empty()) return;
            assert(m_device);

            if (m_texture)
            {
                m_shader->setTexture("g_texture", m_texture->getTexture());
            }

            m_shader->setMatrix("g_worldViewProjection", m_matWorldViewProj);

            UINT nPass;
            if (m_shader->begin(nPass))
            {
                for (UINT i = 0; i < nPass; ++i)
                {
                    if (m_shader->beginPass(i))
                    {
                        m_device->SetFVF(UIVertex::FVF);
                        m_device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 
                            m_vertices.size() / 3, &m_vertices[0], UIVertex::SIZE);

                        m_shader->endPass();
                    }
                }
                m_shader->end();
            }

            m_vertices.clear();
            m_texture = nullptr;
            m_shader = nullptr;
        }

    private:

        void addVertex(const UVRect & dstRect, const UVRect & srcRect, uint32 color)
        {
            UIVertex vertex;
            float z = 1.0f;

            vertex.position.set(dstRect.left, dstRect.top, z);
            vertex.color = color;
            vertex.uv.set(srcRect.left, srcRect.top);
            m_vertices.push_back(vertex);

            vertex.position.set(dstRect.right, dstRect.top, z);
            vertex.uv.set(srcRect.right, srcRect.top);
            m_vertices.push_back(vertex);

            size_t i = m_vertices.size();
            vertex.position.set(dstRect.left, dstRect.bottom, z);
            vertex.uv.set(srcRect.left, srcRect.bottom);
            m_vertices.push_back(vertex);

            m_vertices.push_back(m_vertices[i]);
            m_vertices.push_back(m_vertices[i - 1]);

            vertex.position.set(dstRect.right, dstRect.bottom, z);
            vertex.uv.set(srcRect.right, srcRect.bottom);
            m_vertices.push_back(vertex);
        }
    };




    GUIRender::GUIRender(dx::Device * m_device, HWND hWnd)
        : m_device(m_device)
        , m_hWnd(hWnd)
        , m_bClip(false)
    {
        if (m_device) m_device->AddRef();

        ZeroMemory(&s_rsCache, sizeof(s_rsCache));

        m_matWorldViewProj.makeIdentity();
    }

    GUIRender::~GUIRender()
    {
        if (m_device) m_device->Release();
    }

    bool GUIRender::isClipEnable() const
    {
        return m_bClip;
    }

    void GUIRender::setClipEnalbe(bool enalbe)
    {
        if(m_bClip == enalbe) return;

        m_bClip = enalbe;
        m_device->SetRenderState(D3DRS_SCISSORTESTENABLE, enalbe);
        SpiritBatcher::instance()->realDraw();
    }

    void GUIRender::getClipRect(CRect & rect) const
    {
        rect = m_rcClip;
    }

    void GUIRender::setClipRect(const CRect & rect)
    {
        m_rcClip = rect;
        m_device->SetScissorRect(&rect);
        SpiritBatcher::instance()->realDraw();
    }

    void GUIRender::setTextureShader(const tstring & file)
    {
        m_textureShader = EffectMgr::instance()->get(file);
    }

    void GUIRender::setColorShader(const tstring & file)
    {
        m_colorShader = EffectMgr::instance()->get(file);
    }

    void GUIRender::setFontShader(const tstring & file)
    {
        m_fontShader = EffectMgr::instance()->get(file);
    }

    void GUIRender::drawRect(const CRect & rc, uint32 color, TexturePtr texture)
    {
        drawRect(rc, UVRect(0, 0, 1, 1), color, texture);
    }

    void GUIRender::drawRect(const CRect & rc, const UVRect & srcRc, uint32 color, TexturePtr texture)
    {
        if (texture && texture->getTexture())
            SpiritBatcher::instance()->draw(UVRect(rc), srcRc, color, texture, m_textureShader);
        else
            SpiritBatcher::instance()->draw(UVRect(rc), srcRc, color, nullptr, m_colorShader);
    }

    void GUIRender::drawRect(const CRect & rc, uint32 color)
    {
        SpiritBatcher::instance()->draw(UVRect(rc), CRect(), color, nullptr, m_colorShader);
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

    void GUIRender::drawWord(const CRect & dest, const UVRect & src, uint32 color, TexturePtr texture)
    {
        if (!texture || !texture->getTexture()) return;

        SpiritBatcher::instance()->draw(UVRect(dest), src, color, texture, m_fontShader);
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
        SpiritBatcher::instance()->setDevice(m_device);
        SpiritBatcher::instance()->setMatrix(matrix);

        saveRenderState();
        setCommonRenderState();
    }

    void GUIRender::renderEnd()
    {
        SpiritBatcher::instance()->realDraw();
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


        m_device->GetRenderState(D3DRS_SCISSORTESTENABLE, &s_rsCache.clip);
        m_device->GetScissorRect(&s_rsCache.clipRect);

        m_bClip = s_rsCache.clip != FALSE;
        m_rcClip = s_rsCache.clipRect;
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

        m_device->SetRenderState(D3DRS_SCISSORTESTENABLE, s_rsCache.clip);
        m_device->SetScissorRect(&s_rsCache.clipRect);
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

        setClipEnalbe(false);
    }


}//end namespace Lazy
