
#include "light.fx"

float4x4 g_world : WORLD;
float4x4 g_worldViewProj : WORLDVIEWPROJECTION;

texture g_texture0;
texture g_texture1;
texture g_texture2;
texture g_texture3;
texture g_textureBlend;

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

sampler samplerBlend   = TERRAIN_SAMPLER(g_textureBlend);

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
    float4 pos  : POSITION;
    float4 uv   : TEXCOORD0;
    float4 vPos : TEXCOORD1;
    float3 vNml : TEXCOORD2;
    float4 vPosInLight : TEXCOORD3;
};

//////////////////////////////////////////////////
///
//////////////////////////////////////////////////
VS_OUTPUT vsMain(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = mul(input.pos, g_worldViewProj);
    output.uv = float4(input.uv1.xy, input.uv2.xy);
    output.vPos = mul(input.pos, g_world);
    output.vNml = normalize(mul(input.nml, (float3x3)g_world));
    output.vPosInLight = mul(output.vPos, g_shadowMapMatrix);
    return output;
}

float4 psMain_0(VS_OUTPUT input) : COLOR0
{ 
    float4 color = tex2D(g_samplers[0], input.uv.xy);
    float4 diffuse = LightShadowMap(input.vPos, input.vNml, input.vPosInLight);
    diffuse.a = 1.0f;
    return color * diffuse;
}

float4 psMain_n(VS_OUTPUT input,
    uniform int NumTextures) : COLOR0
{
    float4 diffuse = LightShadowMap(input.vPos, input.vNml, input.vPosInLight);
    diffuse.a = 1.0f;

    float4 crBlend = tex2D(samplerBlend, input.uv.zw);
    float  weights[4] = (float[4]) crBlend;
    float4 color = (float4) 0;
    for(int i = 0; i < NumTextures; ++i)
    {
        color += tex2D(g_samplers[i], input.uv.xy) * weights[i];
    }

    color.a = 1.0;
    return color * diffuse;
}

int CurNumTexture = 0;
PixelShader psArray[4] = {
    compile ps_3_0 psMain_0(), 
    compile ps_3_0 psMain_n(2),
    compile ps_3_0 psMain_n(3),
    compile ps_3_0 psMain_n(4)
};

//////////////////////////////////////////////////
///
//////////////////////////////////////////////////
void vsShadowMap(float4 position : POSITION,
    out float4 oPos  : POSITION,
    out float2 depth : TEXCOORD0)
{
    oPos = mul(position, g_world);
    oPos = mul(oPos, g_shadowMapMatrix);
    depth.xy = oPos.zw;
}

void psShadowMap(float2 depth : TEXCOORD0,
    out float4 color : COLOR0)
{
    color = depth.x / depth.y;
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
