
float4x4 g_worldViewProj;
texture g_texture;

sampler fontSampler = 
sampler_state
{
    //Texture = <g_texture>;
    MipFilter = Point;
    MinFilter = Point;
    MagFilter = Point;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float4 diffuse  : COLOR0;
    float2 texUV    : TEXCOORD0;
};

VS_OUTPUT renderVS(float4 pos : POSITION, float4 color : COLOR0, float2 tex : TEXCOORD0)
{
    VS_OUTPUT output;
    output.position = mul(pos, g_worldViewProj);
    output.diffuse = color;
    output.texUV = tex;
    
    return output;
}

float4 renderPS(VS_OUTPUT input) : COLOR0
{
    float4 alpha = tex2D(fontSampler, input.texUV);
    float4 color;
    color.rgb = input.diffuse.rgb;
    color.a = input.diffuse.a * alpha.a;
    return color;
}

technique fontTech
{
    pass p0
    {
        ZEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
        
        VertexShader = compile vs_2_0 renderVS();
        PixelShader = compile ps_2_0 renderPS();
    }
}
