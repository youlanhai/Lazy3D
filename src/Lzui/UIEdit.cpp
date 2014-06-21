#include "StdAfx.h"
#include "UIEdit.h"
#include "UIGuiMgr.h"
#include "UISprite.h"
#include "../Font/Font.h"
#include "../Render/RenderDevice.h"

namespace Lazy
{


    CEdit::CEdit(void)
        : m_bMutiLine(false)
        , m_cursor(0)
    {
        setSize(80, 20);

        m_fontPtr = getDefaultFontPtr();

        createTextSprite();
    }

    CEdit::~CEdit(void)
    {
        getGUIMgr()->unactiveEdit(this);
    }

    void CEdit::insertChar(wchar_t ch)
    {
        m_text.insert(m_cursor, 1, ch);
        m_textSprite->insertWord(m_cursor, ch, m_fontPtr);
        ++m_cursor;
    }

    void CEdit::delFront()
    {
        if (m_cursor != 0 && m_cursor <= m_text.size())
        {
            --m_cursor;
            m_text.erase(m_cursor, 1);
            m_textSprite->delWord(m_cursor);
        }
    }

    void CEdit::delBack()
    {
        if (m_cursor < m_text.size())
        {
            m_text.erase(m_cursor, 1);
            m_textSprite->delWord(m_cursor);
        }
    }

    void CEdit::cursorGoUp()
    {
        if (!m_bMutiLine) return;
        if (!m_fontPtr) return;

        int x, y;
        m_textSprite->getCursorPos(m_cursor, x, y);
        m_textSprite->getCursorByPos(m_cursor, x, y - m_fontPtr->getHeight());
    }
    void CEdit::cursorGoDown()
    {
        if (!m_bMutiLine) return;
        if (!m_fontPtr) return;

        int x, y;
        m_textSprite->getCursorPos(m_cursor, x, y);
        m_textSprite->getCursorByPos(m_cursor, x, y + m_fontPtr->getHeight());
    }
    void CEdit::cursorGoLeft()
    {
        if (m_cursor > 0)
            --m_cursor;
    }
    void CEdit::cursorGoRight()
    {
        if (m_cursor < m_text.size())
            ++m_cursor;
    }

    void CEdit::onSysChar(wchar_t ch)
    {
        //debugMessage(_T("cedit onSysChar:%d"), ch);

        if (ch >= 32) insertChar(ch);

        //debugMessage(_T("cedit text: cur=%d len=%d tex='%s'"), m_cursor, m_text.size(), m_text.c_str());
    }

    bool CEdit::onSysKey(bool isKeyDown, EKEY_CODE key)
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

    void CEdit::update(float elapse)
    {
        m_textSprite->update(elapse);
    }

    void CEdit::render(IUIRender * pDevice)
    {
        if (!m_fontPtr) return;

        CRect rc = getClientRect();
        localToGlobal(rc);

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

    bool CEdit::onEvent(const SEvent & event)
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
                m_textSprite->getCursorByPos(m_cursor, event.mouseEvent.x, event.mouseEvent.y);
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

        return IControl::onEvent(event);
    }

    void CEdit::onFocusGet()
    {
        getGUIMgr()->activeEdit(this);
#ifdef ENABLE_SCRIPT
        m_self.call_method_quiet("onFocusGet");
#endif
    }

    void CEdit::onFocusLost()
    {
        getGUIMgr()->unactiveEdit(this);
#ifdef ENABLE_SCRIPT
        m_self.call_method_quiet("onFocusLost");
#endif
    }

    void CEdit::updateText()
    {
        if (!m_fontPtr)
        {
            //m_textSprite->clear();
            return;
        }

        if (m_textSprite)
            m_textSprite->setText(m_text, m_fontPtr);
    }

    void CEdit::setMutiLine(bool enable)
    {
        if (m_bMutiLine == enable) return;

        m_bMutiLine = enable;

        createTextSprite();
    }

    void CEdit::createTextSprite()
    {
        if (m_bMutiLine)
            m_textSprite = new TextViewSprite();
        else
            m_textSprite = new TextLineSprite();

        m_textSprite->setColor(m_color);
        m_textSprite->setMaxWidth(m_size.cx);

        updateText();
    }

    void CEdit::setText(const tstring & text)
    {
        if (m_text == text) return;

        m_text = text;
        m_cursor = m_text.size();

        updateText();
    }

    void CEdit::setFont(const tstring & font)
    {
        if (m_font == font) return;

        m_font = font;
        m_fontPtr = FontMgr::instance()->getFont(font);

        updateText();
    }

    void CEdit::setColor(uint32 color)
    {
        IControl::setColor(color);
        if (m_textSprite)
            m_textSprite->setColor(color);
    }

    void CEdit::setSize(int w, int h)
    {
        IControl::setSize(w, h);

        if (m_textSprite)
            m_textSprite->setMaxWidth(w);
    }

    void CEdit::loadFromStream(LZDataPtr root)
    {
        IControl::loadFromStream(root);

        setMutiLine(root->readBool(L"mutiline", false));
    }

    void CEdit::saveToStream(LZDataPtr root)
    {
        IControl::saveToStream(root);

        root->writeBool(L"mutiline", m_bMutiLine);
    }

}//namespace Lazy