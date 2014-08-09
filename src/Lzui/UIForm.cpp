﻿
#include "stdafx.h"
#include "UIForm.h"

namespace Lazy
{
    CForm::CForm(void)
        : m_clipable(false)
    {
        setDragable(true);
        setTopable(true);
        setSize(200, 100);
    }

    CForm::~CForm()
    {
    }

    void CForm::create(const tstring & image, int x, int y)
    {
        setPosition(x, y);
        setImage(image);

        if(m_texture) ajustToImageSize();
    }

    void CForm::render(IUIRender * pDevice)
    {
        CRect rect = getClientRect();
        localToGlobal(rect);

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

    void CForm::setImage(const tstring & image)
    {
        m_image = image;
        m_texture = TextureMgr::instance()->get(image);
    }

    const tstring & CForm::getImage() const
    {
        return m_image;
    }

    void CForm::ajustToImageSize()
    {
        if (m_texture) setSize(m_texture->getWidth(), m_texture->getHeight());
        else setSize(0, 0);
    }

    void CForm::loadProperty(LZDataPtr root)
    {
        Widget::loadProperty(root);

        setClipable(root->readBool(L"clipable", false));
        setImage(root->readString(L"image"));
    }

    void CForm::saveProperty(LZDataPtr root)
    {
        Widget::saveProperty(root);

        if (m_clipable)
            root->writeBool(L"clipable", m_clipable);

        if (!m_image.empty())
            root->writeString(L"image", m_image);
    }

}//namespace Lazy