#pragma once

#include "UIButton.h"

namespace Lazy
{
    class LZUI_API CheckBox : public Button
    {
    public:
        MAKE_UI_HEADER(CheckBox, Button);

        CheckBox(void);
        ~CheckBox(void);

        void setCheck(bool check);
        bool getCheck(void) const { return m_bCheck; }

        virtual bool onEvent(const SEvent & event) override;
        virtual void setBtnState(int state);

    protected:

        virtual void renderImage(IUIRender * pDevice, CRect rc) override;
        virtual void renderText(IUIRender * pDevice, CRect rc) override;

    private:
        bool m_bCheck;
    };
    typedef RefPtr<CheckBox> CheckBoxPtr;

}// end namespace Lazy

