
float4x4 g_worldViewProj;

texture g_texture0;
texture g_texture1;
texture g_texture2;
texture g_texture3;
texture g_textureBlend;
texture g_textureDiffuse;

#define TERRAIN_SAMPLER(NAME, TEXTURE)  \
sampler NAME = sampler_state        \
{                                   \
    Texture = <TEXTURE>;            \
    MipFilter = LINEAR;             \
    MinFilter = LINEAR;             \
    MagFilter = LINEAR;             \
    AddressU  = WRAP;               \
    AddressV  = WRAP;               \
}

TERRAIN_SAMPLER(sampler0, g_texture0);
TERRAIN_SAMPLER(sampler1, g_texture1);
TERRAIN_SAMPLER(sampler2, g_texture2);
TERRAIN_SAMPLER(sampler3, g_texture3);
TERRAIN_SAMPLER(samplerDiffuse, g_textureDiffuse);
TERRAIN_SAMPLER(samplerBlend, g_textureBlend);

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
    float4 cr0 = tex2D(sampler0, input.uv1);
    float4 crDiffuse = tex2D(samplerDiffuse, input.uv2);

    float4 color = cr0 * crDiffuse;
    color.a = cr0.a;
    return color;
}

float4 psMain_1(VS_OUTPUT input) : COLOR0
{ 
    float4 cr0 = tex2D(sampler0, input.uv1);
    float4 cr1 = tex2D(sampler1, input.uv1);

    float4 crBlend = tex2D(samplerBlend, input.uv2);
    float4 crDiffuse = tex2D(samplerDiffuse, input.uv2);

    float4 color = (cr0 * crBlend.r + cr1 * crBlend.g) * crDiffuse;
    color.a = cr0.a;
    return color;
}

float4 psMain_2(VS_OUTPUT input) : COLOR0
{ 
    float4 cr0 = tex2D(sampler0, input.uv1);
    float4 cr1 = tex2D(sampler1, input.uv1);
    float4 cr2 = tex2D(sampler2, input.uv1);

    float4 crBlend = tex2D(samplerBlend, input.uv2);
    float4 crDiffuse = tex2D(samplerDiffuse, input.uv2);

    float4 color = (cr0 * crBlend.r + \
        cr1 * crBlend.g + \
        cr2 * crBlend.b) * crDiffuse;
    color.a = cr0.a;
    return color;
}

float4 psMain_3(VS_OUTPUT input) : COLOR0
{ 
    float4 cr0 = tex2D(sampler0, input.uv1);
    float4 cr1 = tex2D(sampler1, input.uv1);
    float4 cr2 = tex2D(sampler2, input.uv1);
    float4 cr3 = tex2D(sampler3, input.uv1);

    float4 crBlend = tex2D(samplerBlend, input.uv2);
    float4 crDiffuse = tex2D(samplerDiffuse, input.uv2);

    float4 color = (cr0 * crBlend.r + \
        cr1 * crBlend.g + \
        cr2 * crBlend.b + \
        cr3 * crBlend.a) * crDiffuse;
    color.a = cr0.a;
    return color;
}

technique tech_0
{
    pass p0
    {
        VertexShader = compile vs_2_0 vsMain();
        PixelShader = compile ps_2_0 psMain_0();
    }
}

technique tech_1
{
    pass p0
    {
        VertexShader = compile vs_2_0 vsMain();
        PixelShader = compile ps_2_0 psMain_1();
    }
}

technique tech_2
{
    pass p0
    {
        VertexShader = compile vs_2_0 vsMain();
        PixelShader = compile ps_2_0 psMain_2();
    }
}

technique tech_3
{
    pass p0
    {
        VertexShader = compile vs_2_0 vsMain();
        PixelShader = compile ps_2_0 psMain_3();
    }
}
