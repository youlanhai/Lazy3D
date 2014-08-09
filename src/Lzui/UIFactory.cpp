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

    //////////////////////////////////////////////////////////////////////
    ///
    //////////////////////////////////////////////////////////////////////

    NormalUIFactor::NormalUIFactor()
    {
#define UIREG(CLASS) registerCreateFun(#CLASS, CLASS::createSelf)

        UIREG(Widget);
        UIREG(Label);
        UIREG(Image);
        UIREG(Edit);
        UIREG(Window);
        UIREG(Button);
        UIREG(CheckBox);
        UIREG(Slider);
        UIREG(Slidebar);
        UIREG(EditorCtl);
        UIREG(UIProxy);

#undef UIREG
    }

    Widget * NormalUIFactor::create(const std::string & type)
    {
        std::map<std::string, UICreateFun>::iterator it = m_funMap.find(type);
        if (it == m_funMap.end())
        {
            LOG_ERROR(L"NormalUIFactor::create - ui type '%S' doesn't found!",
                type.c_str());
            return nullptr;
        }

        return (it->second)();
    }

    void NormalUIFactor::release(Widget* ptr)
    {

    }

    void NormalUIFactor::registerCreateFun(const std::string & type, UICreateFun fun)
    {
        m_funMap[type] = fun;
    }
}