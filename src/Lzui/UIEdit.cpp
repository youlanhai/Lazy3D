#include "StdAfx.h"
#include "UIEdit.h"
#include "UIGuiMgr.h"
#include "UISprite.h"
#include "../Font/Font.h"
#include "../Render/RenderDevice.h"

namespace Lazy
{


    Edit::Edit(void)
        : m_bMutiLine(false)
        , m_cursor(0)
    {
        setSize(80, 20);

        m_fontPtr = getDefaultFontPtr();

        createTextSprite();
    }

    Edit::~Edit(void)
    {
        getGUIMgr()->unactiveEdit(this);
    }

    void Edit::insertChar(wchar_t ch)
    {
        m_text.insert(m_cursor, 1, ch);
        m_textSprite->insertWord(m_cursor, ch, m_fontPtr);
        ++m_cursor;
    }

    void Edit::delFront()
    {
        if (m_cursor != 0 && m_cursor <= m_text.size())
        {
            --m_cursor;
            m_text.erase(m_cursor, 1);
            m_textSprite->delWord(m_cursor);
        }
    }

    void Edit::delBack()
    {
        if (m_cursor < m_text.size())
        {
            m_text.erase(m_cursor, 1);
            m_textSprite->delWord(m_cursor);
        }
    }

    void Edit::cursorGoUp()
    {
        if (!m_bMutiLine) return;
        if (!m_fontPtr) return;

        int x, y;
        m_textSprite->getCursorPos(m_cursor, x, y);
        m_textSprite->getCursorByPos(m_cursor, x, y - m_fontPtr->getHeight());
    }
    void Edit::cursorGoDown()
    {
        if (!m_bMutiLine) return;
        if (!m_fontPtr) return;

        int x, y;
        m_textSprite->getCursorPos(m_cursor, x, y);
        m_textSprite->getCursorByPos(m_cursor, x, y + m_fontPtr->getHeight());
    }
    void Edit::cursorGoLeft()
    {
        if (m_cursor > 0)
            --m_cursor;
    }
    void Edit::cursorGoRight()
    {
        if (m_cursor < m_text.size())
            ++m_cursor;
    }

    void Edit::onSysChar(wchar_t ch)
    {
        //debugMessage(_T("cedit onSysChar:%d"), ch);

        if (ch >= 32) insertChar(ch);

        //debugMessage(_T("cedit text: cur=%d len=%d tex='%s'"), m_cursor, m_text.size(), m_text.c_str());
    }

    bool Edit::onSysKey(bool isKeyDown, EKEY_CODE key)
    {
        if (!isKeyDown)
        {
            return true;
        }

        if (key == KEY_LEFT)
        {
            cursorGoLeft();
        }
        else if (key == KEY_RIGHT)
        {
            cursorGoRight();
        }
        else if (key == KEY_UP)
        {
            cursorGoUp();
        }
        else if (key == KEY_DOWN)
        {
            cursorGoDown();
        }
        else if (key == KEY_HOME)
        {
            m_cursor = 0;
        }
        else if (key == KEY_END)
        {
            m_cursor = m_text.size();
        }
        else if (key == KEY_DELETE)
        {
            delBack();
        }
        else if (key == KEY_BACK)
        {
            delFront();
        }
        else if (key == KEY_RETURN)
        {
            insertChar(L'\n');
        }
        else if (key == KEY_TAB)
        {
            insertChar(L'\t');
        }
        else if (key == KEY_ESCAPE)
        {
            setActived(NULL);
        }
        return true;
    }

    void Edit::update(float elapse)
    {
        m_textSprite->update(elapse);
    }

    void Edit::render(IUIRender * pDevice)
    {
        if (!m_fontPtr) return;

        CRect rc = getGlobalRect();

#if 1
        pDevice->drawRect(rc, 0x7f000000);
#endif

        m_textSprite->render(pDevice, CPoint(rc.left, rc.top));

        //渲染光标
        if (getGUIMgr()->isEditActive(this))
        {
            float delta = sinf(GetTickCount() * 0.003f);
            DWORD cr = DWORD(255 * fabs(delta));
            cr = (cr << 24) | 0x00ffffff;

            int x, y;
            m_textSprite->getCursorPos(m_cursor, x, y);

            CRect rc2(x, y, x + 2, y + m_fontPtr->getHeight());
            rc2.offset(rc.left, rc.top);
            pDevice->drawRect(rc2, cr);
        }
    }

    bool Edit::onEvent(const SEvent & event)
    {
        if (event.eventType == EET_GUI_EVENT)
        {
            if (event.guiEvent.message == GuiMsg::getActived)
            {
                onFocusGet();
            }
            else if (event.guiEvent.message == GuiMsg::lostActived)
            {
                onFocusLost();
            }
        }
        else if (event.eventType == EET_MOUSE_EVENT)
        {
            if (event.mouseEvent.event == EME_LMOUSE_DOWN)
            {
                CPoint pt(event.mouseEvent.x, event.mouseEvent.y);
                pt -= getGlobalPosition();
                //pt -= getAbsPosition();
                m_textSprite->getCursorByPos(m_cursor, pt.x, pt.y);
            }
        }
        else if (event.eventType == EET_CHAR_EVENT)
        {
            onSysChar(event.charEvent.ch);
            return true;
        }
        else if (event.eventType == EET_KEY_EVENT)
        {
            return onSysKey(event.keyEvent.down, event.keyEvent.key);
        }

        return Widget::onEvent(event);
    }

    void Edit::onFocusGet()
    {
        getGUIMgr()->activeEdit(this);
#ifdef ENABLE_SCRIPT
        m_self.call_method_quiet("onFocusGet");
#endif
    }

    void Edit::onFocusLost()
    {
        getGUIMgr()->unactiveEdit(this);
#ifdef ENABLE_SCRIPT
        m_self.call_method_quiet("onFocusLost");
#endif
    }

    void Edit::updateText()
    {
        if (!m_fontPtr)
        {
            //m_textSprite->clear();
            return;
        }

        if (m_textSprite)
            m_textSprite->setText(m_text, m_fontPtr);
    }

    void Edit::setMutiLine(bool enable)
    {
        if (m_bMutiLine == enable) return;

        m_bMutiLine = enable;

        createTextSprite();
    }

    void Edit::createTextSprite()
    {
        if (m_bMutiLine)
            m_textSprite = new TextViewSprite();
        else
            m_textSprite = new TextLineSprite();

        m_textSprite->setColor(m_textColor);
        m_textSprite->setMaxWidth(m_size.x);

        updateText();
    }

    void Edit::setText(const tstring & text)
    {
        if (m_text == text) return;

        m_text = text;
        m_cursor = m_text.size();

        updateText();
    }

    void Edit::setFont(const tstring & font)
    {
        if (m_font == font) return;

        m_font = font;
        m_fontPtr = FontMgr::instance()->getFont(font);

        updateText();
    }

    void Edit::setTextColor(uint32 color)
    {
        m_textColor = color;
        if (m_textSprite)
            m_textSprite->setColor(color);
    }

    void Edit::setSize(int w, int h)
    {
        Widget::setSize(w, h);

        if (m_textSprite)
            m_textSprite->setMaxWidth(w);
    }

    bool Edit::setProperty(LZDataPtr config, const tstring & key, LZDataPtr val)
    {
        if (key == L"mutiline")
            setMutiLine(val->asBool());
        else
            return Widget::setProperty(config, key, val);
        return true;
    }

}//namespace Lazy