
#include "stdafx.h"
#include "UIImage.h"

namespace Lazy
{

    Image::Image()
        : m_color(0xffffffff)
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
        CRect rc = getGlobalRect();
        pDevice->drawRect(rc, m_color, m_texture);

        Widget::render(pDevice);
    }

    void Image::setImage(const tstring & image)
    {
        m_image = image;
        m_texture = TextureMgr::instance()->get(m_image);
    }


    bool Image::setProperty(LZDataPtr config, const tstring & key, LZDataPtr val)
    {
        if (key == L"image")
            setImage(val->asString());
        else if (key == L"color")
            setColor(val->asHex());
        else
            return Widget::setProperty(config, key, val);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

}//namespace Lazy