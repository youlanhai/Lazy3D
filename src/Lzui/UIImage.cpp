
#include "stdafx.h"
#include "UIImage.h"

namespace Lazy
{

    CImage::CImage()
    {
        enable(false);
    }

    CImage::~CImage(void)
    {
    }

    void CImage::create(
        int id,
        const tstring &image,
        int x,
        int y,
        int w,
        int h)
    {
        m_id = id;
        m_position.set(x, y);
        m_size.set(w, h);
        enable(false);
        setImage(image);
    }

    void CImage::render(IUIRender * pDevice)
    {
        CRect rc = getClientRect();
        localToGlobal(rc);
        pDevice->drawRect(rc, m_bgColor, m_texture);
    }

    void CImage::setImage(const tstring & image)
    {
        Widget::setImage(image);

        m_texture = TextureMgr::instance()->get(m_image);
    }

//////////////////////////////////////////////////////////////////////////

}//namespace Lazy