//--------------------------------------------------------------------------------------
// File: BasicHLSL.fx
//
// The effect file for the BasicHLSL sample.  
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

float4x4 g_worldViewProjection;    // World * View * Projection matrix
texture g_texture;              // Color texture for mesh


//--------------------------------------------------------------------------------------
// Texture samplers
//--------------------------------------------------------------------------------------
sampler UITextureSampler = 
sampler_state
{
    Texture = <g_texture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};


//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Position   : POSITION;   // vertex position 
    float4 Diffuse    : COLOR0;     // vertex diffuse color (note that COLOR0 is clamped from 0..1)
    float2 TextureUV  : TEXCOORD0;  // vertex texture coords 
};


//--------------------------------------------------------------------------------------
// This shader computes standard transform and lighting
//--------------------------------------------------------------------------------------
VS_OUTPUT RenderSceneVS( float4 vPos : POSITION, 
                         float4 Diffuse : COLOR0,
                         float2 vTexCoord0 : TEXCOORD0,
                         uniform bool bTexture)
{
    VS_OUTPUT Output;
    
    Output.Position = mul(vPos, g_worldViewProjection);
    Output.Diffuse = Diffuse;
    
    // Just copy the texture coordinate through
    if( bTexture ) 
        Output.TextureUV = vTexCoord0; 
    else
        Output.TextureUV = 0; 
    
    return Output;    
}


//--------------------------------------------------------------------------------------
// Pixel shader output structure
//--------------------------------------------------------------------------------------
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color    
};


//--------------------------------------------------------------------------------------
// This shader outputs the pixel's color by modulating the texture's
//       color with diffuse material color
//--------------------------------------------------------------------------------------
PS_OUTPUT RenderScenePS( VS_OUTPUT In,
                         uniform bool bTexture ) 
{ 
    PS_OUTPUT Output;

    if( bTexture ) Output.RGBColor = tex2D(UITextureSampler, In.TextureUV) * In.Diffuse;
    
    else Output.RGBColor = In.Diffuse;

    return Output;
}


//--------------------------------------------------------------------------------------
// Renders scene to render target
//--------------------------------------------------------------------------------------
technique RenderWithTexture
{
    pass P0
    {
        ZENABLE = FALSE;
        ALPHABLENDENABLE = TRUE;
        SRCBLEND = SRCALPHA;
        DESTBLEND = INVSRCALPHA;
        
        VertexShader = compile vs_2_0 RenderSceneVS( true );
        PixelShader  = compile ps_2_0 RenderScenePS( true ); // trivial pixel shader (could use FF instead if desired)
    }
}

technique RenderNoTexture
{
    pass P0
    {
        ZENABLE = FALSE;
        ALPHABLENDENABLE = TRUE;
        SRCBLEND = SRCALPHA;
        DESTBLEND = INVSRCALPHA;
        
        VertexShader = compile vs_2_0 RenderSceneVS( false );
        PixelShader  = compile ps_2_0 RenderScenePS( false ); // trivial pixel shader (could use FF instead if desired)
    }
}

