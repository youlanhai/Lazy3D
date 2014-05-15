//D3DTimer.cpp 
#include "stdafx.h"
#include "FPS.h"

#include "Res.h"
#include "../utility/Utility.h"

cFpsRender::cFpsRender()
{

}

cFpsRender::~cFpsRender()
{

}

void cFpsRender::render()
{
    Lazy::tstring str;
    formatString(str, L"fps:%.2f", getFps());

    CRect rc(0, 0, 500, 30);
    LPD3DXFONT font = getResMgr()->getFontEx(L"", true);
    if (font)
    {
        font->DrawText(NULL, str.c_str(),
            str.size(), &rc, DT_LEFT|DT_SINGLELINE, 0xffff0000);
    }
}