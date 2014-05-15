#pragma once

namespace Lazy
{

    namespace FontConfig
    {
        const int MaxTextureWidth = 512;
        const int MaxTextureHeight = 512;
        const bool PreloadAscii = true;
    }

    //////////////////////////////////////////////////////////////////////////
    //纹理结点。管理一张纹理。
    //////////////////////////////////////////////////////////////////////////
    class FontTextureNode
    {
    public:
        FontTextureNode(int w, int h, int dw, int dh);

        ~FontTextureNode();

    public:

        bool valid() const { return m_texture != NULL; }

        int maxSize() const{ return m_rows * m_cols; }

        bool empty() const { return m_index >= maxSize(); }

        int gridWidth() const { return m_dw; }
        int gridHeight() const { return m_height; }

        int width() const { return m_width; }
        int height() const { return m_height; }

        LPDIRECT3DTEXTURE9 texture(){ return m_texture; }

        bool lockOne(int & x, int & y);

    private:

        bool create();

        FontTextureNode(const FontTextureNode &);
        const FontTextureNode & operator = (const FontTextureNode &);

    private:
        int m_width;
        int m_height;
        int m_dw;
        int m_dh;
        int m_rows;
        int m_cols;
        int m_index;

        LPDIRECT3DTEXTURE9 m_texture;
    };

    typedef std::shared_ptr<FontTextureNode> FontTextureNodePtr;

    typedef std::vector<FontTextureNodePtr> FontTextureSet;

    //////////////////////////////////////////////////////////////////////////
    //纹理管理器，管理一组字体大小相同的纹理。
    //////////////////////////////////////////////////////////////////////////
    class FontTextureMgr
    {
    public:
        FontTextureMgr();

        ~FontTextureMgr();

        FontTextureNodePtr getTexNode(int fontSize);

    private:

        FontTextureNodePtr createNode(int fontSize);

    private:
        FontTextureSet m_pool;
    };
}//namespace Lazy