#include "stdafx.h"
#include "UICheckBox.h"
#include "UISprite.h"

#include "../Lzpy/Lzpy.h"

namespace Lazy
{

#define CR_SELECT   0x7fffffff

    CheckBox::CheckBox(void)
        : m_bCheck(false)
    {

    }


    CheckBox::~CheckBox()
    {
    }

    void CheckBox::setCheck(bool check)
    {
        m_bCheck = check;

        if (!m_bCheck)
        {
            setBtnState(ButtonState::normal);
        }
    }

    bool CheckBox::onEvent(const SEvent & event)
    {
        if (event.eventType == EET_MOUSE_EVENT && event.mouseEvent.event == EME_LMOUSE_UP)
        {
            setCheck(!m_bCheck);

            m_script.call_method_quiet("onButtonCheck", m_bCheck);
        }

        return Button::onEvent(event);
    }


    void CheckBox::setBtnState(int state)
    {
        if (m_state == ButtonState::down)
        {
            m_state = state;
            m_crFade.setDestColor(CR_SELECT);//(CR_SELECT);
        }
        else if (!m_bCheck)
        {
            m_crFade.setDestColor(m_stateColor[state]);
            m_state = state;
        }
    }


    void CheckBox::renderImage(IUIRender * pDevice, CRect rc)
    {
        tstring image = getRealImage();
        if (image.empty())
        {
            pDevice->drawRect(rc, getBgColor());
            if (m_bCheck)
            {
                CRect rc2;
                rc2.left = rc.left + rc.width() / 3;
                rc2.right = rc.left + rc.width() * 2 / 3;
                rc2.top = rc.top + rc.height() / 3;
                rc2.bottom = rc.top + rc.height() * 2 / 3;
                pDevice->drawRect(rc2, 0xffffffff);
            }
        }
        else
        {
            TexturePtr texture = TextureMgr::instance()->get(image);
            pDevice->drawRect(rc, getBgColor(), texture);
        }
    }

    void CheckBox::renderText(IUIRender * pDevice, CRect rc)
    {
        if (!m_text.empty())
        {
            CPoint pt;
            pt.x = rc.width() + 10;
            pt.y = (rc.height() - m_textSprite->getSize().cy) / 2;
            m_textSprite->render(pDevice, pt);
        }
    }

}// end namespace Lazy
