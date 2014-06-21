#include "stdafx.h"
#include "Res.h"


#include "../utility/MemoryCheck.h"

//////////////////////////////////////////////////////////////////////////
cResFactory::cResFactory()
{
}

cResFactory::~cResFactory()
{
}

/** 创建资源对象。*/
RenderResPtr cResFactory::createObj(int type)
{
    throw(std::runtime_error("This res type doesn't supported!"));
    return NULL;
}

//////////////////////////////////////////////////////////////////////////

static cResMgr* g_pResMgr_ = NULL;
LZDLL_API cResMgr* getResMgr(void)
{
    return g_pResMgr_;
}

///构造函数
cResMgr::cResMgr()
{
    g_pResMgr_ = this;
}
