//Texture.h
#pragma once
#include "Resource.h"
#include "ResMgr.h"

namespace Lazy
{

    ///纹理
    class Texture : public IResource
    {
    public:
        explicit Texture(const tstring & source);
        explicit Texture(dx::Texture * pTex);
        Texture(dx::Texture * pTex, D3DFORMAT fmt, uint32 width, uint32 height);

        ~Texture();

        /** 加载资源。*/
        virtual bool load() override;

        /** 获得资源类型*/
        virtual int getType(void) const override { return rt::texture; }

        dx::Texture* getTexture(void) { return m_texture; }

        uint32 getWidth() const { return m_width; }
        uint32 getHeight() const { return m_height; }
        CSize getSize() const { return CSize(m_width, m_height); }
        D3DFORMAT getFormat() const { return m_format; }

    protected:
        void udpateInfo();

        dx::Texture*    m_texture;
        D3DFORMAT       m_format;
        uint32          m_width;
        uint32          m_height;
    };

    typedef RefPtr<Texture> TexturePtr;

    class TextureMgr : public ResMgr<TextureMgr, TexturePtr>
    {
    public:
        virtual int getType() { return rt::texture; }

        dx::Texture * getTexture(const std::wstring & name);
    };
}