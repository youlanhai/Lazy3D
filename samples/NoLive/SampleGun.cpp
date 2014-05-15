#include "../LZ3DEngine/Commen.h"
#include "../LZ3DEngine/Light.h"
#include "../LZ3DEngine/Material.h"
#include "SampleGun.h"

CSampleGun::CSampleGun(void)
{
}

CSampleGun::~CSampleGun(void)
{
}


bool CSampleGun::init(IDirect3DDevice9 * pDevice,int nType,D3DXVECTOR3 & pos)
{
    //m_timer.create(500.0f);
    m_timer.create(100.0f);
    m_nType = nType;
    m_vPos = pos;
    
    m_square.init(pDevice,16,16);
	m_target.init(pDevice,16,16);

    return true;
}

void CSampleGun::clear()
{

}

void CSampleGun::update(float fElapse)
{
    m_timer.update(fElapse*1000.0f);
}


void CSampleGun::render(IDirect3DDevice9 * pDevice,D3DXMATRIXA16 * pMatWord)
{
    D3DXMATRIXA16 matWord,matScale,matTrans,matRot;


    float z = m_vPos.z;
    if (isRunning())
    {
        //机器后座力体现
        z = z-(1.0f-m_timer.getPercent())*2.0f;

        //渲染火花
        float scale = (rand()%30+20)*0.008f;
        D3DXMatrixScaling(&matScale,scale,scale,scale);
        D3DXMatrixTranslation(&matTrans,m_vPos.x,m_vPos.y, z+10.0f);
        D3DXMatrixRotationX(&matRot,D3DX_PI/2.0f);
        matWord = matRot * matScale * matTrans * (*pMatWord);
        pDevice->SetTransform(D3DTS_WORLD,&matWord);
        pDevice->SetTexture(0,getTexMgr()->get("gun/MACHINEGUNFLARE.DDS"));

        pDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
        pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);

        pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
        pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
#if 0
        pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

        pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
        pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
#endif
        m_square.render(pDevice); 
        
        pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
        pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
        pDevice->SetRenderState( D3DRS_LIGHTING, TRUE);
    }

    //渲染机器
    CMesh *pMesh = m_meshs.getAt(m_nType);
    if (pMesh != NULL)
    {
        D3DXMatrixRotationY(&matRot,D3DX_PI);
        D3DXMatrixTranslation(&matTrans,m_vPos.x,m_vPos.y, z);
        matWord = matRot * matTrans * (*pMatWord);
        pDevice->SetTransform(D3DTS_WORLD,&matWord);

        D3DLIGHT9 oldLight;
        pDevice->GetLight(0,&oldLight);
        //CLight light;

        D3DLIGHT9 light; 
        ZeroMemory(&light, sizeof(light));
        light.Type = D3DLIGHT_DIRECTIONAL;
        light.Ambient.a = 1.0f; 
        light.Ambient.g = 0.1f;
        light.Ambient.b = 0.1f;
        light.Ambient.r = 0.1f;
        light.Diffuse.a = 1.0f; 
        light.Diffuse.g = 0.8f;
        light.Diffuse.b = 0.8f;
        light.Diffuse.r = 0.8f;
        light.Direction = D3DXVECTOR3(1.0f, -1.0f, 1.0f);

        pDevice->SetRenderState( D3DRS_LIGHTING, TRUE);
        pDevice->SetLight(0, &light);
        pDevice->LightEnable(0, true);
        //light.SetDirectionLight1(pDevice,D3DXVECTOR3(0.0f,1.0f,0.0f),D3DXCOLOR(2.0f,0.0f,0.0f,1.0f));
        CMaterial::setMaterial(pDevice,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f));
        
        pDevice->SetTexture(0,0);
        pMesh->render();

        pDevice->SetLight(0,&oldLight);
    }
}

bool CSampleGun::canFire()
{
    return (!isRunning());
}

bool CSampleGun::isRunning()
{
    return m_timer.isRunning();
}

void CSampleGun::fire()
{
    if (canFire())
    {
        m_timer.reset();
        m_timer.start();
    }
}