#include "light.fx"

float4x4    mWorld : WORLD;
float4x4    mWorldViewProj : WORLDVIEWPROJECTION;
float3      g_cameraPosition : CAMERAPOSITION;

struct VS_INPUT
{
    float4  Pos     : POSITION;
    float3  Normal  : NORMAL;
    float3  Tex0    : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4  Pos     : POSITION;
    float4  Normal  : COLOR0;
    float4  EyeNml  : COLOR1;
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

//////////////////////////////////////////////////
/// vertex shader
//////////////////////////////////////////////////

VS_OUTPUT vsVertexLight(VS_INPUT i)
{
    VS_OUTPUT o = (VS_OUTPUT)0;
    o.Pos = mul(i.Pos, mWorldViewProj);

    float3 Normal = mul(i.Normal, mWorld);    
    float3 Pos = mul(i.Pos, mWorld);

    o.Normal.xyz = Light(normalize(g_cameraPosition - Pos), normalize(Normal));
    o.Normal.w = 1.0f;
    o.Tex0  = i.Tex0.xy;
    return o;
}

VS_OUTPUT vsPixelLight(VS_INPUT i)
{
    VS_OUTPUT o;

    o.Pos = mul(i.Pos, mWorldViewProj);
    float3 Pos = mul(i.Pos, mWorld);
    float3 Normal = mul(i.Normal, mWorld);    

    o.EyeNml = float4(normalize(g_cameraPosition - Pos), 0.0f);
    o.Normal = float4(normalize(Normal), 0.0f);
    o.Tex0   = i.Tex0.xy;
    return o;
}

#include "light_ps.ps"
//////////////////////////////////////
// Techniques specs follow
//////////////////////////////////////
technique pixelLight
{
    pass p0
    {
        VertexShader = compile vs_2_0 vsPixelLight();
        PixelShader  = compile ps_2_0 psPixelLight();
    }
}

technique vertexLight
{
    pass p0
    {
        VertexShader = compile vs_2_0 vsVertexLight();
        PixelShader  = compile ps_2_0 psVertexLight();
    }
}
