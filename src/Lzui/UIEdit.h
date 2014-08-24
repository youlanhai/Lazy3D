#pragma once

#include "UIWidget.h"

namespace Lazy
{
    /**编辑框*/
    class Edit : public Widget
    {
    public:
        MAKE_UI_HEADER(Edit)

        Edit(void);
        virtual ~Edit(void);

        virtual void update(float elapse) override;
        virtual void render(IUIRender * pDevice) override;

        void setMutiLine(bool m);
        bool isMutiLine() const { return m_bMutiLine; }

        void setText(const tstring & text);
        void setFont(const tstring & name);
        void setTextColor(uint32 color);
        void setSize(int w, int h);

        virtual bool setProperty(LZDataPtr config, const tstring & key, LZDataPtr val) override;

    protected:

        virtual bool onEvent(const SEvent & event) override;

        virtual void onSysChar(wchar_t ch);

        virtual bool onSysKey(bool isKeyDown, EKEY_CODE key);

        void onFocusGet();
        void onFocusLost();

        void insertChar(wchar_t ch);
        void delFront();
        void delBack();

        void cursorGoUp();
        void cursorGoDown();
        void cursorGoLeft();
        void cursorGoRight();

        void updateText();

        void createTextSprite();

    protected:
        tstring     m_text;
        tstring     m_font;
        uint32      m_textColor;
        uint32      m_selectionColor;
        uint32      m_cursorColor;

        FontPtr     m_fontPtr;
        TextSpritePtr   m_textSprite;

        bool        m_bMutiLine;
        size_t      m_cursor;
    };

}