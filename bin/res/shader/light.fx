
float3 g_lightPosition : LIGHTPOSITION;

float4 MaterialAmbient : MATERIALAMBIENT;
float4 MaterialDiffuse : MATERIALDIFFUSE;
float4 MaterialSpecular : MATERIALSPECULAR;

float3 g_cameraPosition : CAMERAPOSITION;


float4 LightDiffuse(float3 Normal, float3 lightDir)
{
    float CosTheta = max(0.0f, dot(Normal, lightDir)) * 1.2f;
    return MaterialDiffuse * CosTheta;
}

float4 LightSpecularPhone(float3 EyeNormal, float3 Normal, float3 lightDir)
{
	if(dot(lightDir, Normal) > 0.0f)
	{
		float3 reflectNormal = normalize(reflect(-lightDir, Normal));
		float theta = max(0.0f, dot(reflectNormal, EyeNormal));
		theta = pow(theta, MaterialSpecular.a);
		return MaterialSpecular * theta;
	}
	return (float4) 0;
}

float4 LightSpecularBiPhone(float3 EyeNormal, float3 Normal, float3 lightDir)
{
	if(dot(lightDir, Normal) > 0.0f)
	{
		float3 halfNormal = normalize(lightDir + EyeNormal);
		float theta = max(0.0f, dot(Normal, halfNormal));
		theta = pow(theta, MaterialSpecular.a);
		return MaterialSpecular * theta;
	}
	return (float4) 0;
}


float4 Light(float3 vPos, float3 vNormal)
{
	float3 Normal = normalize(vNormal);
	float3 EyeNormal = normalize(g_cameraPosition - vPos);
	float3 LightDir = normalize(g_lightPosition - vPos);

	return MaterialAmbient + \
		LightDiffuse(Normal, LightDir) + \
		LightSpecularBiPhone(EyeNormal, Normal, LightDir);
}
