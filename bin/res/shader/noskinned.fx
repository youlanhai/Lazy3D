#include "light.fx"

float4x4    mWorld : WORLD;
float4x4    mWorldViewProj : WORLDVIEWPROJECTION;

///////////////////////////////////////////////////////
struct VS_INPUT
{
    float4  Pos             : POSITION;
    float3  Normal          : NORMAL;
    float3  Tex0            : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4  Pos     : POSITION;
    float4  Diffuse : COLOR;
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

VS_OUTPUT VShade(VS_INPUT i)
{
    VS_OUTPUT   o;
    o.Pos = mul(i.Pos, mWorldViewProj);

    float3 Normal = mul(i.Normal, mWorld);    
    Normal = normalize(Normal);

    float3 Pos = mul(i.Pos, mWorld);

    o.Diffuse.xyz = Light(Pos, Normal);
    o.Diffuse.w = 1.0f;

    o.Tex0  = i.Tex0.xy;
    return o;
}

float4 PSShade(
    float4  Diffuse : COLOR,
    float2  Tex0    : TEXCOORD0) : COLOR
{
    return tex2D(textureSampler, Tex0) * Diffuse;
}

//////////////////////////////////////
// Techniques specs follow
//////////////////////////////////////
technique t0
{
    pass p0
    {
        VertexShader = compile vs_2_0 VShade();
        PixelShader = compile ps_2_0 PSShade();
    }
}

