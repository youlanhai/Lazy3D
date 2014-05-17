
float4x4 g_worldViewProjection;
texture g_texture;

sampler textureSampler = 
sampler_state
{
    Texture = <g_texture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

struct VS_OUTPUT
{
    float4 position;
    float4 color;
    float2 uv;
};

VS_OUTPUT vsMain(float4 vPos : POSITION, 
                 float4 Diffuse : COLOR0,
                 float2 vTexCoord0 : TEXCOORD0)
{
    VS_OUTPUT output;
    
    output.position = mul(vPos, g_worldViewProjection);
    output.color = Diffuse;
    output.uv = vTexCoord0; 
    return output;    
}


float4 RenderScenePS( VS_OUTPUT input) : COLOR0
{ 
    return input.color * tex2D(textureSampler, input.uv);
}

technique
{
    pass P0
    {
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
