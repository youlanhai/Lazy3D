
float4x4 g_worldViewProjection;
texture g_texture;

sampler textureSampler = sampler_state
{
    Texture = <g_texture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float4 color  : COLOR0;
    float2 uv    : TEXCOORD0;
};

VS_OUTPUT vsMain(float4 pos : POSITION, float4 color : COLOR0, float2 tex : TEXCOORD0)
{
    VS_OUTPUT output;
    output.position = mul(pos, g_worldViewProjection);
    output.color = color;
    output.uv = tex;
    
    return output;
}


float4 psMain(VS_OUTPUT In) : COLOR0
{ 
    return tex2D(textureSampler, In.uv) * In.color;
}

technique
{
    pass p0
    {
        VertexShader = compile vs_2_0 vsMain();
        PixelShader = compile ps_2_0 psMain();
    }
}
