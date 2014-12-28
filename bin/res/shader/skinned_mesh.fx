
#include "light.fx"
#include "skinned.fx"

float4x4    mViewProj : VIEWPROJECTION;

texture g_texture;
sampler textureSampler = sampler_state
{
    Texture = <g_texture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

#include "light_ps.ps"
///////////////////////////////////////////////////////
struct VS_INPUT
{
    float4  Pos             : POSITION;
    float4  BlendWeights    : BLENDWEIGHT;
    float4  BlendIndices    : BLENDINDICES;
    float3  Normal          : NORMAL;
    float3  Tex0            : TEXCOORD0;
};

VS_OUTPUT vsMain(VS_INPUT i, uniform int NumBones)
{
    VS_OUTPUT   o;
     
    float3 Pos, Normal;
    skinTransform(i.Pos, i.Normal, NumBones, i.BlendWeights, i.BlendIndices, Pos, Normal);

    o.Pos = mul(float4(Pos.xyz, 1.0f), mViewProj);
    o.EyeNml = float4(normalize(g_cameraPosition - Pos), 0.0f);
    o.Normal = float4(normalize(Normal), 0.0f);
    o.Tex0  = i.Tex0.xy;
    return o;
}

int CurNumBones = 2;
VertexShader vsArray[4] = {
    compile vs_2_0 vsMain(1), 
    compile vs_2_0 vsMain(2),
    compile vs_2_0 vsMain(3),
    compile vs_2_0 vsMain(4)
};

//////////////////////////////////////
// Techniques specs follow
//////////////////////////////////////
technique pixelLight
{
    pass p0
    {
        VertexShader = (vsArray[CurNumBones]);
        PixelShader = compile ps_2_0 psPixelLight();
    }
}
