#include "stdafx.h"
#include "UIEditorCtl.h"
#include "../Render/RenderDevice.h"

namespace Lazy
{

    int whatDragType(const CRect & rc, const CPoint & pt, int delta)
    {
        if (!rc.isIn(pt)) return DragType::none;

        int type = DragType::none;

        if (rc.width() >= delta * 2)
        {
            if (pt.x - rc.left <= delta) type |= DragType::left;
            else if (rc.right - pt.x <= delta) type |= DragType::right;
        }

        if (rc.height() >= delta * 2)
        {
            if (pt.y - rc.top <= delta) type |= DragType::top;
            else if (rc.bottom - pt.y <= delta) type |= DragType::bottom;
        }

        if (type == DragType::none) type = DragType::center;
        return type;
    }


    CEditorCtl::CEditorCtl()
        : m_dragType(0)
        , m_edgeSize(10)
    {
        setSize(40, 40);
        enableDrag(true);

        m_color = 0xff777777;
        m_bgColor = 0xff00ff00;
    }


    CEditorCtl::~CEditorCtl()
    {
    }

    void CEditorCtl::update(float fElapse)
    {

    }

    void CEditorCtl::render(IUIRender * pDevice)
    {
        if (!m_bVisible) return;

        CRect rc = getClientRect();
        localToGlobal(rc);

        //drawRect(pDevice, rc, getBgColor());
        pDevice->drawRectFrame(rc, 1, m_bgColor);
        rc.expand(-m_edgeSize, -m_edgeSize);
        pDevice->drawRectFrame(rc, 1, m_color);
    }

    bool CEditorCtl::onEvent(const SEvent & event)
    {
        if (event.isMouseEvent() && event.mouseEvent.event == EME_LMOUSE_UP)
        {
            m_dragType = DragType::none;
        }

        return Widget::onEvent(event);
    }

    void CEditorCtl::onDrag(const CPoint & dp, const CPoint & pt)
    {
        if (m_dragType == DragType::none)
        {
            m_dragType = whatDragType(getClientRect(), pt, m_edgeSize);
            debugMessage(_T("INFO: drag type %d"), m_dragType);

            if (m_dragType == DragType::none) return;
        }

        if (m_dragType == DragType::center)
        {
            Widget::onDrag(dp, pt);
        }
        else
        {
            if (m_dragType & DragType::left)
            {
                int right = m_position.x + m_size.cx;

                m_size.cx -= dp.x;
                if (m_size.cx < 1)  m_size.cx = 1;
                m_position.x = right - m_size.cx;

            }
            else if (m_dragType & DragType::right)
            {
                m_size.cx += dp.x;
                if (m_size.cx < 1) m_size.cx = 1;
            }

            if (m_dragType & DragType::top)
            {
                int bottom = m_position.y + m_size.cy;

                m_size.cy -= dp.y;
                if (m_size.cy < 1) m_size.cy = 1;
                m_position.y = bottom - m_size.cy;
            }
            else if (m_dragType & DragType::bottom)
            {
                m_size.cy += dp.y;
                if (m_size.cy < 1) m_size.cy = 1;
            }
        }
    }
}