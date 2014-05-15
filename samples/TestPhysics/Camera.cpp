//D3DCamera.cpp
#include "Camera.h"

//////////////////////////////////////////////////////////////////////////
CCamera::CCamera(IDirect3DDevice9* pDevice, CameraType type/* = THIRD*/)
{
	initCamera(pDevice, type);
}

CCamera::~CCamera(void)
{

}

void CCamera::initCamera(IDirect3DDevice9* pDevice, CameraType type/* = THIRD*/)
{
    m_pd3dDevice = pDevice;
    setCamareType(type);
    m_position = D3DXVECTOR3( 0.0f, 20.0f, -200.0f );
    m_fDistToPlayer = 100.0f;
    setDistRange(10.0f, 300.0f);
    setNearFar(1.0f, 10000.0f);
}

void CCamera::setNearFar(float fNear,float fFar)
{
    m_fNear = fNear;
    m_fFar = fFar;
    setup3DCamera();
}

void CCamera::setCamareType(CameraType cameraType)
{
	m_cameraType = cameraType;
}

void CCamera::setDistRange(float mind, float maxd)
{
    m_distMin = mind;
    m_distMax = maxd;
    correctDist();
}

void CCamera::setDistance(float fDistance)
{
    m_fDistToPlayer = fDistance;
    correctDist();
}

/** 矫正距离玩家的距离。*/
void CCamera::correctDist(void)
{
    if (m_fDistToPlayer<m_distMin)
    {
        m_fDistToPlayer = m_distMin;
    }
    else if (m_fDistToPlayer > m_distMax)
    {
        m_fDistToPlayer = m_distMax;
    }
}

void CCamera::setup3DCamera(void)
{
	D3DXMATRIX projectionMatrix;
	D3DVIEWPORT9 tempViewPort;
	m_pd3dDevice->GetViewport(&tempViewPort);
	D3DXMatrixPerspectiveFovLH( 
        &projectionMatrix,
        D3DX_PI * 0.25f, 
		float(tempViewPort.Width)/float(tempViewPort.Height), 
		m_fNear,
		m_fFar );
	m_pd3dDevice->SetTransform(D3DTS_PROJECTION,&projectionMatrix);
}

void CCamera::setup2DCamera(void)
{
#if 0
	D3DXMATRIX temp;
	D3DXMatrixIdentity(&temp);
	m_pd3dDevice->SetTransform(D3DTS_VIEW, &temp);
#endif

	D3DVIEWPORT9 tempViewPort;
    m_pd3dDevice->GetViewport(&tempViewPort);

    D3DXMATRIX projectionMatrix;
	D3DXMatrixOrthoLH(
        &projectionMatrix, 
		float(tempViewPort.Width),
		float(tempViewPort.Height), 
		m_fNear, 
		m_fFar);
	m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &projectionMatrix);
}
