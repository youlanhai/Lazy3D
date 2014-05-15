/**********************************************************
Material.h（材质类）
作用: 设置材质
**********************************************************/
#pragma once

class LZDLL_API CMaterial : public IBase
{
public:
	CMaterial(void);
	~CMaterial(void);
	static void setMaterial(
		IDirect3DDevice9 *device, 
		D3DCOLORVALUE d,    //Diffuse，漫反射颜色
		D3DCOLORVALUE a,    //Ambient，环境光颜色
		D3DCOLORVALUE s,    //Specular，镜面反射颜色
		D3DCOLORVALUE e,    //Emissave，自发光
		float p			    //Power，强度
	);

    static void setMaterial(
        IDirect3DDevice9 *device, 
        D3DCOLORVALUE d,    //Diffuse，漫反射颜色
        D3DCOLORVALUE a    //Ambient，环境光颜色
    );

    static void setMaterial(
        IDirect3DDevice9 *device, 
        D3DCOLORVALUE d    //Diffuse，漫反射颜色
    );
    
    /*设置白色材质，s为衰减系数(0.0~1.0)*/
    static void setWhiteMaterial(IDirect3DDevice9 *device,float s);
};
