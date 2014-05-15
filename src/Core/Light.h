/**********************************************************
Light.h（光照类）
作用: 设置方向,点,聚光灯三种光源
**********************************************************/

#pragma once

class LZDLL_API CLight : public IBase
{
public:
	CLight(void);
	~CLight(void);
	void SetPointLight(			//设置点光源
		IDirect3DDevice9 *device, 
		const D3DXVECTOR3 &position,	//位置
		D3DXCOLOR color			//颜色
		);
	void SetSpotLight(			//设置聚光灯
		IDirect3DDevice9 *device,
		const D3DXVECTOR3 &position,	//位置
		const D3DXVECTOR3 &direction,	//方向
		D3DXCOLOR color			//颜色
		);
	void SetDirectionLight(		//设置方向光源。只有漫反射颜色
		IDirect3DDevice9 *device, 
		const D3DXVECTOR3 &direction,	//位置
		D3DXCOLOR color			//颜色
		);
	void SetDirectionLight1(	//设置方向光源
		IDirect3DDevice9 *device, 
		const D3DXVECTOR3 &direction,	//位置
		D3DXCOLOR color			//颜色
		);
private:
	D3DLIGHT9 d3dlight;
};
