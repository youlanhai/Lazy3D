

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

//////////////////////////////////////////////////
///
//////////////////////////////////////////////////
