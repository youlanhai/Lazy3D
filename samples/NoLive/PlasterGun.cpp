#include "../LZ3DEngine/Commen.h"
#include "../LZ3DEngine/Light.h"
#include "../LZ3DEngine/Material.h"
#include "PlasterGun.h"


CPlasterGun::CPlasterGun(void)
{
}

CPlasterGun::~CPlasterGun(void)
{
}


bool CPlasterGun::init(IDirect3DDevice9 * pDevice,int nType,D3DXVECTOR3 & pos)
{
    m_timer.create(500.0f);
    m_nType = nType;
    m_vPos = pos;
    
    m_square.init(pDevice,1,500);
	m_target.init(pDevice,16,16);

    return true;
}

void CPlasterGun::clear()
{

}

void CPlasterGun::update(float fElapse)
{
    m_timer.update(fElapse*1000.0f);
}


void CPlasterGun::render(IDirect3DDevice9 * pDevice,D3DXMATRIXA16 * pMatWord)
{
    D3DXMATRIXA16 matWord,matScale,matTrans,matRot;

	{//渲染靶心
		D3DXMatrixScaling(&matScale,0.5f,0.5f,0.5f);
		D3DXMatrixTranslation(&matTrans,m_vPos.x,m_vPos.y, m_vPos.z+100.0f);
		D3DXMatrixRotationX(&matRot,D3DX_PI/2.0f);
		matWord = matRot * matScale * matTrans * (*pMatWord);
		pDevice->SetTransform(D3DTS_WORLD,&matWord);
		pDevice->SetTexture(0, getTexMgr()->get("gun/Crosshair.dds"));

		pDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
        pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
        pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
        pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

        pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

        pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );

        pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
        pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

        m_target.render(pDevice); 
        
        pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
        pDevice->SetRenderState( D3DRS_LIGHTING, TRUE);

	}

    float z = m_vPos.z;
    if (isRunning())
    {
        //机器后座力体现
        z = z-(1.0f-m_timer.getPercent())*2.0f;

        //渲染火花
        D3DXMatrixScaling(&matScale,0.5f,1.0f,1.0f);
        D3DXMatrixTranslation(&matTrans,m_vPos.x,m_vPos.y, m_vPos.z+10.0f);
        matWord = matScale * matTrans * (*pMatWord);
        pDevice->SetTransform(D3DTS_WORLD,&matWord);
        pDevice->SetTexture(0, getTexMgr()->get("gun/MACHINEGUNFLARE.DDS"));

        pDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
        //pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
        pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
        pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

        pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

        pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );

        pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
        pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

        m_square.render(pDevice); 
        
        pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
        pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
        pDevice->SetRenderState( D3DRS_LIGHTING, TRUE);
    }

    //渲染机器
    CMesh *pMesh = m_meshs.getAt(0);
    if (pMesh != NULL)
    {
        D3DXMatrixRotationY(&matRot,D3DX_PI);
        D3DXMatrixTranslation(&matTrans,m_vPos.x,m_vPos.y, z);
        matWord = matRot * matTrans * (*pMatWord);
        pDevice->SetTransform(D3DTS_WORLD,&matWord);

        D3DLIGHT9 oldLight;
        pDevice->GetLight(0,&oldLight);
        CLight light;
        light.SetDirectionLight1(pDevice,D3DXVECTOR3(0.0f,1.0f,0.0f),D3DXCOLOR(0.0f,0.0f,1.0f,1.0f));
        pDevice->SetRenderState( D3DRS_LIGHTING, TRUE);

        pDevice->SetTexture(0,0);
        pMesh->render();

        pDevice->SetLight(0,&oldLight);
    }
}

bool CPlasterGun::canFire()
{
    return (!isRunning());
}

bool CPlasterGun::isRunning()
{
    return m_timer.isRunning();
}

void CPlasterGun::fire()
{
    if (canFire())
    {
        m_timer.reset();
        m_timer.start();
    }
}