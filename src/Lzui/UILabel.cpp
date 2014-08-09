
#include "stdafx.h"
#include "UILabel.h"
#include "UIGuiMgr.h"
#include "UISprite.h"


namespace Lazy
{

    CLabel::CLabel(void)
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

    CLabel::~CLabel(void)
    {
    }

    void CLabel::create(const tstring & text, int x, int y)
    {
        setPosition(x, y);
        setText(text);
    }

    void CLabel::setMutiLine(bool enable)
    {
        if (m_mutiLine == enable) return;

        m_mutiLine = enable;
        createTextSprite();
    }

    void CLabel::createTextSprite()
    {
        if (m_mutiLine)
            m_textSprite = new TextViewSprite();
        else
            m_textSprite = new TextLineSprite();

        m_textSprite->setColor(m_textColor);
        m_textSprite->setMaxWidth(m_size.x);

        updateText();
    }

    void CLabel::update(float elapse)
    {
        Widget::update(elapse);

        m_textSprite->update(elapse);
    }

    void CLabel::render(IUIRender * pDevice)
    {
        if (m_textSprite)
        {
            CPoint pt;
            localToGlobal(pt);

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

    const CSize & CLabel::getTextSize()
    {
        m_textSprite->layout();
        return m_textSprite->getSize();
    }

    int CLabel::getTextLines()
    {
        m_textSprite->layout();
        return m_textSprite->getTextLines();
    }

    void CLabel::updateText()
    {
        if (!m_fontPtr)
            return;

        if (m_textSprite)
            m_textSprite->setText(m_text, m_fontPtr);
    }

    void CLabel::setFont(const std::wstring & font)
    {
        if (m_font == font)
            return;

        m_font = font;
        m_fontPtr = FontMgr::instance()->getFont(font);

        updateText();
    }

    void CLabel::setText(const std::wstring & text)
    {
        if (m_text == text)
            return;

        m_text = text;
        m_text.erase(std::remove(m_text.begin(), m_text.end(), '\r'), m_text.end());

        updateText();
    }

    void CLabel::setLineSpace(int lineSpace)
    {
        m_lineSpace = lineSpace;
        m_textSprite->setLineSpace(lineSpace);
    }

    void CLabel::setSize(int w, int h)
    {
        Widget::setSize(w, h);

        if (m_textSprite)
            m_textSprite->setMaxWidth(w);
    }

    void CLabel::setTextColor(uint32 color)
    {
        m_textColor = color;

        if (m_textSprite)
            m_textSprite->setColor(color);
    }

    ///加载布局。
    void CLabel::loadProperty(LZDataPtr config)
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
    void CLabel::saveProperty(LZDataPtr config)
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