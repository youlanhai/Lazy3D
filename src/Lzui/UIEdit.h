#pragma once

#include "UIControl.h"

namespace Lazy
{
    /**编辑框*/
    class CEdit : public Widget
    {
    public:
        MAKE_UI_HEADER(CEdit)

        CEdit(void);
        virtual ~CEdit(void);

        virtual void update(float elapse) override;
        virtual void render(IUIRender * pDevice) override;

        void setMutiLine(bool m);
        bool isMutiLine() const { return m_bMutiLine; }

        virtual void setText(const tstring & text) override;
        virtual void setFont(const tstring & name) override;
        virtual void setColor(uint32 color) override;
        virtual void setSize(int w, int h) override;

        virtual void loadFromStream(LZDataPtr root);
        virtual void saveToStream(LZDataPtr root);

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
        FontPtr     m_fontPtr;
        TextSpritePtr   m_textSprite;

        bool        m_bMutiLine;
        size_t      m_cursor;
    };

}