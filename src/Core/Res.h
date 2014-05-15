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


typedef RefPtr<class cResFont> ResFontPtr;

class LZDLL_API cResMgr : public cRenderResMgr
{
public:
    
    ResFontPtr getFont(const std::wstring & filename, bool useDefault = true);
    LPD3DXFONT getFontEx(const std::wstring & filename, bool useDefault = true);

    cResMgr();
};


LZDLL_API cResMgr* getResMgr(void);