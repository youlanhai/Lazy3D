//cSkyBox.cpp
#include "stdafx.h"

#include "Res.h"
#include "SkyBox.h"

#include "../utility/Utility.h"
#include "../Render/Texture.h"

struct SkyVertex
{
    SkyVertex(float x, float y, float z, float u, float v)
    {
        this->x = x; this->y = y; this->z = z;
        this->u = u; this->v = v;
    }
    float x,y,z;
    float u,v;
    static  DWORD FVF;
    static  UINT SIZE;
};
DWORD SkyVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;
UINT SkyVertex::SIZE = sizeof(SkyVertex);


//////////////////////////////////////////////////////////////////////////

cSkyBox::cSkyBox()
    : m_min(-5000, -2000, -5000)
    , m_max(5000, 3000, 5000)
{
    m_pVertexBuffer = NULL;
    m_pSource = NULL;
    m_visible = true;
}

cSkyBox::~cSkyBox(void)
{
    SafeRelease(m_pVertexBuffer);
}

///创建顶点
bool cSkyBox::create(IDirect3DDevice9 *pDevice)
{
    assert(m_pVertexBuffer == nullptr && "VB has been created!");

    if(FAILED(pDevice->CreateVertexBuffer(
        24* SkyVertex::SIZE, 
        0, 
        SkyVertex::FVF, 
        D3DPOOL_MANAGED,
        &m_pVertexBuffer, 
        0)))
    {
        return false;
    }

    fillVertext();

    return true;
}

void cSkyBox::setSkyRange(const Math::Vector3 & min_, const Math::Vector3 & max_)
{
    m_min = min_;
    m_max = max_;

    if (m_pVertexBuffer) fillVertext();
}

void cSkyBox::fillVertext()
{
    assert(m_pVertexBuffer != nullptr);

    SkyVertex *skyvertex = nullptr;
    if(FAILED(m_pVertexBuffer->Lock(0, 0, (void**)&skyvertex, 0)))
    {
        return;
    }

    //北面
    skyvertex[0] = SkyVertex(m_min.x, m_max.y, m_max.z, 0.0f, 0.0f);
    skyvertex[1] = SkyVertex(m_max.x, m_max.y, m_max.z, 1.0f, 0.0f);
    skyvertex[2] = SkyVertex(m_min.x, m_min.y, m_max.z, 0.0f, 1.0f);
    skyvertex[3] = SkyVertex(m_max.x, m_min.y, m_max.z, 1.0f, 1.0f);
    //东面
    skyvertex[4] = SkyVertex(m_max.x, m_max.y, m_max.z, 0.0f, 0.0f);
    skyvertex[5] = SkyVertex(m_max.x, m_max.y, m_min.z, 1.0f, 0.0f);
    skyvertex[6] = SkyVertex(m_max.x, m_min.y, m_max.z, 0.0f, 1.0f);
    skyvertex[7] = SkyVertex(m_max.x, m_min.y, m_min.z, 1.0f, 1.0f);
    //西面
    skyvertex[8]  = SkyVertex(m_max.x, m_max.y, m_min.z, 0.0f, 0.0f);
    skyvertex[9]  = SkyVertex(m_min.x, m_max.y, m_min.z, 1.0f, 0.0f);
    skyvertex[10] = SkyVertex(m_max.x, m_min.y, m_min.z, 0.0f, 1.0f);
    skyvertex[11] = SkyVertex(m_min.x, m_min.y, m_min.z, 1.0f, 1.0f);
    //南面
    skyvertex[12] = SkyVertex(m_min.x, m_max.y, m_min.z, 0.0f, 0.0f);
    skyvertex[13] = SkyVertex(m_min.x, m_max.y, m_max.z, 1.0f, 0.0f);
    skyvertex[14] = SkyVertex(m_min.x, m_min.y, m_min.z, 0.0f, 1.0f);
    skyvertex[15] = SkyVertex(m_min.x, m_min.y, m_max.z, 1.0f, 1.0f);
    //顶面
    skyvertex[16] = SkyVertex(m_min.x, m_max.y, m_min.z, 0.0f, 0.0f);
    skyvertex[17] = SkyVertex(m_max.x, m_max.y, m_min.z, 1.0f, 0.0f);
    skyvertex[18] = SkyVertex(m_min.x, m_max.y, m_max.z, 0.0f, 1.0f);
    skyvertex[19] = SkyVertex(m_max.x, m_max.y, m_max.z, 1.0f, 1.0f);
    //底面
    skyvertex[20] = SkyVertex(m_min.x, m_min.y, m_max.z, 0.0f, 0.0f);
    skyvertex[21] = SkyVertex(m_max.x, m_min.y, m_max.z, 1.0f, 0.0f);
    skyvertex[22] = SkyVertex(m_min.x, m_min.y, m_min.z, 0.0f, 1.0f);
    skyvertex[23] = SkyVertex(m_max.x, m_min.y, m_min.z, 1.0f, 1.0f);


    m_pVertexBuffer->Unlock();
}

void cSkyBox::render(IDirect3DDevice9 *pDevice)
{
    if (!m_visible || NULL==pDevice)
    {
        return ;
    }

    if (m_pVertexBuffer == nullptr)
    {
        create(pDevice);
    }

    Matrix4x4 world;
    if (m_pSource)
    {
        D3DXVECTOR3 pos = m_pSource->getPos();
        D3DXMatrixTranslation(&world, pos.x, pos.y, pos.z);
    }
    else
    {
        D3DXMatrixIdentity(&world);
    }

    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

    pDevice->SetTransform(D3DTS_WORLD, &world);
    pDevice->SetStreamSource(0, m_pVertexBuffer, 0, SkyVertex::SIZE);
    pDevice->SetFVF(SkyVertex::FVF);

    std::wstring name(512, L'\0');

    for(int i = 0; i<6; i++)
    {
        Lazy::formatStringW(name, m_texNames.c_str(), i);

        dx::Texture *pTex = Lazy::TextureMgr::instance()->getTexture(name);
        if (pTex)
        {
            pDevice->SetTexture(0, pTex);
            pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, i * 4, 2);
        }
    }

    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}