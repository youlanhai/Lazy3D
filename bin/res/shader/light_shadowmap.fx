
#define USE_SHADOW_MAP

#include "light.fx"

#define SHADOW_EPSILON          0.00005f
#define SHADOW_TEXTURE_SIZE     512


float4x4 g_shadowMapMatrix : SHADOWMAPMATRIX;

texture g_textureDiffuse;
sampler samplerDiffuse = sampler_state
{
    Texture = <g_textureDiffuse>;
    MipFilter = Point;
    MinFilter = Point;
    MagFilter = Point;
    AddressU  = Clamp;
    AddressV  = Clamp;
};


float4 LightShadowMap(
    float3 vPos,
    float3 vNormal,
    float4 vPosInLight)
{
    float3 vDstPos = vPosInLight.xyz / vPosInLight.w; //透视除法

    //将纹理坐标从渲染目标空间转换到纹理空间
    float2 ShadowTexCoord = 0.5f * vDstPos.xy + float2( 0.5f, 0.5f );
    ShadowTexCoord.y = 1.0f - ShadowTexCoord.y;

    // Determine the lerp amounts           
    float2 lerps = frac( SHADOW_TEXTURE_SIZE * ShadowTexCoord );

	float samplediv = 1.0f / SHADOW_TEXTURE_SIZE;

    //read in bilerp stamp, doing the shadow checks
    float4 sourcevals;
    sourcevals.x = step(vDstPos.z, SHADOW_EPSILON + tex2D( samplerDiffuse, ShadowTexCoord ).r);
    sourcevals.y = step(vDstPos.z, SHADOW_EPSILON + tex2D( samplerDiffuse, ShadowTexCoord + float2(samplediv, 0) ).r);
    sourcevals.z = step(vDstPos.z, SHADOW_EPSILON + tex2D( samplerDiffuse, ShadowTexCoord + float2(0, samplediv) ).r);
    sourcevals.w = step(vDstPos.z, SHADOW_EPSILON + tex2D( samplerDiffuse, ShadowTexCoord + float2(samplediv, samplediv) ).r);
    
    // lerp between the shadow values to calculate our light amount
    float LightAmount = lerp( lerp( sourcevals.x, sourcevals.y, lerps.x ),
                              lerp( sourcevals.z, sourcevals.w, lerps.x ),
                              lerps.y );

	float3 Normal = normalize(vNormal);
	float3 EyeNormal = normalize(g_cameraPosition - vPos);
	float3 LightDir = normalize(g_lightPosition - vPos);

	return MaterialAmbient + \
	 	LightDiffuse(Normal, LightDir) * LightAmount +\
	 	LightSpecularBiPhone(EyeNormal, Normal, LightDir) * LightAmount;
}


//////////////////////////////////////////////////
///
//////////////////////////////////////////////////
