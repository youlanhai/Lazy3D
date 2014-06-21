#pragma once

#include "Base.h"

namespace Lazy
{
    struct VertexRHWUV
    {
        float x, y, z, rhw;
        float u, v;

        const static DWORD FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;
    };
    const DWORD VertexRHWUV_SIZE = sizeof(VertexRHWUV);

    struct VertexRHWColorUV
    {
        float x, y, z, rhw;
        DWORD color;
        float u, v;

        const static DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
    };
    const DWORD VertexRHWColorUV_SIZE = sizeof(VertexRHWColorUV);


    /* 2d界面类
    ** 主要用于向屏幕贴图
    */
    class LZDLL_API C2DFace : public IBase
    {
    public:
        C2DFace(void);

        virtual ~C2DFace(void);

        bool init(IDirect3DDevice9 *pDevice, float x, float y, float w, float h);

        bool updateVertex(VertexRHWUV pVertex[4]);

        void render(IDirect3DDevice9 *pDevice);
    private:
        IDirect3DVertexBuffer9 *m_pVertexBuffer;
    };

    class LZDLL_API ProgressBar : public IBase
    {
    public:
        ProgressBar();
        virtual ~ProgressBar();

        void update(float elapse);
        void render(LPDIRECT3DDEVICE9 pDevice) const;

        void setForeImage(const std::wstring & image);
        void setBackImage(const std::wstring & image);

        void setForeColor(DWORD cr) { crFore = cr; }
        void setBackColor(DWORD cr) { crBack = cr; }
        void setBack2Color(DWORD cr) { crBack2 = cr; }

        void setForeRect(float x, float y, float w, float h)
        {
            x1 = x;
            y1 = y;
            w1 = w;
            h1 = h;
        }
        void setBackRect(float x, float y, float w, float h)
        {
            x2 = x;
            y2 = y;
            w2 = w;
            h2 = h;
        }

        void setProgress(float p, bool forceSame = true);
        float getProgress() const { return progress; }

    private:
        float progress;
        float curProgress;
        float speed;

        float x1, y1, w1, h1;
        float x2, y2, w2, h2;

        DWORD crFore, crBack, crBack2;

        LPDIRECT3DTEXTURE9 pTexFore;
        LPDIRECT3DTEXTURE9 pTexBack;
    };


}//namespace Lazy