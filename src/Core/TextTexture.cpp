
#include "stdafx.h"

#include "../Render/Texture.h"
#include "../Font/Font.h"

#include "TextTexture.h"
#include "App.h"

namespace Lazy
{
    bool clearTextrue(LPDIRECT3DTEXTURE9 pTexture, DWORD lvl, BYTE value)
    {
        if (!pTexture)
        {
            return false;
        }

        D3DSURFACE_DESC desc;
        if (FAILED(pTexture->GetLevelDesc(lvl, &desc)))
        {
            return false;
        }

        D3DLOCKED_RECT lockRC;
        if (FAILED(pTexture->LockRect(lvl, &lockRC, 0, 0)))
        {
            return false;
        }

        for (UINT i = 0; i < desc.Height; ++i)
        {
            memset((BYTE*)(lockRC.pBits) + lockRC.Pitch * i, value, lockRC.Pitch);
        }

        pTexture->UnlockRect(lvl);
        return true;
    }

    int near2Size(int size)
    {
        int t = 1;

        for (int i = 1; i < 32; ++i)
        {
            t = 1 << i;
            if(size < t)
            {
                return t;
            }
        }

        return t;
    }

//////////////////////////////////////////////////////////////////////////
    TextTextureEx::TextTextureEx()
        : m_color(0xff000000)
        , m_pTexture(0)
        , m_dirty(true)
        , m_width(0)
        , m_height(0)
    {

    }
    TextTextureEx::~TextTextureEx()
    {
        SafeRelease(m_pTexture);
    }

    void TextTextureEx::setText(const std::wstring & text)
    {
        if (m_text == text)
        {
            return;
        }
        m_text = text;
        m_dirty = true;
    }

    void TextTextureEx::setColor(DWORD cr)
    {
        if (m_color == cr)
        {
            return;
        }
        m_color = cr;
        m_dirty = true;
    }

    void TextTextureEx::setFont(const std::wstring & font)
    {
        if (m_font == font)
        {
            return;
        }

        m_font = font;
        m_dirty = true;
    }


    void TextTextureEx::loadTexture()
    {
        if (!m_dirty)
        {
            return;
        }

        m_dirty = false;//尽管会创建失败，但也要设置为false。

        FontPtr font = FontMgr::instance()->getFont(m_font);
        if (!font)
        {
            clearTextrue(m_pTexture, 0, 0);

            debugMessageA("ERROR : get font %s faild!", m_font.c_str());
            return;
        }

        m_width = (m_text.size() + 1) * font->getHeight();
        m_height = font->getHeight();

        if (m_pTexture)
        {
            D3DSURFACE_DESC desc;
            if(FAILED(m_pTexture->GetLevelDesc(0, &desc)))
            {
                clearTextrue(m_pTexture, 0, 0);

                debugMessage(_T("ERROR : get texture desc faild!"));
                return;
            }
            if(m_width > int(desc.Width) || m_height > int(desc.Height))
            {
                SafeRelease(m_pTexture);
            }
        }

        if (!m_pTexture)
        {

#if 1
            int texWidth = near2Size(m_width);
            int texHeight = near2Size(m_height);
#else
            int texWidth = m_width;
            int texHeight = m_height + 10;
#endif

            LPDIRECT3DDEVICE9 pDevice = getApp()->getDevice();
            if(FAILED(D3DXCreateTexture( pDevice, texWidth, texHeight,
                                         1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTexture)))
            {
                debugMessage(_T("ERROR : create texture for %s faild"), m_text.c_str());
                return ;
            }

            debugMessage(_T("create topboard texture (w=%d, h=%d) : %s"), texWidth, texHeight, m_text.c_str());
        }

        clearTextrue(m_pTexture, 0, 0);
        font->fillTextToTexture(m_pTexture, 0, 0, m_text, m_color);
    }

} // end namespace Lazy
