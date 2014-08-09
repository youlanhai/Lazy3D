
#include "stdafx.h"
#include "UIImage.h"

namespace Lazy
{

    Image::Image()
    {
    }

    Image::~Image(void)
    {
    }

    void Image::create(
        const tstring &image,
        int x,
        int y,
        int w,
        int h)
    {
        setPosition(x, y);
        setSize(w, h);
        setImage(image);
    }

    void Image::render(IUIRender * pDevice)
    {
        CRect rc = getClientRect();
        localToGlobal(rc);
        pDevice->drawRect(rc, 0xffffffff, m_texture);

        Widget::render(pDevice);
    }

    void Image::setImage(const tstring & image)
    {
        m_image = image;
        m_texture = TextureMgr::instance()->get(m_image);
    }


    void Image::loadProperty(LZDataPtr root)
    {
        Widget::loadProperty(root);

        setImage(root->readString(L"image"));
    }

    void Image::saveProperty(LZDataPtr root)
    {
        Widget::saveProperty(root);

        if (!m_image.empty())
            root->writeString(L"image", m_image);
    }

    //////////////////////////////////////////////////////////////////////////

}//namespace Lazy