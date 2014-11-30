

float3 lightDir = normalize(float3(0.0f, 1.0f, 1.0f));    //light Direction 

float4 MaterialAmbient : MATERIALAMBIENT;
float4 MaterialDiffuse : MATERIALDIFFUSE;
float4 MaterialSpecular : MATERIALSPECULAR;

float3 LightDiffuse(float3 Normal)
{
    float CosTheta = max(0.0f, dot(Normal, lightDir));
    return MaterialDiffuse.rgb * CosTheta;
}

float3 LightSpecularPhone(float3 EyeNormal, float3 Normal)
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

float3 LightSpecularBiPhone(float3 EyeNormal, float3 Normal)
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
	return MaterialAmbient.rgb + LightDiffuse(Normal) + LightSpecularBiPhone(EyeNormal, Normal);
}
