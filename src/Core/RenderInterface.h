
#pragma once

namespace Lazy
{

    ///可渲染对象接口
    class IRenderable
    {
    public:
        IRenderable(){}
        virtual ~IRenderable(){}

        ///画面渲染
        virtual void render(IDirect3DDevice9 * /*pDevice*/) {}

        ///逻辑更新
        virtual void update(float /*elapse*/) {}
    };

} // end namespace Lazy
