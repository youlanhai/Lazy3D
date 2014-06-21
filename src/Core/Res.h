#pragma once

#include "RenderRes.h"

namespace Lazy
{

    class LZDLL_API ResFactory : public IResFactory
    {
    public:
        ResFactory();
        ~ResFactory();

        /** 创建资源对象。*/
        virtual RenderResPtr createObj(int type) override;
    };


    class LZDLL_API ResMgrOld : public cRenderResMgr
    {
    public:

        ResMgrOld();
    };


    LZDLL_API ResMgrOld* getResMgr(void);

} // end namespace Lazy
