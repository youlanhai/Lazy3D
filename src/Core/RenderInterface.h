
#pragma once

namespace Lazy
{

///可渲染对象接口
    struct LZDLL_API IRenderObj
    {
        ///画面渲染
        virtual void render(IDirect3DDevice9 *) = 0;

        ///逻辑更新
        virtual void update(float) = 0;
    };

    typedef IRenderObj IRenderable;


    struct LZDLL_API IUpdate : public IRenderObj
    {
        virtual void render(IDirect3DDevice9 *) {}
    };


    struct LZDLL_API IRender : public IRenderObj
    {
        virtual void update(float) {}
    };

} // end namespace Lazy
