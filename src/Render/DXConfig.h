#pragma once

#include <Windows.h>
#include <d3dx9.h>

///dx定义
namespace dx
{
    typedef IDirect3D9          D3D;
    typedef IDirect3DDevice9    Device;
    typedef ID3DXEffect         Effect;
    typedef IDirect3DTexture9   Texture;
    typedef IDirect3DBaseTexture9 BaseTexture;
    typedef ID3DXFont           Font;
    typedef ID3DXMesh           Mesh;
    typedef ID3DXAnimationController AnimController;
    typedef IDirect3DPixelShader9   PixelShader;
    typedef IDirect3DVertexShader9  VertexShader;
    typedef D3DMATERIAL9        Material;

    typedef D3DRENDERSTATETYPE  RSType;
    typedef D3DPRIMITIVETYPE    PTType;
    typedef D3DTRANSFORMSTATETYPE TSType;
    
}

#define SAFE_ADDREF_COM(p)  if(p){ p->AddRef(); }
#define SAFE_DELREF_COM(p) if(p){p->Release(); }
