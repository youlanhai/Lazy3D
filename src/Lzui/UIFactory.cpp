#include "stdafx.h"
#include "UIFactory.h"
#include "UIControl.h"
#include "UIGuiMgr.h"
#include "UIImage.h"
#include "UILabel.h"
#include "UIEdit.h"
#include "UIForm.h"
#include "UIButton.h"
#include "UICheckBox.h"
#include "UISlidebar.h"
#include "UIEditorCtl.h"
#include "UIProxy.h"

namespace Lazy
{
    static UIFactoryPtr s_uiFactory;

    UIFactoryPtr uiFactory()
    {
        assert(s_uiFactory);
        return s_uiFactory;
    }

    void setUIFactory(UIFactoryPtr factory)
    {
        s_uiFactory = factory;
    }

    UIFactoryBase::UIFactoryBase()
    {
    }


    UIFactoryBase::~UIFactoryBase()
    {
    }


    NormalUIFactor::NormalUIFactor()
    {
#define UIREG(TYPE, CLASS) registerCreateFun(uitype::TYPE, CLASS::createSelf)

        UIREG(Control, IControl);
        UIREG(Label, CLabel);
        UIREG(Image, CImage);
        UIREG(Edit, CEdit);
        UIREG(Form, CForm);
        UIREG(Button, CButton);
        UIREG(Check, CCheckBox);
        UIREG(Slider, CSlider);
        UIREG(Slidebar, CSlidebar);
        UIREG(EditorCtl, CEditorCtl);
        UIREG(Proxy, UIProxy);

#undef UIREG
    }

    IControl * NormalUIFactor::create(int type)
    {
        std::map < int, UICreateFun>::iterator it = m_funMap.find(type);
        if (it == m_funMap.end())
        {
            throw(std::invalid_argument("NormalUIFactor::create - This ui type doesn't found!"));
            return nullptr;
        }

        return (it->second)();
    }

    void NormalUIFactor::release(PControl ptr)
    {

    }

    void NormalUIFactor::registerCreateFun(int type, UICreateFun fun)
    {
        m_funMap[type] = fun;
    }
}