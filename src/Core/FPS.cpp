//D3DTimer.cpp
#include "stdafx.h"
#include "FPS.h"

#include "../utility/Utility.h"
#include "../Lzui/Lzui.h"

namespace Lazy
{

    cFpsRender::cFpsRender()
    {
#if 0
        m_label = new CLabel();
        m_label->setPosition(10, 10);

        CGUIManager *p = getGUIMgr();
        if(p) p->addChild(m_label.get());
#endif
    }

    cFpsRender::~cFpsRender()
    {
        m_label = nullptr;
    }

    void cFpsRender::render()
    {
        if (m_label)
        {
            tstring str;
            formatString(str, L"fps:%.2f", getFps());
            m_label->setText(str);
        }
    }

    void cFpsRender::show(bool show)
    {
        if (m_label) m_label->show(show);
    }

    bool cFpsRender::visible() const
    {
        if (m_label) return m_label->isVisible();
        return false;
    }

    void cFpsRender::toggle()
    {
        if (m_label) m_label->toggle();
    }

} // end namespace Lazy
