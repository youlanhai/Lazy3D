
float4x4 g_worldViewProjection;
texture g_texture;

sampler fontSampler = sampler_state
{
    Texture = <g_texture>;
    MipFilter = Point;
    MinFilter = Point;
    MagFilter = Point;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float4 diffuse  : COLOR0;
    float2 uv    : TEXCOORD0;
};

VS_OUTPUT vsMain(float4 pos : POSITION,
                float4 color : COLOR0,
                float2 tex : TEXCOORD0)
{
    VS_OUTPUT output;
    output.position = mul(pos, g_worldViewProjection);
    output.diffuse = color;
    output.uv = tex;
    
    return output;
}

float4 psMain(VS_OUTPUT input) : COLOR0
{
    float4 alpha = tex2D(fontSampler, input.uv);
    float4 color;
    color.rgb = input.diffuse.rgb;
    color.a = input.diffuse.a * alpha.a;
    return color;
}

technique tech_default
{
    pass p0
    {
        VertexShader = compile vs_2_0 vsMain();
        PixelShader = compile ps_2_0 psMain();
    }
}
