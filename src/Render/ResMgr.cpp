#include "StdAfx.h"
#include "ResMgr.h"

namespace Lazy
{
#if 0
    DxFontMgr::DxFontMgr()
    {

    }
    DxFontMgr::~DxFontMgr()
    {

    }

    dx::Font* DxFontMgr::load(const tstring & fname)
    {
        if (fname.empty()) return NULL;

        FontInfo font(fname);

        dx::Font* d3dfont = nullptr;
        UINT weight = font.bold ? FW_BOLD : FW_NORMAL;

        if (FAILED(D3DXCreateFont(
                       m_pd3dDevice,
                       font.height,
                       0,
                       weight,
                       1,
                       FALSE,
                       DEFAULT_CHARSET,
                       OUT_DEFAULT_PRECIS,
                       DEFAULT_QUALITY,
                       DEFAULT_PITCH | FF_DONTCARE,
                       font.name.c_str(),
                       &d3dfont)))
        {
            debugMessage(_T("ERROR %s font load faild!"), fname.c_str());
            return NULL;
        }
        return d3dfont;
    }
    void DxFontMgr::onLostDevice()
    {
        for (ResPool::iterator it = m_pool.begin(); it != m_pool.end(); ++it)
        {
            if (it->second)
            {
                it->second->OnLostDevice();
            }
        }
    }

    void DxFontMgr::onResetDevice()
    {
        for (ResPool::iterator it = m_pool.begin(); it != m_pool.end(); ++it)
        {
            if (it->second)
            {
                it->second->OnResetDevice();
            }
        }
    }
#endif

}//namespace Lazy