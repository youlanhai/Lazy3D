#pragma once

#include "FontTextrue.h"

namespace Lazy
{
    ///文字信息。用于渲染一个文字。
    struct WordInfo
    {
        //纹理坐标
        float x1, y1;
        float x2, y2;

        //排版偏移坐标
        int dx, dy;
        int dw, dh;

        //排版步进尺寸
        int ax, ay;

        //文字纹理
        TexturePtr texture;
    };

    //////////////////////////////////////////////////////////////////////////

    ///存储一个字符的图像纹理信息
    struct CharGlyphData
    {
        short advanceX;//本字符在文本排版中的步进宽度。
        short advanceY;//本字符在文本排版中的步进高度。

        //字符的步进宽度和步进高度构成了外围Rect，像素数据Bitmap构成了内围Rect，
        //外围Rect和内围Rect之间的空隙就是左边距，右边距，上边距，下边距。
        //这里只需要记录左边距和上边距即可。
        short leftMargin;
        short topMargin;
        short bitmapWidth;//本字符像素数据的宽度。
        short bitmapHeight;//本字符像素数据的高度。

        //纹理坐标
        short texU;
        short texV;

        //文字纹理
        TexturePtr texture;

        //是否合法
        bool valid;
    };


    //////////////////////////////////////////////////////////////////////////
    ///字体
    //////////////////////////////////////////////////////////////////////////
    class Font : public IBase
    {
    public:
        typedef std::map<wchar_t, CharGlyphData> GlyphMap;

        Font(void *pFace, int height);
        ~Font(void);

        bool getWord(wchar_t ch, WordInfo & word);

        CharGlyphData* getGlyph(wchar_t ch);


        bool fillWordToTexture(dx::Texture *pDestTex, int dx, int dy, wchar_t ch, D3DXCOLOR color, int & ax, int & ay);
        bool fillTextToTexture(dx::Texture *pDestTex, int dx, int dy, const std::wstring & text, DWORD color);

        int getCharWidth(wchar_t ch);

        int getHeight() const { return m_height; }

    private:

        CharGlyphData* createGlyph(wchar_t ch);

        Font(const Font &);
        const Font & operator=(const Font &);
    private:
        void            *m_pFace;
        int             m_height;
        FontTextureMgr  m_texMgr;
        GlyphMap        m_glyphMap;
    };

    typedef RefPtr<Font> FontPtr;

    //////////////////////////////////////////////////////////////////////////
    ///字体管理器
    //////////////////////////////////////////////////////////////////////////
    class FontMgr
    {
    public:
        typedef std::map<tstring, void*> FaceMap;
        typedef std::map<tstring, FontPtr> FontMap;

        /** 由于字体文件名可能过长，使用key来代替文件名会更方便。*/
        bool registerFontFile(const tstring & fontFile, const tstring & key);

        ///获取字体。
        FontPtr getFont(const tstring & fontName);

        /** 初始化字体 */
        bool init();

        /** 释放资源 */
        void fini();

        /** 唯一实例 */
        static FontMgr * instance();

    private:
        FontMgr();
        ~FontMgr();
        FontMgr(const FontMgr &);
        const FontMgr & operator = (const FontMgr &);

        void* getFace(const tstring & faceName);

        void* createFace(const tstring & fontFile);

    private:
        bool    m_valid;
        FontMap m_fontMap;
        FaceMap m_faceMap;
    };

}//namespace Lazy