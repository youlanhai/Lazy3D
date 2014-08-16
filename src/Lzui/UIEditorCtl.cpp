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


    EditorCtl::EditorCtl()
        : m_dragType(0)
        , m_edgeSize(10)
        , m_color(0xff777777)
        , m_bgColor(0xff00ff00)
    {
        setSize(40, 40);
        setDragable(true);
    }

    EditorCtl::~EditorCtl()
    {
    }

    void EditorCtl::update(float fElapse)
    {

    }

    void EditorCtl::render(IUIRender * pDevice)
    {
        CRect rc = getGlobalRect();

        //drawRect(pDevice, rc, getBgColor());
        pDevice->drawRectFrame(rc, 1, m_bgColor);
        rc.expand(-m_edgeSize, -m_edgeSize);
        pDevice->drawRectFrame(rc, 1, m_color);
    }

    bool EditorCtl::onEvent(const SEvent & event)
    {
        if (event.isMouseEvent() && event.mouseEvent.event == EME_LMOUSE_UP)
        {
            m_dragType = DragType::none;
        }

        return Widget::onEvent(event);
    }

    void EditorCtl::onDrag(const CPoint & dp, const CPoint & pt)
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
                int right = m_position.x + m_size.x;

                m_size.x -= dp.x;
                if (m_size.x < 1)  m_size.x = 1;
                m_position.x = right - m_size.x;

            }
            else if (m_dragType & DragType::right)
            {
                m_size.x += dp.x;
                if (m_size.x < 1) m_size.x = 1;
            }

            if (m_dragType & DragType::top)
            {
                int bottom = m_position.y + m_size.y;

                m_size.y -= dp.y;
                if (m_size.y < 1) m_size.y = 1;
                m_position.y = bottom - m_size.y;
            }
            else if (m_dragType & DragType::bottom)
            {
                m_size.y += dp.y;
                if (m_size.y < 1) m_size.y = 1;
            }
        }
    }
}