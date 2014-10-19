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
        void setColor(uint32 color);
        void setFont(const std::wstring & font);

        dx::Texture * getTexture() const { return m_pTexture; }

        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }

        const std::wstring & getText() const { return m_text; }
        const std::wstring & getFont() const { return m_font; }
        uint32 getColor() const { return m_color; }

        void loadTexture();

    private:
        std::wstring    m_text;
        std::wstring    m_font;
        uint32          m_color;
        dx::Texture *   m_pTexture;
        bool            m_dirty;
        int             m_width;
        int             m_height;
    };

} // end namespace Lazy
