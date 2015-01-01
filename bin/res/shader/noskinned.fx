
float4x4    g_world : WORLD;
float4x4    g_worldViewProj : WORLDVIEWPROJECTION;

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

//////////////////////////////////////////////////
/// vertex shader
//////////////////////////////////////////////////

struct VS_INPUT
{
    float4  pos     : POSITION;
    float3  nml     : NORMAL;
    float2  uv      : TEXCOORD0;

};

VS_OUTPUT vsPixelLight(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.pos = mul(input.pos, g_worldViewProj);
    output.uv.xy = input.uv;
    output.vPos = mul(input.pos, g_world);
    output.vNml = normalize(mul(input.nml, (float3x3)g_world));

#ifdef USE_SHADOW_MAP
    output.vPosInLight = mul(output.vPos, g_shadowMapMatrix);
#endif

    return output;
}


//////////////////////////////////////
//
//////////////////////////////////////

#ifdef USE_SHADOW_MAP

void vsShadowMap(float4 position : POSITION,
    out float4 oPos  : POSITION,
    out float2 depth : TEXCOORD0)
{
    oPos = mul(position, g_world);
    oPos = mul(oPos, g_shadowMapMatrix);
    depth.xy = oPos.zw;
}

technique tech_shadowmap
{
    pass p0
    {
        VertexShader = compile vs_2_0 vsShadowMap();
        PixelShader  = compile ps_2_0 psShadowMap();
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
        VertexShader = compile vs_2_0 vsPixelLight();
        PixelShader  = compile ps_2_0 psPixelLight();
    }
}
