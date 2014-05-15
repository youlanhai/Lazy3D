//D3DLight.cpp 
#include "stdafx.h"
#include "Light.h"

CLight::CLight(void)
{
}

CLight::~CLight(void)
{
}

void CLight::SetPointLight(IDirect3DDevice9 *device, const D3DXVECTOR3 &position, D3DXCOLOR color)
{
	ZeroMemory(&d3dlight,sizeof(d3dlight));
	d3dlight.Type			= D3DLIGHT_POINT;
	d3dlight.Ambient		= color * 0.6f;//将环境光减弱0.6
	d3dlight.Diffuse		= color;
	d3dlight.Specular		= color * 0.6f;
	d3dlight.Position		= position;
	d3dlight.Range			= 1000.0f;
	d3dlight.Falloff		= 1.0f;
	d3dlight.Attenuation0	= 1.0f;//不衰减
	d3dlight.Attenuation1	= 0.0f;
	d3dlight.Attenuation2	= 0.0f;
	device->SetLight(0,&d3dlight);
	device->LightEnable(0,true);
}

void CLight::SetSpotLight(IDirect3DDevice9 *device,
    const D3DXVECTOR3 &position, 
    const D3DXVECTOR3 &direction, D3DXCOLOR color)
{
	ZeroMemory(&d3dlight,sizeof(d3dlight));
	d3dlight.Type			= D3DLIGHT_SPOT;
	d3dlight.Ambient		= color * 0.0f;//不需要环境光
	d3dlight.Diffuse		= color;
	d3dlight.Specular		= color * 0.6f;
	d3dlight.Position		= position;
	d3dlight.Direction		= direction;
	d3dlight.Range			= 1000.0f;
	d3dlight.Falloff		= 1.0f;
	d3dlight.Attenuation0	= 1.0f;
	d3dlight.Attenuation1	= 0.0f;
	d3dlight.Attenuation2	= 0.0f;
	d3dlight.Theta			= 0.4f;
	d3dlight.Phi			= 0.9f;
	device->SetLight(0,&d3dlight);
	device->LightEnable(0,true);
}

void CLight::SetDirectionLight(IDirect3DDevice9 *device, 
    const D3DXVECTOR3 &direction, D3DXCOLOR color)
{
	ZeroMemory(&d3dlight,sizeof(d3dlight));
	d3dlight.Diffuse	= color;
	d3dlight.Type		= D3DLIGHT_DIRECTIONAL;
	d3dlight.Direction	= direction;
	device->SetLight(0,&d3dlight);
	device->LightEnable(0,true);
}

void CLight::SetDirectionLight1(IDirect3DDevice9 *device, 
    const D3DXVECTOR3 &direction, D3DXCOLOR color)
{
	ZeroMemory(&d3dlight,sizeof(d3dlight));
	d3dlight.Ambient		= color * 0.1f;
	d3dlight.Diffuse		= color;
	d3dlight.Specular		= color * 0.5f;
	d3dlight.Type		= D3DLIGHT_DIRECTIONAL;
	d3dlight.Direction	= direction;
	device->SetLight(0,&d3dlight);
	device->LightEnable(0,true);
}