
float4x4 g_world;
float4x4 g_viewProj : WORLDVIEWPROJECTION;

texture g_texture;
sampler textureSampler = sampler_state
{
    Texture = <g_texture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

#include "light_shadowmap.fx"
#include "light_ps.ps"
#include "skinned.fx"
///////////////////////////////////////////////////////
struct VS_INPUT
{
    float4  Pos             : POSITION;
    float4  BlendWeights    : BLENDWEIGHT;
    float4  BlendIndices    : BLENDINDICES;
    float3  Normal          : NORMAL;
    float4  Tex0            : TEXCOORD0;
};

VS_OUTPUT vsMain(VS_INPUT i, uniform int NumBones)
{
    VS_OUTPUT o = (VS_OUTPUT) 0;
     
    float3 Pos, Normal;
    skinTransform(i.Pos, i.Normal, NumBones, i.BlendWeights, i.BlendIndices, Pos, Normal);

    o.vPos = float4(Pos, 1.0f);
    o.vNml = normalize(Normal);

    o.pos = mul(o.vPos, g_viewProj);
    o.uv.xy = float4(i.Tex0.xy, o.vPos.zw);

#ifdef USE_SHADOW_MAP
    o.vPosInLight = mul(o.vPos, g_shadowMapMatrix);
#endif
    return o;
}

int CurNumBones = 2;
VertexShader vsArray[4] = {
    compile vs_2_0 vsMain(1), 
    compile vs_2_0 vsMain(2),
    compile vs_2_0 vsMain(3),
    compile vs_2_0 vsMain(4)
};

#ifdef USE_SHADOW_MAP
float4 psShadowMap2(
    float4 depth : TEXCOORD0) : COLOR0
{
    return depth.z / depth.w;
}

technique tech_shadowmap
{
    pass p0
    {
        VertexShader = (vsArray[CurNumBones]);
        PixelShader  = compile ps_2_0 psShadowMap2();
    }
}
#endif
//////////////////////////////////////
// Techniques specs follow
//////////////////////////////////////
technique tech_default
{
    pass p0
    {
        VertexShader = (vsArray[CurNumBones]);
        PixelShader = compile ps_2_0 psPixelLight();
    }
}

