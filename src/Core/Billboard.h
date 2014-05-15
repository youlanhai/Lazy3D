#pragma once

#include "base.h"
#include "RenderObj.h"

/** 公告板*/
class LZDLL_API cBillboard : public IBase, public IRenderObj
{
public:
    cBillboard(void);
    ~cBillboard(void);

    bool createVertex(IDirect3DDevice9 *pDevice);

    void setSize(float w, float h);

    void setImage(const std::wstring & texName);

    void setTexture(IDirect3DTexture9* pTex){ m_pTexture = pTex; }

    void setPos(D3DXVECTOR3 pos){ m_pos = pos; }

    virtual void update(float fElapse);

    virtual void render(IDirect3DDevice9 * pDevice);

    void show(bool s){ m_show = s; }
    bool visible(){ return m_show; }

    void setMaxShowDistance(float s){ m_maxShowDistance = s; }

    float getShowMaxDistance(){ return m_maxShowDistance; }

    bool isTooFar();

protected:

    void updateVertex();

protected:
    IDirect3DVertexBuffer9  *m_pVertex;
    IDirect3DTexture9       *m_pTexture;
    D3DXVECTOR3             m_pos;
    float                   m_width;
    float                   m_height;
    bool                    m_show;
    float                   m_maxShowDistance;
};

