
float4x4 g_worldViewProj;

texture g_texture0;
texture g_texture1;
texture g_texture2;
texture g_texture3;
texture g_textureBlend;
texture g_textureDiffuse;

sampler sampler0 = sampler_state
{
    Texture = <g_texture0>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler sampler1 = sampler_state
{
    Texture = <g_texture1>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler sampler2 = sampler_state
{
    Texture = <g_texture2>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler sampler3 = sampler_state
{
    Texture = <g_texture3>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler samplerDiffuse = sampler_state
{
    Texture = <g_textureDiffuse>;
    MipFilter = NONE;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU  = CLAMP;
    AddressV  = CLAMP;
};

sampler samplerBlend = sampler_state
{
    Texture = <g_textureBlend>;
    MipFilter = NONE;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU  = CLAMP;
    AddressV  = CLAMP;
};


struct VS_OUTPUT
{
    float4 position : POSITION;
    float2 uv1    : TEXCOORD0;
    float2 uv2    : TEXCOORD1;
};

//////////////////////////////////////////////////
/// vertex shader
//////////////////////////////////////////////////
VS_OUTPUT vsMain(
    float4 pos : POSITION,
    float2 uv1 : TEXCOORD0,
    float2 uv2 : TEXCOORD1)
{
    VS_OUTPUT output;
    output.position = mul(pos, g_worldViewProj);
    output.uv1 = uv1;
    output.uv2 = uv2;
    
    return output;
}

//////////////////////////////////////////////////
/// pixel shader
//////////////////////////////////////////////////
float4 psMain_0(VS_OUTPUT In) : COLOR0
{ 
    float4 cr0 = tex2D(sampler0, In.uv1);
    float4 crDiffuse = tex2D(samplerDiffuse, In.uv2);

    return cr0 * crDiffuse;
}

float4 psMain_1(VS_OUTPUT In) : COLOR0
{ 
    float4 cr0 = tex2D(sampler0, In.uv1);
    float4 cr1 = tex2D(sampler1, In.uv1);

    float4 crBlend = tex2D(samplerBlend, In.uv2);
    float4 crDiffuse = tex2D(samplerDiffuse, In.uv2);

    return (cr0 * crBlend.r + cr1 * crBlend.g) * crDiffuse;
}

float4 psMain_2(VS_OUTPUT In) : COLOR0
{ 
    float4 cr0 = tex2D(sampler0, In.uv1);
    float4 cr1 = tex2D(sampler1, In.uv1);
    float4 cr2 = tex2D(sampler2, In.uv1);

    float4 crBlend = tex2D(samplerBlend, In.uv2);
    float4 crDiffuse = tex2D(samplerDiffuse, In.uv2);

    return (cr0 * crBlend.r + \
        cr1 * crBlend.g + \
        cr2 * crBlend.b) * crDiffuse;
}

float4 psMain_3(VS_OUTPUT In) : COLOR0
{ 
    float4 cr0 = tex2D(sampler0, In.uv1);
    float4 cr1 = tex2D(sampler1, In.uv1);
    float4 cr2 = tex2D(sampler2, In.uv1);
    float4 cr3 = tex2D(sampler3, In.uv1);

    float4 crBlend = tex2D(samplerBlend, In.uv2);
    float4 crDiffuse = tex2D(samplerDiffuse, In.uv2);

    return (cr0 * crBlend.r + \
        cr1 * crBlend.g + \
        cr2 * crBlend.b + \
        cr3 * crBlend.a) * crDiffuse;
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
