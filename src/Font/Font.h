#pragma once

#include "FontTextrue.h"

namespace Lazy
{

///文字对齐方式
namespace AlignType
{
    const uint32 Left       = 1; ///<左对齐
    const uint32 HCenter    = 2; ///<水平居中
    const uint32 Right      = 4; ///<右对齐
    const uint32 Top        = 8; ///<顶对齐
    const uint32 Bottom     = 16; ///<底对齐
    const uint32 VCenter    = 32; ///<垂直居中
    const uint32 Center     = HCenter | VCenter; ///垂直、水平都居中
    const uint32 MutiLine   = 64; ///以后要去掉
}

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
    dx::Texture *pTexture;
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
    dx::Texture * pTexture;

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

    ///渲染一个文字
    bool drawWord(int x, int y, wchar_t ch, DWORD cr, int & ax, int & ay);

    ///从(x, y)位置起，渲染一行文字。
    bool drawTextWithoutShader(int x, int y, const std::wstring & text, DWORD cr);
    bool drawTextWithShader(int x, int y, const std::wstring & text, DWORD cr);

#ifdef DISABLE_SHADER
#define drawTextByPos drawTextWithoutShader    
#else
#define drawTextByPos drawTextWithShader
#endif

    ///根据格式，渲染一组文字
    bool drawText(const std::wstring & text, const CRect & rc, DWORD style, DWORD cr);

    bool getTextSize(int & width, int & height, const std::wstring & text);
    bool getTextInfo(CSize & size, int & lines, const std::wstring & text, const CRect & rc, DWORD style);
    bool getMaxSize(int & width, int & height, const std::wstring & text, DWORD style);

    void parseTextToLines(
        std::vector<std::wstring> & textLines, 
        std::vector<int> & widthLines,
        const std::wstring & text,
        const CRect & rc,
        DWORD style);

    ///根据光标位置，查鼠标位置。
    bool getCursorPos(const std::wstring & text, const CRect & rc, DWORD style, 
        size_t cursor, int & x, int & y, int & h);

    ///根据鼠标位置，查光标位置。
    bool getCursorByPos(const std::wstring & text, const CRect & rc, DWORD style, 
        size_t & cursor, int x, int y);

    bool fillWordToTexture(LPDIRECT3DTEXTURE9 pDestTex, int dx, int dy, wchar_t ch, D3DXCOLOR color, int & ax, int & ay);
    bool fillTextToTexture(LPDIRECT3DTEXTURE9 pDestTex, int dx, int dy, const std::wstring & text, DWORD color);

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