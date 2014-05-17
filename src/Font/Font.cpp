#include "StdAfx.h"
#include "Font.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_ERRORS_H
#include FT_OUTLINE_H

namespace Lazy
{
    void FillPixelData(dx::Texture *pTexture, 
        const RECT& dest_rect,
        unsigned char* pPixelBuffer, 
        int nWidth,
        int nHeight)
    {
        assert(pTexture && pPixelBuffer && "FillPixelData");

        //Lock顶层贴图。
        UINT uiLevel = 0;
        D3DLOCKED_RECT locked_rect;
        if (pTexture->LockRect(uiLevel, &locked_rect, &dest_rect, D3DLOCK_DISCARD) != D3D_OK)
        {
            return;
        }

        int nIndex = 0;
        for (int y = 0; y < nHeight; ++y)
        {
            unsigned char* pDestBuffer = ((unsigned char*)locked_rect.pBits) + locked_rect.Pitch * y;
            for (int x = 0; x < nWidth; ++x)
            {
                pDestBuffer[x] = pPixelBuffer[nIndex];
                ++nIndex;
            }
        }
        pTexture->UnlockRect(uiLevel);
    }

    void DrawGlyphToTexture(CharGlyphData & stData, unsigned char* pPixelBuffer)
    {
        RECT dest_rect;
        dest_rect.left = stData.texU ;
        dest_rect.right = dest_rect.left + stData.bitmapWidth;
        dest_rect.top = stData.texV;
        dest_rect.bottom = dest_rect.top + stData.bitmapHeight;
        FillPixelData(stData.texture->getTexture(), dest_rect, pPixelBuffer, stData.bitmapWidth, stData.bitmapHeight);
    }

    //////////////////////////////////////////////////////////////////////////

    Font::Font(void *pFace, int height)
        : m_pFace(pFace)
        , m_height(height)
    {

        LOG_INFO(_T("create font: size=%d"), height);

        if (FontConfig::PreloadAscii)
        {
            for (wchar_t i=0; i<128; ++i)
            {
                createGlyph(i);
            }
        }
    }


    Font::~Font(void)
    {
    }

    bool Font::getWord(wchar_t ch, WordInfo & word)
    {
        const CharGlyphData * pGlyph = getGlyph(ch);
        if (!pGlyph->valid)  return false;

        word.x1 = float(pGlyph->texU) / FontConfig::MaxTextureWidth;
        word.y1 = float(pGlyph->texV) / FontConfig::MaxTextureHeight;
        word.x2 = float(pGlyph->texU + pGlyph->bitmapWidth) / FontConfig::MaxTextureWidth;
        word.y2 = float(pGlyph->texV + pGlyph->bitmapHeight) / FontConfig::MaxTextureHeight;

        word.ax = pGlyph->advanceX;
        word.ay = pGlyph->advanceY;
        word.dx = pGlyph->leftMargin;
        word.dy = pGlyph->topMargin;
        word.dw = pGlyph->bitmapWidth;
        word.dh = pGlyph->bitmapHeight;
        word.texture = pGlyph->texture;

        return true;
    }

    CharGlyphData* Font::getGlyph(wchar_t ch)
    {
        GlyphMap::iterator it = m_glyphMap.find(ch);
        if (it != m_glyphMap.end())
            return &(it->second);

        return createGlyph(ch);
    }

    CharGlyphData* Font::createGlyph(wchar_t ch)
    {
        //debugMessage(L"Font::createGlyph %c", ch);

#ifdef _DEBUG
        GlyphMap::iterator it = m_glyphMap.find(ch);
        assert(it == m_glyphMap.end() && "the char has been created!");
#endif

        CharGlyphData * pGLData = &(m_glyphMap[ch]);
        ZeroMemory(pGLData, sizeof(CharGlyphData));

        FT_Face pFace = (FT_Face)m_pFace;
        FT_Set_Pixel_Sizes(pFace, m_height, m_height);

        //启用抗锯齿。
        FT_Int32 nLoadFlags = FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_NORMAL;
        FT_Error nResult = FT_Load_Char(pFace, ch, nLoadFlags);
        if (nResult)
        {
            //字体文件中没有这个字！
            //用一个错误字符表示，比如星号。
            //未完待续。
            LOG_ERROR(_T("Font::createGlyph : the char '%c' doesn't found!"), ch);
            return pGLData;
        }
        
        const FT_GlyphSlot& theGlyph = pFace->glyph;

        //本类只生成横向排版字形，不处理竖向排版。在非等宽字体下，X方向上的步进是各不相同的。
        pGLData->advanceX = (short)(theGlyph->advance.x >> 6);

        //本类只生成横向排版字形，所以Y方向上的步进是固定的；如果是竖向排版，则X方向上的步进是固定的。
        pGLData->advanceY = m_height;
        pGLData->leftMargin = theGlyph->bitmap_left;
        pGLData->topMargin = m_height - theGlyph->bitmap_top;
        pGLData->bitmapWidth = theGlyph->bitmap.width;
        pGLData->bitmapHeight = theGlyph->bitmap.rows;


        const FT_Bitmap& bitmap = theGlyph->bitmap;
        //一个byte表示一个像素，存储bitmap灰度图。
        unsigned char* pPixelBuffer = 0;
        bool bShouldDeletePixelBuffer = false;

        if(bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
        {
            pPixelBuffer = (unsigned char*)bitmap.buffer;
            bShouldDeletePixelBuffer = false;
        }
        else if(bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
        {
            int width = bitmap.width;
            int height = bitmap.rows;
            pPixelBuffer = new unsigned char[width*height];
            bShouldDeletePixelBuffer = true;
            int nIndex = 0;
            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    if (bitmap.buffer[y*width + x/8] & (0x80 >> (x & 7)))
                    {
                        pPixelBuffer[nIndex] = 0xFF;
                    }
                    else
                    {
                        pPixelBuffer[nIndex] = 0x00;
                    }
                    ++nIndex;
                }
            }
        }
        else
        {
            LOG_ERROR(_T("Font::createGlyph : doen't support bitmap.pixel_mode '%d'"), bitmap.pixel_mode);
        }

        if (pPixelBuffer)
        {
            int x, y;
            FontTextureNodePtr node = m_texMgr.getTexNode(m_height);
            if(node && node->lockOne(x, y))
            {
                pGLData->texU = x;
                pGLData->texV = y;
                pGLData->texture = node->texture();

                DrawGlyphToTexture(*pGLData, pPixelBuffer);
            }

            if (bShouldDeletePixelBuffer)
            {
                delete [] pPixelBuffer;
            }

            pPixelBuffer = nullptr;
        }

        pGLData->valid = true;
        return pGLData;
    }

    int Font::getCharWidth(wchar_t ch)
    {
        if (ch == L'\n') return 0;
        if (ch == L'\r' || ch == L' ') return m_height / 2;
        if (ch == L'\t') return m_height * 2;

        CharGlyphData* pData = getGlyph(ch);
        if (!pData || !pData->valid) return m_height / 2;

        return pData->advanceX;
    }

    bool Font::fillWordToTexture(dx::Texture *pDestTex, 
        int dx, int dy, wchar_t ch, D3DXCOLOR color,
        int & ax, int & ay)
    {
        CharGlyphData *pGlyph = getGlyph(ch);
        if (!pGlyph || !pGlyph->valid)
        {
            ax = ay = m_height;
            return false;
        }

        ax = pGlyph->advanceX;
        ay = pGlyph->advanceY;

        dx::Texture *pSrcTex = pGlyph->texture->getTexture();
        if (!pSrcTex)
        {
            return false;
        }

        D3DLOCKED_RECT destLockRC, srcLockRC;

        {
            RECT destRC;
            destRC.left = dx + pGlyph->leftMargin;
            destRC.top = dy + pGlyph->topMargin;
            destRC.right = destRC.left + pGlyph->bitmapWidth;
            destRC.bottom = destRC.top + pGlyph->bitmapHeight;

            HRESULT hr = pDestTex->LockRect(0, &destLockRC, &destRC, 0);
            if(FAILED(hr))
            {
                LOG_ERROR(_T("Font::fillWordToTexture: lock dest texture faild:%d"), HRESULT_CODE(hr));
                return false;
            }
        }
        {
            RECT srcRC;
            srcRC.left = pGlyph->texU;
            srcRC.top = pGlyph->texV;
            srcRC.right = srcRC.left + pGlyph->bitmapWidth;
            srcRC.bottom = srcRC.top + pGlyph->bitmapHeight;

            if(FAILED(pSrcTex->LockRect(0, &srcLockRC, &srcRC, 0)))
            {
                pDestTex->UnlockRect(0);
                return false;
            }
        }

        int nWidth = pGlyph->bitmapWidth;
        int nHeight = pGlyph->bitmapHeight;

        unsigned char r = (unsigned char)(255 * color.r);
        unsigned char g = (unsigned char)(255 * color.g);
        unsigned char b = (unsigned char)(255 * color.b);

        for (int y = 0; y < nHeight; ++y)
        {
            for (int x = 0; x < nWidth; ++x)
            {
                unsigned char* src_pixel = ((unsigned char*)srcLockRC.pBits) + srcLockRC.Pitch * y + x;
                unsigned char* dest_pixel = ((unsigned char*)destLockRC.pBits) + destLockRC.Pitch * y + x * 4;
                 
                dest_pixel[0] = b;
                dest_pixel[1] = g;
                dest_pixel[2] = r;
                dest_pixel[3] = BYTE(color.a * (*src_pixel));
            }
        }

        pSrcTex->UnlockRect(0);
        pDestTex->UnlockRect(0);

        return true;
    }
    
    bool Font::fillTextToTexture(dx::Texture * pDestTex,
        int dx, int dy, const std::wstring & text, DWORD color)
    {
        int ax = 0, ay = 0;
        for (std::wstring::const_iterator it = text.begin(); it != text.end(); ++it)
        {
            fillWordToTexture(pDestTex, dx, dy, *it, color, ax, ay);
            dx += ax;
        }

        return true;
    }


    //////////////////////////////////////////////////////////////////////////
    static FT_Library g_ft_lib;

    /*static*/ FontMgr * FontMgr::instance()
    {
        static FontMgr s_pMgr;
        
        return &s_pMgr;
    }

    FontMgr::FontMgr()
        : m_valid(false)
    {
    }

    FontMgr::~FontMgr()
    {

    }

    bool FontMgr::init()
    {
        assert(!m_valid && "init more than once!");

        m_valid = true;
        FT_Init_FreeType(&g_ft_lib);
        return true;
    }

    void FontMgr::fini()
    {
        if (m_valid)
        {
            m_valid = false;
            FT_Done_FreeType(g_ft_lib);
        }
    }

    bool FontMgr::registerFontFile(const tstring & fontFile, const tstring & key)
    {
        FaceMap::iterator it = m_faceMap.find(key);

        if (it != m_faceMap.end())
            return true;

        tstring realPath;
        if (!getfs()->getRealPath(realPath, fontFile))
        {
            LOG_ERROR(L"Can't find font file '%s'", fontFile.c_str());
            return false;
        }

        void* pFace = createFace(realPath);
        m_faceMap[key] = pFace;
        return pFace != NULL;
    }

    void* FontMgr::getFace(const tstring & faceName)
    {
        FaceMap::iterator it = m_faceMap.find(faceName);
        if (it != m_faceMap.end())
            return it->second;

        throw(std::runtime_error("the font face doesn't registered!please register it first."));
    }

    FontPtr FontMgr::getFont(const tstring & fontName)
    {
        FontMap::iterator it = m_fontMap.find(fontName);
        if (it != m_fontMap.end())
        {
            return it->second;
        }

        //字体信息无效
        FontInfo info(fontName);
        if (!info.valid()) return NULL;

        //没有此字体
        void* pFace = getFace(info.name);
        if(!pFace) return NULL;

        //创建新的字体
        FontPtr ptr(new Font(pFace, info.height));
        m_fontMap[fontName] = ptr;

        return ptr;
    }

    void* FontMgr::createFace(const tstring & faceName)
    {
        //FT模块的返回值。为0表示成功。
        FT_Error nFTResult = 0;
        FT_Face pFace = NULL ;

#ifdef _UNICODE
        std::string faceNameA = wcharToChar(faceName);
        nFTResult = FT_New_Face(g_ft_lib, faceNameA.c_str(), 0, &pFace);
#else
        //加载一个字体。
        nFTResult = FT_New_Face(g_ft_lib, faceName.c_str(), 0, &pFace);
#endif
        if (nFTResult)
        {
            LOG_ERROR(_T("Load font file '%s' failed!"), faceName.c_str());
            return NULL;
        }

        //FontFace加载完毕后，默认是Unicode charmap。
        //这里再主动设置一下，使用Unicode charmap。
        nFTResult = FT_Select_Charmap(pFace, FT_ENCODING_UNICODE);
        if (nFTResult)
        {
            LOG_ERROR(_T("Select Unicode Encode Failed!"));
            return NULL;
        }

        LOG_INFO(_T("Load font file '%s'."), faceName.c_str());
        return pFace;
    }

}//namespace Lazy