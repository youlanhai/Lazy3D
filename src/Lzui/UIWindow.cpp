
#include "stdafx.h"
#include "UIWindow.h"

namespace Lazy
{
    Window::Window(void)
        : m_clipable(false)
    {
        setDragable(true);
        setTopable(true);
        setSize(200, 100);
    }

    Window::~Window()
    {
    }

    void Window::create(const tstring & image, int x, int y)
    {
        setPosition(x, y);
        setImage(image);

        if(m_texture) ajustToImageSize();
    }

    void Window::render(IUIRender * pDevice)
    {
        CRect rect = getGlobalRect();

        if (m_drawable)
            pDevice->drawRect(rect, 0xffffffff, m_texture);

        if (!m_clipable)
        {
            Widget::render(pDevice);
            return;
        }

        //处理裁剪区域

        CRect clipRect = rect;

        bool oldClipEnabled = pDevice->isClipEnable();
        CRect oldClipRect;
        pDevice->getClipRect(oldClipRect);

        if (oldClipEnabled)
        {
            clipRect.getIntersectRect(clipRect, oldClipRect);

            if (clipRect.isEmpty())
                return;//没有交集
        }

        pDevice->setClipEnalbe(true);
        pDevice->setClipRect(clipRect);

        m_children.render(pDevice);

        pDevice->setClipRect(oldClipRect);
        pDevice->setClipEnalbe(oldClipEnabled);
    }

    void Window::setImage(const tstring & image)
    {
        m_image = image;
        m_texture = TextureMgr::instance()->get(image);
    }

    const tstring & Window::getImage() const
    {
        return m_image;
    }

    void Window::ajustToImageSize()
    {
        if (m_texture) setSize(m_texture->getWidth(), m_texture->getHeight());
        else setSize(0, 0);
    }

    bool Window::setProperty(LZDataPtr config, const tstring & key, LZDataPtr val)
    {
        if (key == L"clipable")
            setClipable(val->asBool());
        else if (key == L"image")
            setImage(val->asString());
        else
            return Widget::setProperty(config, key, val);
        return true;
    }

}//namespace Lazy