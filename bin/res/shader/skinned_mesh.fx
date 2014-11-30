
#include "light.fx"
#include "skinned.fx"

float4x4    mViewProj : VIEWPROJECTION;
float3 CameraPosition : CAMERAPOSITION;

///////////////////////////////////////////////////////
struct VS_INPUT
{
    float4  Pos             : POSITION;
    float4  BlendWeights    : BLENDWEIGHT;
    float4  BlendIndices    : BLENDINDICES;
    float3  Normal          : NORMAL;
    float3  Tex0            : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4  Pos     : POSITION;
    float4  EyeNormal: COLOR0;
    float4  Normal  : COLOR1;
    float2  Tex0    : TEXCOORD0;
};

texture g_texture;

sampler textureSampler = sampler_state
{
    Texture = <g_texture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};


VS_OUTPUT vsMain(VS_INPUT i, uniform int NumBones)
{
    VS_OUTPUT   o;
     
    float3 Pos, Normal;
    skinTransform(i.Pos, i.Normal, NumBones, i.BlendWeights, i.BlendIndices, Pos, Normal);

    o.Pos = mul(float4(Pos.xyz, 1.0f), mViewProj);
    o.EyeNormal = float4(normalize(CameraPosition - Pos), 0.0f);
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


float4 psMainLight(
    float4  EyeNormal : COLOR0,
    float4  Normal  : COLOR1,
    float2  Tex0    : TEXCOORD0) : COLOR
{
    float4 Diffuse;
    Diffuse.rgb = Light(normalize(EyeNormal.xyz), normalize(Normal.xyz));
    Diffuse.a = 1.0f;
    //return Diffuse;
    return tex2D(textureSampler, Tex0) * Diffuse;
}

//////////////////////////////////////
// Techniques specs follow
//////////////////////////////////////
technique t0
{
    pass p0
    {
        VertexShader = (vsArray[CurNumBones]);
        PixelShader = compile ps_2_0 psMainLight();
    }
}
