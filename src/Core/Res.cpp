#include "stdafx.h"
#include "Res.h"

#include "Font2D.h"

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
    if (type == REST_FONT)
    {
        return cResFont::createObj();
    }
    
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


ResFontPtr cResMgr::getFont(const std::wstring & filename, bool useDefault/*=true*/)
{
    ResFontPtr pRes = get(filename, REST_FONT);
    if (!pRes && useDefault)
    {
        pRes = get(L"宋体/16", REST_FONT);
    }

    return pRes;
}

LPD3DXFONT cResMgr::getFontEx(const std::wstring & filename, bool useDefault/*=true*/)
{
    ResFontPtr pFont = getFont(filename, useDefault);
    if (!pFont) return nullptr;

    return pFont->getFont();
}
