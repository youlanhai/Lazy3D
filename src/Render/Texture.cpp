//Texture.cpp
#include "stdafx.h"
#include "Texture.h"
#include "RenderDevice.h"

namespace Lazy
{

    Texture::Texture(const tstring & source)
        : IResource(source)
        , m_texture(nullptr)
        , m_width(0)
        , m_height(0)
        , m_format(D3DFMT_UNKNOWN)
    {
    }

    Texture::Texture(dx::Texture * pTex)
        : IResource(_T(""))
        , m_texture(pTex)
        , m_width(0)
        , m_height(0)
        , m_format(D3DFMT_UNKNOWN)
    {
        if (m_texture)
            m_texture->AddRef();
            
        udpateInfo();
    }

    Texture::~Texture()
    {
        SAFE_RELEASE(m_texture);
    }

    void Texture::udpateInfo()
    {
        if (m_texture)
        {
            D3DSURFACE_DESC desc;
            m_texture->GetLevelDesc(0, &desc);
            m_width = desc.Width;
            m_height = desc.Height;
            m_format = desc.Format;
        }
    }

    /** 加载资源。*/
    bool Texture::load()
    {
        if (m_texture)
        {
            throw(std::runtime_error("Texture::load has been loaded!"));
        }

        tstring realPath;
        if (!getfs()->getRealPath(realPath, m_source))
        {
            LOG_ERROR(L"Texture '%s' was not found!", m_source.c_str());
            return false;
        }

        if ( FAILED(D3DXCreateTextureFromFile(
            rcDevice()->getDevice(), realPath.c_str(), &m_texture)))
        {
            m_texture = NULL;
            return false;
        }

        udpateInfo();
        return true;
    }



    dx::Texture * TextureMgr::getTexture(const std::wstring & name)
    {
        TexturePtr ptr = get(name);
        if (ptr) return ptr->getTexture();

        return nullptr;
    }
}