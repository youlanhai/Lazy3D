

float4 psShadowMap(
    float2 depth : TEXCOORD0) : COLOR0
{
    return depth.x / depth.y;
}

