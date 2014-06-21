//CSceneFog.cpp
#include "stdafx.h"
#include "SceneFog.h"

namespace Lazy
{

    CSceneFog::CSceneFog(void)
    {
        m_pDevice = NULL;
        m_ftType = FT_LINEAR | FT_PIXEL; //雾化类型
        m_fStart = 500.0f;
        m_fEnd = 2000.0f;
        m_crColor = D3DCOLOR_XRGB(200, 199, 172);
        m_bRange = true;
        m_fDensity = 1.0f;//密度
        m_visible = true;
    }

    CSceneFog::~CSceneFog(void)
    {
    }

///初始化
    void CSceneFog::init(IDirect3DDevice9 *pDevice)
    {

        m_pDevice = pDevice;
    }

    void CSceneFog::setFog(void)
    {
        if (!m_visible)
        {
            return;
        }
        assert(m_pDevice != NULL && "CSceneFog::setFog");

        DWORD flag = (DWORD)m_ftType;
        bool bPixel = ((flag & (DWORD)FT_PIXEL) != 0);

        m_pDevice->SetRenderState(D3DRS_FOGENABLE, true);
        m_pDevice->SetRenderState(D3DRS_FOGCOLOR, m_crColor);

        if (bPixel)//像素雾化
        {
            flag -= (DWORD)FT_PIXEL;
            switch(flag)
            {
            case FT_LINEAR:
                setLinearPixelFog();
                break;
            case FT_EXP:
                setExpVertexFog();
                break;
            case FT_EXP2:
                setExp2PixelFog();
                break;
            default:
                break;
            }
        }
        else//顶点雾化
        {
            switch(flag)
            {
            case FT_LINEAR:
                setLinearVertexFog();
                break;
            case FT_EXP:
                setExpVertexFog();
                break;
            case FT_EXP2:
                setExp2VertexFog();
                break;
            default:
                break;
            }
        }
    }

    void CSceneFog::closeFog(void)
    {
        m_pDevice->SetRenderState(D3DRS_FOGENABLE, false);
    }

    void CSceneFog::setFogType(DWORD ft)
    {
        m_ftType = ft;
    }

    void CSceneFog::setLinearVertexFog(void)
    {
        m_pDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
        m_pDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD*)&m_fStart);
        m_pDevice->SetRenderState(D3DRS_FOGEND, *(DWORD*)&m_fEnd);
        m_pDevice->SetRenderState(D3DRS_RANGEFOGENABLE, m_bRange);
    }

    void CSceneFog::setExpVertexFog(void)
    {
        m_pDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_EXP);
        m_pDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD*)&m_fDensity);
        m_pDevice->SetRenderState(D3DRS_RANGEFOGENABLE, m_bRange);
    }

    void CSceneFog::setExp2VertexFog(void)
    {
        m_pDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_EXP2);
        m_pDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD*)&m_fDensity);
        m_pDevice->SetRenderState(D3DRS_RANGEFOGENABLE, m_bRange);
    }

    void CSceneFog::setLinearPixelFog(void)
    {
        m_pDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
        m_pDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD*)&m_fStart);
        m_pDevice->SetRenderState(D3DRS_FOGEND, *(DWORD*)&m_fEnd);
    }

    void CSceneFog::setExpPixelFog(void)
    {
        m_pDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP);
        m_pDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD*)&m_fDensity);
        m_pDevice->SetRenderState(D3DRS_FOGEND, *(DWORD*)&m_fEnd);
    }

    void CSceneFog::setExp2PixelFog(void)
    {
        m_pDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP2);
        m_pDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD*)&m_fDensity);
        m_pDevice->SetRenderState(D3DRS_FOGEND, *(DWORD*)&m_fEnd);
    }

} // end namespace Lazy
