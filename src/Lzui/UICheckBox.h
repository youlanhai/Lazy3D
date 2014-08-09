#pragma once

#include "UIButton.h"

namespace Lazy
{
    class LZUI_API CCheckBox : public CButton
    {
    public:
        MAKE_UI_HEADER(CCheckBox);

        CCheckBox(void);
        ~CCheckBox(void);

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
    typedef RefPtr<CCheckBox> CheckBoxPtr;

}// end namespace Lazy

