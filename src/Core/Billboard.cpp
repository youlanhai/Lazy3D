
#include "stdafx.h"
#include "Res.h"
#include "CursorCamera.h"
#include "Billboard.h"
#include "App.h"
#include "../Render/Texture.h"



struct BillboardVertex
{
    BillboardVertex(){}
    BillboardVertex(float x_, float y_, float z_, float u_, float v_) 
        : x(x_), y(y_), z(z_), u(u_), v(v_)
    {
    }
    float x, y, z;
    float u, v;
    static int SIZE ;
    static DWORD FVF;
};

int BillboardVertex::SIZE = sizeof(BillboardVertex);
DWORD BillboardVertex::FVF = D3DFVF_XYZ|D3DFVF_TEX1;

//////////////////////////////////////////////////////////////////////////

cBillboard::cBillboard(void)
{
    m_pVertex = NULL;
    m_pTexture = NULL;
    m_pos = D3DXVECTOR3(0, 0, 0);
    m_width = 100;
    m_height = 32;
    m_show = true;
    m_maxShowDistance = 0;
}

cBillboard::~cBillboard(void)
{
    //getApp()->removeBillboard(this);
    SafeRelease(m_pVertex);
}

bool cBillboard::createVertex(IDirect3DDevice9 *pDevice)
{
    SafeRelease(m_pVertex);


    if (FAILED(pDevice->CreateVertexBuffer(
        4*BillboardVertex::SIZE, 
        0,
        BillboardVertex::FVF,
        D3DPOOL_MANAGED,
        &m_pVertex,
        NULL)))
    {
        return false;
    }

    BillboardVertex *pVertex;
    if (FAILED(m_pVertex->Lock(0, 4*BillboardVertex::SIZE, (void**)&pVertex, 0)))
    {
        return false;
    }

    pVertex[0] = BillboardVertex(-0.5f,  0.5f, 0.0f, 0.0f, 0.0f);
    pVertex[1] = BillboardVertex( 0.5f,  0.5f, 0.0f, 1.0f, 0.0f);
    pVertex[2] = BillboardVertex(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f);
    pVertex[3] = BillboardVertex( 0.5f, -0.5f, 0.0f, 1.0f, 1.0f);

    m_pVertex->Unlock();

    return true;
}


void cBillboard::setImage(const std::wstring & texName)
{
    m_pTexture = Lazy::TextureMgr::instance()->getTexture(texName);
}

void cBillboard::setSize(float w, float h)
{
    m_width = w;
    m_height = h;
}

bool cBillboard::isTooFar()
{
    if (m_maxShowDistance > 0)
    {
        D3DXVECTOR3 vecDist = m_pos - getCamera()->position();
        if (D3DXVec3Length(&vecDist) > m_maxShowDistance)
        {
            return true;
        }
    }
    return false;
}

void cBillboard::update(float)
{

}

void cBillboard::updateVertex()
{
    if (!m_pTexture)
    {
        return ;
    }
    D3DSURFACE_DESC desc;
    if (FAILED(m_pTexture->GetLevelDesc(0, &desc)))
    {
        return;
    }

    BillboardVertex *pVertex;
    if (FAILED(m_pVertex->Lock(0, 4*BillboardVertex::SIZE, (void**)&pVertex, 0)))
    {
        return ;
    }

    float u2 = float(m_width) / desc.Width;
    float v2 = min(1.0f, m_height * 1.2f / desc.Height);

    float halfW = 1.0f;
    float halfH = 1.0f;
    pVertex[0] = BillboardVertex( -halfW, halfH, 0, 0.0f, 0.0f);
    pVertex[1] = BillboardVertex( halfW, halfH, 0, u2, 0.0f);
    pVertex[2] = BillboardVertex( -halfW, -halfH, 0, 0.0f, v2);
    pVertex[3] = BillboardVertex( halfW, -halfH, 0, u2, v2);

    m_pVertex->Unlock();
}

void cBillboard::render(IDirect3DDevice9 * pDevice)
{
    if (!m_show || isTooFar())
    {
        return;
    }

    if (NULL == m_pVertex)
    {
        if(!createVertex(pDevice))
        {
            return ;
        }
    }

    Matrix4x4 mat;

    D3DXVECTOR3 look = m_pos - getCamera()->position();
    float dist = D3DXVec3Length(&look);

    float heigth = float(getApp()->getHeight());

    Matrix4x4 matScale;
    float dh = tan(D3DX_PI/8.0f) * dist;

    float sh = m_height / heigth * dh;
    float sw = m_width / heigth * dh;

    D3DXMatrixScaling(&matScale, sw, sh, 1.0f);

    D3DXMatrixRotationX(&mat, getCamera()->pitch());
    Matrix4x4 matRot;
    D3DXMatrixRotationY(&matRot, getCamera()->yaw());

    Matrix4x4 matTrans;
    D3DXMatrixTranslation(&matTrans, m_pos.x, m_pos.y, m_pos.z);

    mat = matScale * mat * matRot * matTrans;
    pDevice->SetTransform(D3DTS_WORLD, &mat);

    pDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
    pDevice->SetRenderState( D3DRS_ZENABLE, TRUE);
    pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
    pDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
    pDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );

    pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    updateVertex();
    pDevice->SetTexture(0, m_pTexture);
    pDevice->SetStreamSource(0, m_pVertex, 0, BillboardVertex::SIZE);
    pDevice->SetFVF(BillboardVertex::FVF);
    pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

    pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

}
