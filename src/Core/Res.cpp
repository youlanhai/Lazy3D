#include "stdafx.h"
#include "Res.h"


#include "../utility/MemoryCheck.h"

namespace Lazy
{

//////////////////////////////////////////////////////////////////////////
    ResFactory::ResFactory()
    {
    }

    ResFactory::~ResFactory()
    {
    }

    /** 创建资源对象。*/
    RenderResPtr ResFactory::createObj(int type)
    {
        throw(std::runtime_error("This res type doesn't supported!"));
        return NULL;
    }

//////////////////////////////////////////////////////////////////////////

    static ResMgrOld* g_pResMgr_ = NULL;
    LZDLL_API ResMgrOld* getResMgr(void)
    {
        return g_pResMgr_;
    }

///构造函数
    ResMgrOld::ResMgrOld()
    {
        g_pResMgr_ = this;
    }

} // end namespace Lazy
