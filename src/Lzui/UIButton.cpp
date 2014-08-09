﻿
#include "stdafx.h"
#include "UIButton.h"
#include "UISprite.h"

namespace Lazy
{

#define CR_TIME 0.5f
    COLORREF buttonColor [] = { 0xaf808080, 0xafd0d0d0, 0xaf606060, 0xaf404040, 0xafa0a0a0 };

    //////////////////////////////////////////////////////////////////////////
    CButton::CButton(void)
        : m_colorFadable(true)
        , m_posMovable(true)
        , m_textSprite(new TextLineSprite())
        , m_textAlign(RelativeAlign::center)
    {
        memcpy(m_stateColor, buttonColor, sizeof(buttonColor));

        m_crFade.reset(m_stateColor[0], m_stateColor[0], CR_TIME);

        setBgColor(m_stateColor[0]);
        setBtnState(ButtonState::normal);
        enableDrag(false);
        enableClickTop(true);
        setSize(80, 30);

        m_fontPtr = getDefaultFontPtr();
        m_textSprite->setColor(m_color);
    }

    CButton::~CButton(void)
    {
    }

    void CButton::create(
        int id,
        const tstring & caption,
        const tstring & image,
        int x,
        int y,
        int w,
        int h)
    {
        m_id = id;
        setPosition(x, y);
        setSize(w, h);
        setText(caption);
        setImage(image);
    }

    void CButton::update(float fElapse)
    {
        if (m_colorFadable)
        {
            m_crFade.update(fElapse);
            setBgColor(m_crFade.getCurColor());
        }

        m_textSprite->update(fElapse);
    }

    void CButton::render(IUIRender * pDevice)
    {
        CRect rc = getClientRect();
        localToGlobal(rc);

        if (m_posMovable)
        {
            if (m_state == ButtonState::active)
            {
                rc.offset(-1, -1);
            }
            else if (m_state == ButtonState::down)
            {
                rc.offset(1, 1);
            }
        }

        renderImage(pDevice, rc);
        renderText(pDevice, rc);

    }

    void CButton::renderImage(IUIRender * pDevice, CRect rc)
    {
        TexturePtr texture = TextureMgr::instance()->get(getRealImage());
        pDevice->drawRect(rc, getBgColor(), texture);
    }

    void CButton::renderText(IUIRender * pDevice, CRect rc)
    {
        if (m_text.empty() || !m_fontPtr) return;

        CPoint pt(rc.left, rc.top);

        if (m_textAlign & RelativeAlign::hcenter)
        {
            pt.x += (m_size.cx - m_textSprite->getSize().cx) / 2;
        }
        else if (m_textAlign & RelativeAlign::right)
        {
            pt.x += m_size.cx - m_textSprite->getSize().cx;
        }

        if (m_textAlign & RelativeAlign::vcenter)
        {
            pt.y += (m_size.cy - m_textSprite->getSize().cy) / 2;
        }
        else if (m_textAlign & RelativeAlign::bottom)
        {
            pt.y += m_size.cy - m_textSprite->getSize().cy;
        }

        m_textSprite->render(pDevice, pt);
    }

    bool CButton::onEvent(const SEvent & event)
    {
        if (event.eventType == EET_GUI_EVENT)
        {
            switch (event.guiEvent.message)
            {
            case GuiMsg::mouseEnter:
                setBtnState(ButtonState::active);
                break;

            case GuiMsg::mouseLeave:
                setBtnState(ButtonState::normal);
                break;

            default: break;
            }
        }
        else if (event.eventType == EET_MOUSE_EVENT)
        {
            if (event.mouseEvent.event == EME_LMOUSE_DOWN)
            {
                setBtnState(ButtonState::down);
            }
            else if (event.mouseEvent.event == EME_LMOUSE_UP)
            {
                setBtnState(ButtonState::active);

#ifdef ENABLE_SCRIPT
                m_self.call_method_quiet("onButtonClick");
#endif
            }
        }


        return Widget::onEvent(event);
    }

    void CButton::setBtnState(int state)
    {
        m_state = state;
        m_crFade.setDestColor(m_stateColor[state]);
    }

    tstring CButton::getRealImage(void)
    {
        if (m_image.find(_T("%d")) != tstring::npos)
        {
            tstring dest;
            formatString(dest, m_image.c_str(), m_state);
            return dest;
        }
        return m_image;
    }

    void CButton::setStateColor(uint32 state, uint32 cr)
    {
        m_stateColor[state] = cr;
    }

    uint32 CButton::getStateColor(uint32 state)
    {
        return m_stateColor[state];
    }

    void CButton::setText(const tstring & text)
    {
        if (m_text == text) return;

        Widget::setText(text);

        m_textSprite->setText(m_text, m_fontPtr);
    }

    void CButton::setFont(const tstring & font)
    {
        Widget::setFont(font);
        m_fontPtr = FontMgr::instance()->getFont(m_font);

        m_textSprite->setText(m_text, m_fontPtr);
    }

    void CButton::setColor(uint32 color)
    {
        Widget::setColor(color);
        m_textSprite->setColor(color);
    }

    ///加载布局。
    void CButton::loadFromStream(LZDataPtr config)
    {
        Widget::loadFromStream(config);

        setTextAlign(config->readInt(L"textAlign", RelativeAlign::center));
        setPosMovable(config->readBool(L"posMovable", m_posMovable));
        setColorFadable(config->readBool(L"colorFadable", m_colorFadable));
    }

    ///保存布局
    void CButton::saveToStream(LZDataPtr config)
    {
        Widget::saveToStream(config);

        config->writeInt(L"textAlign", m_textAlign);
        config->writeBool(L"posMovable", m_posMovable);
        config->writeBool(L"colorFadable", m_colorFadable);
    }
}//namespace Lazy