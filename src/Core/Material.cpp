//Material.cpp
#include "stdafx.h"
#include "Material.h"

namespace Lazy
{

    Material::Material(void)
    {
    }
    Material::~Material(void)
    {
    }

    void Material::setMaterial(
        IDirect3DDevice9 *device,
        D3DCOLORVALUE d,
        D3DCOLORVALUE a,
        D3DCOLORVALUE s,
        D3DCOLORVALUE e,
        float p)
    {
        D3DMATERIAL9 material;
        material.Ambient = a;
        material.Diffuse = d;
        material.Specular = s;
        material.Emissive = e;
        material.Power = p;
        //设置材质
        device->SetMaterial(&material);
    }

    void Material::setMaterial(
        IDirect3DDevice9 *device,
        D3DCOLORVALUE d,    //Diffuse，漫反射颜色
        D3DCOLORVALUE a   //Ambient，环境光颜色
    )
    {
        D3DXCOLOR black(0.0f, 0.0f, 0.0f, 0.0f);
        setMaterial(device, d, a, black, black, 1.0f);
    }

    void Material::setMaterial(
        IDirect3DDevice9 *device,
        D3DCOLORVALUE d    //Diffuse，漫反射颜色
    )
    {
        setMaterial(device, d, D3DXCOLOR(d) * 0.8f);
    }

    /*设置白色材质，s为衰减系数*/
    void Material::setWhiteMaterial(IDirect3DDevice9 *device, float s)
    {
        D3DXCOLOR d(s, s, s, 1.0f);
        setMaterial(device, d, d * 0.6f);
    }

} // end namespace Lazy
