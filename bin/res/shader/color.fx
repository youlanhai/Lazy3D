
float4x4 g_worldViewProjection;

struct VS_OUTPUT
{
    float4 position     : POSITION;
    float4 color        : COLOR0;
};

VS_OUTPUT vsMain(   float4 vPos : POSITION, 
                    float4 Diffuse : COLOR0)
{
    VS_OUTPUT output;
    
    output.position = mul(vPos, g_worldViewProjection);
    output.color = Diffuse;
    
    return output;    
}


float4 psMain( VS_OUTPUT input) : COLOR0
{
    return input.color;
}


technique tech_default
{
    pass P0
    {
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
