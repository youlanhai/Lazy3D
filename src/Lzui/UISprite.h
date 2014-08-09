#pragma once

namespace Lazy
{

    typedef std::vector<SpritePtr> SpritePool;

    class TextureSprite : public ISprite
    {
    public:
        TextureSprite();
        ~TextureSprite();

        virtual void update(float elapse);
        virtual void render(IUIRender * pDevice, const CPoint & world);

        void setTexture(TexturePtr tex, const UVRect & rect = IdentityUVRect);
        void setUVRect(const UVRect & rc);

    protected:
        UVRect m_srcRect;
        TexturePtr  m_texture;
    };

    //一张图片
    class ImageSprite : public TextureSprite
    {
    public:
        ImageSprite();
        ~ImageSprite();

        void setImage(const tstring & image);
        void setImage(const tstring & image, const UVRect & rc);

        const tstring & getImage() const { return m_image; }

    protected:
        tstring     m_image;
    };

    //一个控件
    class ControlSprite : public ISprite
    {
    public:
        ControlSprite();
        ~ControlSprite();

        void setControl(WidgetPtr control) { m_control = control; }
        WidgetPtr getControl() const { return m_control; }

        virtual void update(float elapse);
        virtual void render(IUIRender * pDevice, const CPoint & world);

    private:
        WidgetPtr m_control;
    };

    //一个文字
    class WordSprite : public ISprite
    {
    public:
        WordSprite(bool grouped = false);
        ~WordSprite();

        void setWord(wchar_t ch, FontPtr font, uint32 color);
        wchar_t getWord() const { return m_ch; }

        void setColor(uint32 color) { m_color = color; }
        uint32 getColor() const { return m_color; }

        inline void update(float elapse) {}
        virtual void render(IUIRender * pDevice, const CPoint & world);

    private:
        bool    m_grouped;
        wchar_t m_ch;
        uint32  m_color;
        CPoint  m_offsetPos;
        CSize   m_offsetSize;
        TexturePtr m_texture;
        UVRect  m_uvRect;
    };
    typedef RefPtr<WordSprite> WordSpritePtr;


    class ITextSprite : public ISprite
    {
    public:

        ITextSprite();
        ~ITextSprite();

        virtual void insertWord(size_t pos, wchar_t ch, FontPtr font) = 0;
        virtual void delWord(size_t pos) = 0;
        virtual size_t getTextLines() const = 0;
        virtual bool getCursorByPos(size_t & cursor, int x, int y) = 0;
        virtual void getCursorPos(size_t cursor, int & x, int & y) = 0;

        virtual void setText(const std::wstring & text, FontPtr font);

        void insertText(size_t pos, const std::wstring & text, FontPtr font);
        void delText(size_t pos, size_t count);

        const std::wstring & getText() const { return m_text; }
        wchar_t getWord(size_t pos) const { return m_text[pos]; }

        uint32 getColor() const { return m_color; }
        virtual void setColor(uint32 color) { m_color = color; }

        virtual void setMaxWidth(int width);
        int getMaxWidth() const { return m_maxWidth; }

        virtual void setLineSpace(int space);
        int getLineSpace() const { return m_lineSpace; }

        size_t length() const { return m_text.length(); }

        void clear();
        void layout();

        virtual void update(float elapse);

    protected:
        virtual void onClear() = 0;
        virtual void onLayout() = 0;

    protected:
        uint32          m_color;
        std::wstring    m_text;
        bool            m_layoutDirty;
        int             m_lineSpace;    ///行距
        int             m_maxWidth;     ///最大宽度
    };

    ///集中渲染一行文本，以提高渲染效率
    class TextLineSprite : public ITextSprite
    {
    public:
        TextLineSprite(bool grouped = false);
        ~TextLineSprite();

        WordSpritePtr getSprite(size_t pos) { return m_sprites[pos]; }
        void insertWord(size_t pos, WordSpritePtr sprite);

        virtual void setText(const std::wstring & text, FontPtr font);
        virtual void setColor(uint32 color);
        virtual size_t getTextLines() const { return m_text.empty() ? 0 : 1; }

        virtual void insertWord(size_t pos, wchar_t ch, FontPtr font);
        virtual void delWord(size_t pos);

        virtual void onClear();
        virtual void onLayout();

        virtual void render(IUIRender * pDevice, const CPoint & world);

        virtual bool getCursorByPos(size_t & cursor, int x, int y);
        virtual void getCursorPos(size_t cursor, int & x, int & y);

    private:
        bool            m_grouped;
        std::vector<WordSpritePtr>      m_sprites;
    };
    typedef RefPtr<TextLineSprite> TextLineSpritePtr;

    ///支持多行文字渲染
    class TextViewSprite : public ITextSprite
    {
    public:
        TextViewSprite();
        ~TextViewSprite();

        virtual void setText(const std::wstring & text, FontPtr font);
        virtual void setColor(uint32 color);
        virtual size_t getTextLines() const { return m_lineSprites.size(); }

        virtual void insertWord(size_t pos, wchar_t ch, FontPtr font);
        virtual void delWord(size_t pos);

        virtual void onClear();
        virtual void onLayout();

        virtual void render(IUIRender * pDevice, const CPoint & world);
        virtual bool getCursorByPos(size_t & cursor, int x, int y);
        virtual void getCursorPos(size_t cursor, int & x, int & y);

    protected:
        TextLineSpritePtr createLineSprite();

    private:
        std::vector<TextLineSpritePtr> m_lineSprites;
    };
    typedef RefPtr<TextViewSprite> TextViewSpritePtr;


    class RitchViewSprite : public ITextSprite
    {
    public:

        RitchViewSprite();
        ~RitchViewSprite();

        void update(float elapse);
        void render(IUIRender * pDevice, const CPoint & world);


        static ISprite * createImage(const tstring & image, const UVRect & srcRc = IdentityUVRect);
        static ISprite * createTexture(TexturePtr tex, const UVRect & srcRc = IdentityUVRect);
        static ISprite * createWord(wchar_t ch, FontPtr font);
        static ISprite * createText(const tstring & text, FontPtr font);
        static ISprite * createControl(Widget * pctl);


        void addSprite(SpritePtr sprite);
        void addTexture(TexturePtr tex, const UVRect & srcRc = IdentityUVRect);
        void addImage(const tstring & image, const UVRect & srcRc = IdentityUVRect);
        void addWord(wchar_t ch, FontPtr font);
        void addWord(wchar_t ch, const tstring & fontname);
        void addText(const std::wstring & str, FontPtr font);
        void addControl(WidgetPtr control);

        void insertSprite(size_t pos, SpritePtr sprite);

        void layout();

        size_t numSprites() const { return m_sprites.size(); }
        SpritePtr getSprite(size_t i) { return m_sprites[i]; }

        bool empty() const { return m_sprites.empty(); }
        void clear() { m_sprites.clear(); }

    private:
        SpritePool m_sprites;
    };

}//namespace Lazy