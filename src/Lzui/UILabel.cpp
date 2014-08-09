
#include "stdafx.h"
#include "UILabel.h"
#include "UIGuiMgr.h"
#include "UISprite.h"


namespace Lazy
{

    CLabel::CLabel(void)
        : m_align(RelativeAlign::left | RelativeAlign::top)
        , m_bMutiLine(false)
        , m_lineSpace(4)
        , m_maxWidth(100)
    {
        enable(false);
        m_size.set(m_maxWidth, 20);

        m_fontPtr = getDefaultFontPtr();
        createTextSprite();
    }


    CLabel::~CLabel(void)
    {
    }

    void CLabel::create(int id, const tstring & caption, int x, int y)
    {
        m_id = id;
        m_position.set(x, y);
        setText(caption);
    }


    void CLabel::enableMutiLine(bool enable)
    {
        if (m_bMutiLine == enable) return;

        m_bMutiLine = enable;
        createTextSprite();
    }

    void CLabel::createTextSprite()
    {
        if (m_bMutiLine)
            m_textSprite = new TextViewSprite();
        else
            m_textSprite = new TextLineSprite();

        m_textSprite->setColor(m_color);
        m_textSprite->setMaxWidth(m_size.cx);

        updateText();
    }

    void CLabel::update(float elapse)
    {
        m_textSprite->update(elapse);
    }

    void CLabel::render(IUIRender * pDevice)
    {
        if (m_text.empty()) return;
        if (!m_textSprite) return;

        CPoint pt;
        localToGlobal(pt);

        if (m_align & RelativeAlign::hcenter)
        {
            pt.x += (m_size.cx - m_textSprite->getSize().cx) / 2;
        }
        else if (m_align & RelativeAlign::right)
        {
            pt.x += m_size.cx - m_textSprite->getSize().cx;
        }

        if (m_align & RelativeAlign::vcenter)
        {
            pt.y += (m_size.cy - m_textSprite->getSize().cy) / 2;
        }
        else if (m_align & RelativeAlign::bottom)
        {
            pt.y += m_size.cy - m_textSprite->getSize().cy;
        }

        m_textSprite->render(pDevice, pt);
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
        {
            return;
        }

        if (m_textSprite)
            m_textSprite->setText(m_text, m_fontPtr);
    }

    void CLabel::setFont(const std::wstring & font)
    {
        if (m_font == font) return;

        m_font = font;
        m_fontPtr = FontMgr::instance()->getFont(font);

        updateText();
    }

    void CLabel::setText(const std::wstring & text)
    {
        if (m_text == text) return;

        m_text = text;
        m_text.erase(std::remove(m_text.begin(), m_text.end(), '\r'), m_text.end());

        updateText();
    }

    void CLabel::setLineSpace(int lineSpace)
    {
        m_lineSpace = lineSpace;
        m_textSprite->setLineSpace(lineSpace);
    }

    void CLabel::setMaxWidth(int width)
    {
        m_maxWidth = width;
        m_textSprite->setMaxWidth(width);
    }

    void CLabel::setSize(int w, int h)
    {
        Widget::setSize(w, h);

        if (m_textSprite)
            m_textSprite->setMaxWidth(w);
    }

    void CLabel::setColor(uint32 color)
    {
        Widget::setColor(color);
        if (m_textSprite)
            m_textSprite->setColor(color);
    }

    ///加载布局。
    void CLabel::loadFromStream(LZDataPtr config)
    {
        Widget::loadFromStream(config);

        setAlign(config->readInt(L"textAlign"));
        enableMutiLine(config->readBool(L"mutiline"));
    }

    ///保存布局
    void CLabel::saveToStream(LZDataPtr config)
    {
        Widget::saveToStream(config);

        config->writeInt(L"textAlign", m_align);
        config->writeBool(L"mutiline", m_bMutiLine);
    }

}//namespace Lazy