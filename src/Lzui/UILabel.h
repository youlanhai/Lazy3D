#pragma once

#include "UIWidget.h"

namespace Lazy
{

    class LZUI_API Label : public Widget
    {
    public:
        MAKE_UI_HEADER(Label)

        Label(void);
        ~Label(void);

        virtual void create(const tstring & text, int x, int y);

        ///设置文本
        virtual void setText(const tstring & text);
        virtual const tstring & getText(void) const { return m_text; }

        ///设置字体
        virtual void setFont(const tstring & name);
        virtual const tstring & getFont(void) const { return m_font; }

        void setTextColor(uint32 color);
        uint32 getTextColor() const;

        void setLineSpace(int lineSpace);
        int getLineSpace() const { return m_lineSpace; }

        void setTextAlign(int align) { m_textAlign = align; }
        uint32 getTextAlign(void) const { return m_textAlign; }

        void setMutiLine(bool enable);
        bool getMutiLine() const { return m_mutiLine; }

        const CSize & getTextSize() ;
        int getTextLines();

        virtual void update(float elapse);
        virtual void render(IUIRender * pDevice);

        void setSize(int w, int h) override;

        ///加载布局。
        virtual void loadProperty(LZDataPtr config) override;

        ///保存布局
        virtual void saveProperty(LZDataPtr config) override;

    protected:
        void createTextSprite();
        void updateText();

        tstring         m_text;         ///<文本
        tstring         m_font;         ///<字体名称
        uint32          m_textColor;
        uint32	        m_textAlign;    ///排版风格
        bool            m_mutiLine;
        int             m_lineSpace;

        TextSpritePtr   m_textSprite;
        FontPtr         m_fontPtr;
    };


}//namespace Lazy