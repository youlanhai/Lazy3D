
#include "stdafx.h"
#include "UIForm.h"

namespace Lazy
{

    //CPanel
    CForm::CForm(void)
        : m_bClip(false)
    {
        enableDrag(true);
        enableDrawSelf(true);
        enableLimitInRect(true);
        setSize(200, 100);
        setBgColor(0x7fffffff);
    }


    CForm::~CForm()
    {
    }

    void CForm::create(int id, const tstring & image, int x, int y)
    {
        setID(id);
        setPosition(x, y);
        setImage(image);

        if(m_texture) ajustToImageSize();
    }


    void CForm::render(IUIRender * pDevice)
    {
        CRect rect = getClientRect();
        localToGlobal(rect);

        if (m_bDrawSelf)
            pDevice->drawRect(rect, getBgColor(), m_texture);

        if (!m_bClip || m_rcClip.isEmpty())
        {
            IControl::render(pDevice);
            return;
        }

        //处理裁剪区域
        
        CRect clipRect = m_rcClip;
        clipRect.offset(rect.left, rect.top);

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
        IControl::setImage(image);
        m_texture = TextureMgr::instance()->get(image);

        if (m_texture) setBgColor(0xffffffff);
    }

    void CForm::setClipRect(const CRect & rc)
    {
        m_rcClip = rc;
    }

    void CForm::ajustToImageSize()
    {
        if (m_texture) setSize(m_texture->getWidth(), m_texture->getHeight());
        else setSize(0, 0);
    }



}//namespace Lazy