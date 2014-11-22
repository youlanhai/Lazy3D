
float4 lhtDir = {0.0f, 0.0f, -1.0f, 1.0f};    //light Direction 
float4 lightDiffuse = {0.6f, 0.6f, 0.6f, 1.0f}; // Light Diffuse
float4 MaterialAmbient /*: MATERIALAMBIENT*/ = {0.1f, 0.1f, 0.1f, 1.0f};
float4 MaterialDiffuse /*: MATERIALDIFFUSE*/ = {0.8f, 0.8f, 0.8f, 1.0f};

float4x4    mWorld : WORLD;
float4x4    mWorldViewProj : WORLDVIEWPROJECTION;

///////////////////////////////////////////////////////
struct VS_INPUT
{
    float4  Pos             : POSITION;
    float3  Normal          : NORMAL;
    float3  Tex0            : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4  Pos     : POSITION;
    float4  Diffuse : COLOR;
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

float3 Diffuse(float3 Normal)
{
    float CosTheta;
    
    // N.L Clamped
    CosTheta = max(0.0f, dot(Normal, lhtDir.xyz));
       
    // propogate scalar result to vector
    return (CosTheta);
}


VS_OUTPUT VShade(VS_INPUT i)
{
    VS_OUTPUT   o;
     
    // transform position from world space into view and then projection space
    o.Pos = mul(i.Pos, mWorldViewProj);

    float3      Normal = mul(i.Normal, mWorld);    
    // normalize normals
    Normal = normalize(Normal);

    // Shade (Ambient + etc.)
    o.Diffuse.xyz = MaterialAmbient.xyz + Diffuse(Normal) * MaterialDiffuse.xyz;
    o.Diffuse.w = 1.0f;

    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

    return o;
}

float4 PSShade(
    float4  Diffuse : COLOR,
    float2  Tex0    : TEXCOORD0) : COLOR
{
    return tex2D(textureSampler, Tex0) * Diffuse;
}

//////////////////////////////////////
// Techniques specs follow
//////////////////////////////////////
technique t0
{
    pass p0
    {
        VertexShader = compile vs_2_0 VShade();
        PixelShader = compile ps_2_0 PSShade();
    }
}

