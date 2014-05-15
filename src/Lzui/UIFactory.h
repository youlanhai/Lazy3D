#pragma once

namespace Lazy
{
    ///ui工厂，创建ui
    class UIFactoryBase : public IBase
    {
    public:
        UIFactoryBase();
        virtual ~UIFactoryBase();

        virtual IControl* create(int type) = 0;
        virtual void release(PControl ptr) = 0;
        virtual void registerCreateFun(int type, UICreateFun fun) = 0;
    };

    typedef RefPtr<UIFactoryBase> UIFactoryPtr;

    UIFactoryPtr uiFactory();
    void setUIFactory(UIFactoryPtr factory);


    class NormalUIFactor : public UIFactoryBase
    {
    public:
        NormalUIFactor();

        virtual IControl* create(int type) override;
        virtual void release(PControl ptr);
        virtual void registerCreateFun(int type, UICreateFun fun);

    private:
        std::map < int, UICreateFun> m_funMap;
    };
}