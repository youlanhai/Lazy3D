//D3DTimer.cpp
#include "stdafx.h"
#include "FPS.h"

#include "../utility/Utility.h"
#include "../Lzui/Lzui.h"

namespace Lazy
{

    FpsRender::FpsRender()
    {
        GUIMgr *p = getGUIMgr();
        m_label = p->createWidgetT<Label>();
        m_label->setPosition(10, 10);
    }

    FpsRender::~FpsRender()
    {
    }

    void FpsRender::render()
    {
        if (m_label)
        {
            tstring str;
            formatString(str, L"fps:%.2f", getFps());
            m_label->setText(str);
        }
    }

    void FpsRender::show(bool show)
    {
        if (m_label) m_label->setVisible(show);
    }

    bool FpsRender::visible() const
    {
        if (m_label) return m_label->getVisible();
        return false;
    }

    void FpsRender::toggle()
    {
        if (m_label) m_label->setVisible(m_label->getVisible());
    }

} // end namespace Lazy
