
#include "stdafx.h"
#include "UIButton.h"
#include "UISprite.h"

#include "../Lzpy/Lzpy.h"

namespace Lazy
{

#define CR_TIME 0.5f
    COLORREF buttonColor [] = { 0xaf808080, 0xafd0d0d0, 0xaf606060, 0xaf404040, 0xafa0a0a0 };

    //////////////////////////////////////////////////////////////////////////
    Button::Button(void)
        : m_colorFadable(true)
        , m_posMovable(true)
        , m_textSprite(new TextLineSprite())
        , m_textAlign(RelativeAlign::center)
    {
        memcpy(m_stateColor, buttonColor, sizeof(buttonColor));

        m_crFade.reset(m_stateColor[0], m_stateColor[0], CR_TIME);

        setBgColor(m_stateColor[0]);
        setBtnState(ButtonState::normal);
        setDragable(false);
        setTopable(true);
        setSize(80, 30);

        m_fontPtr = getDefaultFontPtr();
        m_textSprite->setColor(m_textColor);
    }

    Button::~Button(void)
    {
    }

    void Button::create(
        const tstring & caption,
        const tstring & image,
        int x,
        int y,
        int w,
        int h)
    {
        setPosition(x, y);
        setSize(w, h);
        setText(caption);
        setImage(image);
    }

    void Button::update(float fElapse)
    {
        if (m_colorFadable)
        {
            m_crFade.update(fElapse);
            setBgColor(m_crFade.getCurColor());
        }

        m_textSprite->update(fElapse);
    }

    void Button::render(IUIRender * pDevice)
    {
        CRect rc = getGlobalRect();

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

    void Button::renderImage(IUIRender * pDevice, CRect rc)
    {
        TexturePtr texture = TextureMgr::instance()->get(getRealImage());
        pDevice->drawRect(rc, getBgColor(), texture);
    }

    void Button::renderText(IUIRender * pDevice, CRect rc)
    {
        if (m_text.empty() || !m_fontPtr) return;

        CPoint pt(rc.left, rc.top);

        if (m_textAlign & RelativeAlign::hcenter)
        {
            pt.x += (m_size.x - m_textSprite->getSize().cx) / 2;
        }
        else if (m_textAlign & RelativeAlign::right)
        {
            pt.x += m_size.x - m_textSprite->getSize().cx;
        }

        if (m_textAlign & RelativeAlign::vcenter)
        {
            pt.y += (m_size.y - m_textSprite->getSize().cy) / 2;
        }
        else if (m_textAlign & RelativeAlign::bottom)
        {
            pt.y += m_size.y - m_textSprite->getSize().cy;
        }

        m_textSprite->render(pDevice, pt);
    }

    bool Button::onEvent(const SEvent & event)
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

                SEvent evt;
                fillGuiEvent(evt, GuiMsg::buttonClick, 0, 0);
                delegate.onEvent(evt);

                Lzpy::object_base(m_pScript).call_method_quiet("onButtonClick");
            }
        }


        return Widget::onEvent(event);
    }

    void Button::setBtnState(int state)
    {
        m_state = state;
        m_crFade.setDestColor(m_stateColor[state]);
    }

    tstring Button::getRealImage(void)
    {
        if (m_image.find(_T("%d")) != tstring::npos)
        {
            tstring dest;
            formatString(dest, m_image.c_str(), m_state);
            return dest;
        }
        return m_image;
    }

    void Button::setStateColor(uint32 state, uint32 cr)
    {
        m_stateColor[state] = cr;
    }

    uint32 Button::getStateColor(uint32 state)
    {
        return m_stateColor[state];
    }

    void Button::setText(const tstring & text)
    {
        if (m_text == text) return;

        m_text = text;
        m_textSprite->setText(m_text, m_fontPtr);
    }

    void Button::setFont(const tstring & font)
    {
        m_font = font;
        m_fontPtr = FontMgr::instance()->getFont(m_font);
        m_textSprite->setText(m_text, m_fontPtr);
    }

    void Button::setImage(const tstring & text)
    {
        m_image = text;
    }

    void Button::setBgColor(uint32 color)
    {
        m_bgColor = color;
        m_textSprite->setColor(color);
    }

    ///加载布局。
    bool Button::setProperty(LZDataPtr config, const tstring & key, LZDataPtr val)
    {
        if (key == L"text")
            setText(val->asText());
        else if (key == L"textAlign")
            setTextAlign(val->asInt());
        else if (key == L"posMovable")
            setPosMovable(val->asBool());
        else if (key == L"colorFadable")
            setColorFadable(val->asBool());
        else
            return Widget::setProperty(config, key, val);
        return true;
    }

}//namespace Lazy
