
#include "stdafx.h"
#include "Face.h"
#include "Res.h"
#include "../Render/Texture.h"

namespace Lazy
{

    C2DFace::C2DFace(void)
    {
        m_pVertexBuffer = NULL;
    }

    C2DFace::~C2DFace(void)
    {
        SafeRelease(m_pVertexBuffer);
    }

    /*
    ** x,y,z为显示区域的中心坐标
    */
    bool C2DFace::init(IDirect3DDevice9 *pDevice, float x, float y, float w, float h)
    {
        //创建顶点缓冲区
        if(FAILED(pDevice->CreateVertexBuffer(VertexRHWUV_SIZE * 4,
                                              0, VertexRHWUV::FVF, D3DPOOL_MANAGED, &m_pVertexBuffer, NULL)))
        {
            return false;
        }

        VertexRHWUV vertex[4] =
        {
            x,      y,      1.0f, 1.0f, 0.0f, 0.0f,
            x + w,    y,      1.0f, 1.0f, 1.0f, 0.0f,
            x + w,    y + h,    1.0f, 1.0f, 1.0f, 1.0f,
            x,      y + h,    1.0f, 1.0f, 0.0f, 1.0f,
        };
        return updateVertex(vertex);
    }

    bool C2DFace::updateVertex(VertexRHWUV pVertex[4])
    {
        void* pp = NULL;
        if(FAILED(m_pVertexBuffer->Lock(0, VertexRHWUV_SIZE * 4, (void**)&pp, 0)))
        {
            return false;
        }
        memcpy(pp, pVertex, VertexRHWUV_SIZE * 4);
        m_pVertexBuffer->Unlock();
        return true;
    }

    void C2DFace::render(IDirect3DDevice9 *pDevice)
    {
        //设置顶点缓冲区
        pDevice->SetStreamSource(0, m_pVertexBuffer, 0, VertexRHWUV_SIZE);
        pDevice->SetFVF(VertexRHWUV::FVF);//设置灵活顶点格式
        pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
    }

//////////////////////////////////////////////////////////////////////////


    ProgressBar::ProgressBar()
        : pTexFore(NULL)
        , pTexBack(NULL)
        , progress(0.0f)
        , crFore(0xaf000000)
        , crBack(0x8fffffff)
        , crBack2(0x4f000000)
        , speed(0)
        , curProgress(0)
    {
        x1 = y1 = w1 = h1 = 0.0f;
        x2 = y2 = w2 = h2 = 0.0f;
    }
    ProgressBar::~ProgressBar()
    {

    }

    void ProgressBar::update(float elapse)
    {
        if(speed == 0.0f) return;

        curProgress += speed * elapse;
        if (speed > 0.0f)
        {
            if(curProgress > progress) curProgress = progress;
            speed = 0.0f;
        }
        else if(speed < 0.0f)
        {
            if(curProgress < progress) curProgress = progress;
            speed = 0.0f;
        }
    }
    void ProgressBar::render(LPDIRECT3DDEVICE9 pDevice) const
    {
        pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        pDevice->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );
        pDevice->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA);

        {
            VertexRHWColorUV vertexB[4] =
            {
                x2, y2, 1.0f, 1.0f, crBack, 0.0f, 0.0f,
                x2 + w2, y2, 1.0f, 1.0f, crBack, 1.0f, 0.0f,
                x2 + w2, y2 + h2, 1.0f, 1.0f, crBack, 1.0f, 1.0f,
                x2, y2 + h2, 1.0f, 1.0f, crBack, 0.0f, 1.0f,
            };

            pDevice->SetTexture(0, pTexBack);
            pDevice->SetFVF(VertexRHWColorUV::FVF);
            pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertexB, VertexRHWColorUV_SIZE);
        }

        if(!pTexBack)
        {
            VertexRHWColorUV vertexB[4] =
            {
                x1, y1, 1.0f, 1.0f, crBack2, 0.0f, 0.0f,
                x1 + w1, y1, 1.0f, 1.0f, crBack2, 1.0f, 0.0f,
                x1 + w1, y1 + h1, 1.0f, 1.0f, crBack2, 1.0f, 1.0f,
                x1, y1 + h1, 1.0f, 1.0f, crBack2, 0.0f, 1.0f,
            };

            pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertexB, VertexRHWColorUV_SIZE);
        }

        {
            VertexRHWColorUV vertexF[4] =
            {
                x1, y1, 1.0f, 1.0f, crFore, 0.0f, 0.0f,
                x1 + w1, y1, 1.0f, 1.0f, crFore, 1.0f, 0.0f,
                x1 + w1, y1 + h1, 1.0f, 1.0f, crFore, 1.0f, 1.0f,
                x1, y1 + h1, 1.0f, 1.0f, crFore, 0.0f, 1.0f,
            };
            if (pTexFore)
            {
                vertexF[1].u = curProgress;
                vertexF[2].u = curProgress;
            }
            vertexF[1].x = x1 + w1 * curProgress;
            vertexF[2].x = x1 + w1 * curProgress;

            pDevice->SetTexture(0, pTexFore);
            pDevice->SetFVF(VertexRHWColorUV::FVF);
            pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertexF, VertexRHWColorUV_SIZE);
        }

        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    }

    void ProgressBar::setForeImage(const std::wstring & image)
    {
        pTexFore = TextureMgr::instance()->getTexture(image);
    }
    void ProgressBar::setBackImage(const std::wstring & image)
    {
        pTexBack = TextureMgr::instance()->getTexture(image);
    }

    void ProgressBar::setProgress(float p, bool forceSame)
    {
        progress = p;

        if(progress < 0.0f) progress = 0.0f;
        else if(progress > 1.0f) progress = 1.0f;

        if(forceSame)
        {
            curProgress = progress;
            speed = 0.0f;
        }
        else speed = (progress - curProgress) / 0.5f;
    }

} // end namespace Lazy
