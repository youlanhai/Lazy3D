#pragma once

#include "Base.h"
#include "../utility/UtilConfig.h"

namespace Lazy
{

    class LZDLL_API TextTextureEx : public IBase
    {
    public:
        TextTextureEx();
        ~TextTextureEx();

        void setText(const std::wstring & text);
        void setColor(DWORD cr);
        void setFont(const std::wstring & font);

        LPDIRECT3DTEXTURE9 getTexture() const { return m_pTexture; }

        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }

        const std::wstring & getText() const { return m_text; }
        const std::wstring & getFont() const { return m_font; }
        DWORD getColor() const { return m_color; }

        void loadTexture();

    private:
        std::wstring     m_text;
        std::wstring     m_font;
        DWORD           m_color;
        LPDIRECT3DTEXTURE9 m_pTexture;
        bool            m_dirty;
        int             m_width;
        int             m_height;
    };

} // end namespace Lazy
