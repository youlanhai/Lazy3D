﻿#pragma once

#include "UIWidget.h"

namespace Lazy
{
    ///ui工厂，创建ui
    class UIFactoryBase : public IBase
    {
    public:
        UIFactoryBase();
        virtual ~UIFactoryBase();

        virtual Widget* create(const tstring & type) = 0;
        virtual void release(Widget* ptr) = 0;
        virtual void registerCreateFun(const tstring & type, UICreateFun fun) = 0;
    };

    typedef RefPtr<UIFactoryBase> UIFactoryPtr;

    UIFactoryPtr uiFactory();
    void setUIFactory(UIFactoryPtr factory);


    class NormalUIFactor : public UIFactoryBase
    {
    public:
        NormalUIFactor();

        virtual Widget* create(const tstring & type) override;
        virtual void release(Widget* ptr);
        virtual void registerCreateFun(const tstring & type, UICreateFun fun);

    private:
        std::map<tstring, UICreateFun> m_funMap;
    };
}