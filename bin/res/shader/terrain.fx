
#include "light.fx"

float4x4 g_world : WORLD;
float4x4 g_worldViewProj : WORLDVIEWPROJECTION;
float3   g_cameraPosition : CAMERAPOSITION;

texture g_texture0;
texture g_texture1;
texture g_texture2;
texture g_texture3;
texture g_textureBlend;
texture g_textureDiffuse;

#define TERRAIN_SAMPLER(TEXTURE)    \
sampler_state                       \
{                                   \
    Texture = <TEXTURE>;            \
    MipFilter = LINEAR;             \
    MinFilter = LINEAR;             \
    MagFilter = LINEAR;             \
    AddressU  = WRAP;               \
    AddressV  = WRAP;               \
}

sampler samplerDiffuse = TERRAIN_SAMPLER(g_textureDiffuse);
sampler samplerBlend = TERRAIN_SAMPLER(g_textureBlend);

sampler g_samplers[4] = {
    TERRAIN_SAMPLER(g_texture0),
    TERRAIN_SAMPLER(g_texture1),
    TERRAIN_SAMPLER(g_texture2),
    TERRAIN_SAMPLER(g_texture3),
};

struct VS_INPUT
{
    float4 pos : POSITION;
    float3 nml : NORMAL;
    float2 uv1 : TEXCOORD0;
    float2 uv2 : TEXCOORD1;
};

struct VS_OUTPUT
{
    float4 pos : POSITION;
    float2 uv1 : TEXCOORD0;
    float2 uv2 : TEXCOORD1;
};

//////////////////////////////////////////////////
/// vertex shader
//////////////////////////////////////////////////
VS_OUTPUT vsMain(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = mul(input.pos, g_worldViewProj);
    output.uv1 = input.uv1;
    output.uv2 = input.uv2;
    return output;
}

//////////////////////////////////////////////////
/// pixel shader
//////////////////////////////////////////////////
float4 psMain_0(VS_OUTPUT input) : COLOR0
{ 
    float4 cr0 = tex2D(g_samplers[0], input.uv1);
    float4 crDiffuse = tex2D(samplerDiffuse, input.uv2);
    return cr0 * crDiffuse;
}

float4 psMain_n(VS_OUTPUT input, uniform int NumTextures) : COLOR0
{ 
    float4 crBlend = tex2D(samplerBlend, input.uv2);
    float weights[4] = (float[4]) crBlend;
    float4 color = (float4) 0;
    for(int i = 0; i < NumTextures; ++i)
    {
        color += tex2D(g_samplers[i], input.uv1) * weights[i];
    }

    float4 crDiffuse = tex2D(samplerDiffuse, input.uv2);

    color *= crDiffuse;
    color.a = 1.0;

    return color;
}

int CurNumTexture = 0;
PixelShader psArray[4] = {
    compile ps_2_0 psMain_0(), 
    compile ps_2_0 psMain_n(2),
    compile ps_2_0 psMain_n(3),
    compile ps_2_0 psMain_n(4)
};
//////////////////////////////////////////////////
///
//////////////////////////////////////////////////
struct VS_OUTPUT_DLIGHT
{
    float4 pos : POSITION;
    float4 uv : TEXCOORD0;
    float4 normal : TEXCOORD1;
    float4 eyeNml : TEXCOORD2;
    float2 depth : TEXCOORD3;
};

VS_OUTPUT_DLIGHT vsMainDynamicLight(VS_INPUT input)
{
    VS_OUTPUT_DLIGHT output;
    output.pos = mul(input.pos, g_worldViewProj);
    output.uv.xy = input.uv1;
    output.uv.zw = input.uv2;

    float4 wPos = mul(input.pos, g_world);
    float3 wNormal = mul(input.nml, g_world);
    output.normal = float4(normalize(wNormal), 0.0f);
    output.eyeNml = float4(normalize(g_cameraPosition - wPos.xyz), 0.0f);
    output.depth.xy = output.pos.zw;
    return output;
}

float4 psMainDynamicLight_0(
    float4 uv : TEXCOORD0,
    float3 normal : TEXCOORD1,
    float3 eyeNml : TEXCOORD2,
    float2 depth  : TEXCOORD3 ) : COLOR0
{
    float4 cr0 = tex2D(g_samplers[0], uv.xy);
    float4 crDiffuse;
    crDiffuse.rgb = Light(normalize(eyeNml), normalize(normal));
    crDiffuse.a = 1.0f;
    return cr0 * crDiffuse;
}

//////////////////////////////////////////////////
///
//////////////////////////////////////////////////
struct VS_OUTPUT_SHADOWMAP
{
    float4 pos   : POSITION;
    float2 depth : TEXCOORD0;
};

VS_OUTPUT_SHADOWMAP vsShadowMap(VS_INPUT input)
{
    VS_OUTPUT_SHADOWMAP output = (VS_OUTPUT_SHADOWMAP) 0;
    float4 oPos = mul(input.pos, g_worldViewProj);
    output.pos = oPos;
    output.depth.xy = oPos.zw;
    return output;
}

float4 psShadowMap(float4 depth : TEXCOORD0) : COLOR0
{
    return (float4) (depth.x / depth.y);
}

//////////////////////////////////////////////////
///
//////////////////////////////////////////////////

technique shadowmap
{
    pass p0
    {
        VertexShader = compile vs_2_0 vsShadowMap();
        PixelShader = compile ps_2_0 psShadowMap();
    }
}

technique render_scene
{
    pass p0
    {
        VertexShader = compile vs_2_0 vsMain();
        PixelShader = psArray[CurNumTexture];
    }
}
