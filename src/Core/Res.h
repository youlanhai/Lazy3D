#pragma once

#include "RenderRes.h"


class LZDLL_API cResFactory : public IResFactory
{
public:
    cResFactory();
    ~cResFactory();

    /** 创建资源对象。*/
    virtual RenderResPtr createObj(int type) override;
};


class LZDLL_API cResMgr : public cRenderResMgr
{
public:
    
    cResMgr();
};


LZDLL_API cResMgr* getResMgr(void);