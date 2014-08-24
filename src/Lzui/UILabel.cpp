
#include "stdafx.h"
#include "UILabel.h"
#include "UIGuiMgr.h"
#include "UISprite.h"


namespace Lazy
{

    Label::Label(void)
        : m_textAlign(RelativeAlign::left | RelativeAlign::top)
        , m_mutiLine(false)
        , m_lineSpace(4)
        , m_font(getDefaultFont())
        , m_textColor(0xffffffff)
    {
        m_size.set(100, 20);

        m_fontPtr = getDefaultFontPtr();
        createTextSprite();
    }

    Label::~Label(void)
    {
    }

    void Label::create(const tstring & text, int x, int y)
    {
        setPosition(x, y);
        setText(text);
    }

    void Label::setMutiLine(bool enable)
    {
        if (m_mutiLine == enable) return;

        m_mutiLine = enable;
        createTextSprite();
    }

    void Label::createTextSprite()
    {
        if (m_mutiLine)
            m_textSprite = new TextViewSprite();
        else
            m_textSprite = new TextLineSprite();

        m_textSprite->setColor(m_textColor);
        m_textSprite->setMaxWidth(m_size.x);

        updateText();
    }

    void Label::update(float elapse)
    {
        Widget::update(elapse);

        m_textSprite->update(elapse);
    }

    void Label::render(IUIRender * pDevice)
    {
        if (m_textSprite)
        {
            CPoint pt = getGlobalPosition();

            if (m_align & RelativeAlign::hcenter)
            {
                pt.x += (m_size.x - m_textSprite->getSize().cx) / 2;
            }
            else if (m_align & RelativeAlign::right)
            {
                pt.x += m_size.x - m_textSprite->getSize().cx;
            }

            if (m_align & RelativeAlign::vcenter)
            {
                pt.y += (m_size.y - m_textSprite->getSize().cy) / 2;
            }
            else if (m_align & RelativeAlign::bottom)
            {
                pt.y += m_size.y - m_textSprite->getSize().cy;
            }

            m_textSprite->render(pDevice, pt);
        }

        Widget::render(pDevice);
    }

    const CSize & Label::getTextSize()
    {
        m_textSprite->layout();
        return m_textSprite->getSize();
    }

    int Label::getTextLines()
    {
        m_textSprite->layout();
        return m_textSprite->getTextLines();
    }

    void Label::updateText()
    {
        if (!m_fontPtr)
            return;

        if (m_textSprite)
            m_textSprite->setText(m_text, m_fontPtr);
    }

    void Label::setFont(const std::wstring & font)
    {
        if (m_font == font)
            return;

        m_font = font;
        m_fontPtr = FontMgr::instance()->getFont(font);

        updateText();
    }

    void Label::setText(const std::wstring & text)
    {
        if (m_text == text)
            return;

        m_text = text;
        m_text.erase(std::remove(m_text.begin(), m_text.end(), '\r'), m_text.end());

        updateText();
    }

    void Label::setLineSpace(int lineSpace)
    {
        m_lineSpace = lineSpace;
        m_textSprite->setLineSpace(lineSpace);
    }

    void Label::setSize(int w, int h)
    {
        Widget::setSize(w, h);

        if (m_textSprite)
            m_textSprite->setMaxWidth(w);
    }

    void Label::setTextColor(uint32 color)
    {
        m_textColor = color;

        if (m_textSprite)
            m_textSprite->setColor(color);
    }

    ///加载布局。
    bool Label::setProperty(LZDataPtr config, const tstring & key, LZDataPtr val)
    {
        if (key == L"textAlign")
            setTextAlign(val->asInt());
        else if (key == L"mutiline")
            setMutiLine(val->asBool());
        else if (key == L"font")
            setFont(val->asString());
        else if (key == L"text")
            setText(val->asString());
        else if (key == L"textColor")
            setTextColor(val->asHex());
        else if (key == L"lineSpace")
            setLineSpace(val->asInt());
        else
            return Widget::setProperty(config, key, val);
        return true;
    }

}//namespace Lazy