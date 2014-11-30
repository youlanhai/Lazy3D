

float3 lightDir = normalize(float3(0.0f, -1.0f, -1.0f));    //light Direction 
float4 lightDiffuse = {0.6f, 0.6f, 0.6f, 1.0f}; // Light Diffuse
float4 MaterialAmbient /*: MATERIALAMBIENT*/ = {0.1f, 0.1f, 0.1f, 1.0f};
float4 MaterialDiffuse /*: MATERIALDIFFUSE*/ = {0.8f, 0.8f, 0.8f, 1.0f};
float4 MaterialSpecular /*: MATERIALSPECULAR*/ = {1.0f, 1.0f, 1.0f, 2.0f};

float3 CameraPosition : CAMERAPOSITION = {0.0f, 0.0f, 0.0f};

float3 LightDiffuse(float3 Normal)
{
    float CosTheta = max(0.0f, dot(Normal, -lightDir));
    return MaterialDiffuse.rgb * CosTheta;
}

float3 LightSpecular(float3 Position, float3 Normal)
{
	float3 eyeNormal = normalize(CameraPosition - Position);
	float3 specular = {0.0f, 0.0f, 0.0f};
	if(dot(-lightDir, eyeNormal) > 0.0f)
	{
		float3 reflectNormal = normalize(reflect(lightDir, Normal));
		float cosTheta = max(0.0f, dot(reflectNormal, eyeNormal));
		float theta = pow(cosTheta, MaterialSpecular.a);
		specular = MaterialSpecular.rgb * theta;
	}
	return specular;
}


float3 Light(float3 Position, float3 Normal)
{
	return MaterialAmbient.rgb + LightDiffuse(Normal) + LightSpecular(Position, Normal);
}

