
#include "light.fx"

float4x4    mViewProj : VIEWPROJECTION;

// Matrix Pallette
static const int MAX_MATRICES = 32;
float4x4    mWorldMatrixArray[MAX_MATRICES] ;//: WORLDMATRIXARRAY;

float3 CameraPosition : CAMERAPOSITION;

///////////////////////////////////////////////////////
struct VS_INPUT
{
    float4  Pos             : POSITION;
    float4  BlendWeights    : BLENDWEIGHT;
    float4  BlendIndices    : BLENDINDICES;
    float3  Normal          : NORMAL;
    float3  Tex0            : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4  Pos     : POSITION;
    //float4  Diffuse : COLOR;
    float4  EyeNormal: COLOR0;
    float4  Normal  : COLOR1;
    float2  Tex0    : TEXCOORD0;
};

texture g_texture;

sampler textureSampler = sampler_state
{
    Texture = <g_texture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};


VS_OUTPUT VShade(VS_INPUT i, uniform int NumBones)
{
    VS_OUTPUT   o;
    float3      Pos = 0.0f;
    float3      Normal = 0.0f;    
    float       LastWeight = 0.0f;
     
    // cast the vectors to arrays for use in the for loop below
    float BlendWeightsArray[4] = (float[4])i.BlendWeights;
    int   IndexArray[4]        = {
        (int)i.BlendIndices.x, (int)i.BlendIndices.y,
        (int)i.BlendIndices.z, (int)i.BlendIndices.w};
    
    // calculate the pos/normal using the "normal" weights 
    //        and accumulate the weights to calculate the last weight
    for (int iBone = 0; iBone < NumBones-1; iBone++)
    {
        LastWeight = LastWeight + BlendWeightsArray[iBone];
        
        Pos += mul(i.Pos, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
        Normal += mul(i.Normal, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
    }
    LastWeight = 1.0f - LastWeight; 

    // Now that we have the calculated weight, add in the final influence
    Pos += (mul(i.Pos, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight);
    Normal += (mul(i.Normal, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight); 
    
    // transform position from world space into view and then projection space
    o.Pos = mul(float4(Pos.xyz, 1.0f), mViewProj);

    // Shade (Ambient + etc.)
    //o.Diffuse.xyz = Light(Pos, Normal);
    //o.Diffuse.w = 1.0f;

    o.EyeNormal = float4(normalize(CameraPosition - Pos), 0.0f);
    o.Normal = float4(normalize(Normal), 0.0f);

    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

    return o;
}

int CurNumBones = 2;
VertexShader vsArray[4] = {
    compile vs_2_0 VShade(1), 
    compile vs_2_0 VShade(2),
    compile vs_2_0 VShade(3),
    compile vs_2_0 VShade(4)
};


float4 PSShade(
    float4  Diffuse : COLOR,
    float2  Tex0    : TEXCOORD0) : COLOR
{
    return tex2D(textureSampler, Tex0) * Diffuse;
}

float4 PSLight(
    float4  EyeNormal : COLOR0,
    float4  Normal  : COLOR1,
    float2  Tex0    : TEXCOORD0) : COLOR
{
    float4 Diffuse;
    Diffuse.rgb = Light(normalize(EyeNormal.xyz), normalize(Normal.xyz));
    Diffuse.a = 1.0f;
    //return Diffuse;
    return tex2D(textureSampler, Tex0) * Diffuse;
}

//////////////////////////////////////
// Techniques specs follow
//////////////////////////////////////
technique t0
{
    pass p0
    {
        VertexShader = (vsArray[CurNumBones]);
        //PixelShader = compile ps_2_0 PSShade();
        PixelShader = compile ps_2_0 PSLight();
    }
}

