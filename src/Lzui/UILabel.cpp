
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
    void Label::loadProperty(LZDataPtr config)
    {
        Widget::loadProperty(config);

        setTextAlign(config->readInt(L"textAlign"));
        setMutiLine(config->readBool(L"mutiline"));
        setFont(config->readString(L"font"));
        setText(config->readString(L"text"));
        setTextColor(config->readHex(L"textColor"));
        setLineSpace(config->readInt(L"lineSpace"));
    }

    ///保存布局
    void Label::saveProperty(LZDataPtr config)
    {
        Widget::saveProperty(config);

        config->writeInt(L"textAlign", m_align);

        if(!m_mutiLine)
            config->writeBool(L"mutiline", m_mutiLine);

        if (!m_font.empty())
            config->writeString(L"font", m_font);

        if (!m_text.empty())
            config->writeString(L"text", m_text);

        if (m_textColor != 0xffffffff)
            config->writeHex(L"textColor", m_textColor);

        if (m_lineSpace != 0)
            config->writeInt(L"lineSpace", m_lineSpace);

    }

}//namespace Lazy