

float3 g_lightDir : LIGHTDIRECTION;
float3 g_lightPos : LIGHTPOSITION;

float4 MaterialAmbient : MATERIALAMBIENT;
float4 MaterialDiffuse : MATERIALDIFFUSE;
float4 MaterialSpecular : MATERIALSPECULAR;


#define SHADOW_EPSILON          0.00005f
#define SHADOW_TEXTURE_SIZE     512

float3   g_cameraPosition : CAMERAPOSITION;
float4x4 g_shadowMapMatrix : SHADOWMAPMATRIX;

texture g_textureDiffuse;
sampler samplerDiffuse = sampler_state
{
    Texture = <g_textureDiffuse>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU  = WRAP;
    AddressV  = WRAP;
};


float3 LightDiffuse(float3 Normal, float3 lightDir)
{
    float CosTheta = max(0.0f, dot(Normal, lightDir)) * 2.0f;
    return MaterialDiffuse.rgb * CosTheta;
}

float3 LightSpecularPhone(float3 EyeNormal, float3 Normal, float3 lightDir)
{
	float3 specular = {0.0f, 0.0f, 0.0f};
	if(dot(lightDir, Normal) > 0.0f)
	{
		float3 reflectNormal = normalize(reflect(-lightDir, Normal));
		float cosTheta = max(0.0f, dot(reflectNormal, EyeNormal));
		float theta = pow(cosTheta, MaterialSpecular.a);
		specular = MaterialSpecular.rgb * theta;
	}
	return specular;
}

float3 LightSpecularBiPhone(float3 EyeNormal, float3 Normal, float3 lightDir)
{
	float3 specular = {0.0f, 0.0f, 0.0f};
	if(dot(lightDir, Normal) > 0.0f)
	{
		float3 halfNormal = normalize(lightDir + EyeNormal);
		float cosTheta = max(0.0f, dot(Normal, halfNormal));
		float theta = pow(cosTheta, MaterialSpecular.a);
		specular = MaterialSpecular.rgb * theta;
	}
	return specular;
}


float3 Light(float3 EyeNormal, float3 Normal)
{
	return MaterialAmbient.rgb + \
		LightDiffuse(Normal, g_lightDir) + \
		LightSpecularBiPhone(EyeNormal, Normal, g_lightDir);
}


float ifInShadow(float z, float2 ShadowTexCoord)
{
    return step(z, tex2D( samplerDiffuse, ShadowTexCoord ) + SHADOW_EPSILON);
}

float3 LightShadowMap(
    float4 vPos,
    float3 vNormal,
    float4 vPosInLight)
{
    float3 vDstPos = vPosInLight.xyz / vPosInLight.w; //透视除法

    //将纹理坐标从渲染目标空间转换到纹理空间
    float2 ShadowTexCoord = 0.5f * vDstPos.xy + float2( 0.5f, 0.5f );
    ShadowTexCoord.y = 1.0f - ShadowTexCoord.y;

    if(tex2D( samplerDiffuse, ShadowTexCoord ).r + SHADOW_EPSILON < vDstPos.z)
		return MaterialAmbient.rgb;

	float3 Normal = normalize(vNormal);
	float3 EyeNormal = normalize(g_cameraPosition - vPos.xyz);
	float3 LightDir = normalize(g_lightPos - vPos.xyz);

	return MaterialAmbient.rgb + \
		LightDiffuse(Normal, LightDir) + \
		LightSpecularBiPhone(EyeNormal, Normal, LightDir);
}
