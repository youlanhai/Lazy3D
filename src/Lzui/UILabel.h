#pragma once

namespace Lazy
{

    class LZUI_API CLabel : public IControl
    {
    public:
        MAKE_UI_HEADER(CLabel, uitype::Label)

        CLabel(void);
        ~CLabel(void);

        virtual void create(int id, const tstring & caption, int x, int y);

        void setAlign(int align) { m_align = align; }
        void setText(const std::wstring & text) override;
        void setFont(const std::wstring & font) override;
        void setColor(uint32 color) override;
        void setSize(int w, int h) override;

        void setLineSpace(int lineSpace);
        int getLineSpace() const { return m_lineSpace; }

        void setMaxWidth(int width);
        int getMaxWidth() const { return m_maxWidth; }

        uint32 getAlign(void) const { return m_align; }
        const CSize & getTextSize() ;
        int getTextLines();

        void enableMutiLine(bool enable);
        bool canMutiLine() const { return m_bMutiLine; }

        virtual void update(float elapse);
        virtual void render(IUIRender * pDevice);

        ///加载布局。
        virtual void loadFromStream(LZDataPtr config) override;

        ///保存布局
        virtual void saveToStream(LZDataPtr config) override;

    protected:
        void createTextSprite();
        void updateText();

        uint32	    m_align;    ///排版风格
        FontPtr     m_fontPtr;

        bool        m_bMutiLine;
        TextSpritePtr   m_textSprite;

        int m_lineSpace;
        int m_maxWidth;
    };


}//namespace Lazy