//d3dfont2d.cpp 
#include "stdafx.h"
#include "font2d.h"
#include "Res.h"

#include "../Render/Texture.h"
#include "../Render/RenderDevice.h"
#include "../Font/Font.h"

///构造函数
cResFont::cResFont()
{
    m_font = NULL;
}

cResFont::~cResFont()
{
    SafeRelease(m_font);
}

/** 创建自己。*/
IRenderRes* cResFont::createObj(void)
{
    return new cResFont();
}

/** 加载资源。*/
bool cResFont::load(const std::wstring & filename)
{
    if (filename.empty())
    {
        return false;
    }

    SafeRelease(m_font);

    Lazy::FontInfo font(filename);

    UINT weight = font.bold ? FW_BOLD : FW_NORMAL;

    if(FAILED(D3DXCreateFont(
        Lazy::rcDevice()->getDevice(),
        font.height,
        0,
        weight,
        1,
        FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        font.name.c_str(),
        &m_font)))
    {
        return false;
    }

    return true;
}
