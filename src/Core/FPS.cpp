//D3DTimer.cpp 
#include "stdafx.h"
#include "FPS.h"

#include "Res.h"
#include "../utility/Utility.h"

#include "../Lzui/Lzui.h"

cFpsRender::cFpsRender()
{
    m_label = new Lazy::CLabel();
    m_label->setPosition(10, 10);

    Lazy::CGUIManager *p = Lazy::getGUIMgr();
    if(p) p->addChild(m_label.get());
}

cFpsRender::~cFpsRender()
{
}

void cFpsRender::render()
{
    Lazy::tstring str;
    formatString(str, L"fps:%.2f", getFps());

    m_label->setText(str);
}

void cFpsRender::show(bool show)
{
    m_label->show(show);
}

bool cFpsRender::visible() const
{
    return m_label->isVisible();
}

void cFpsRender::toggle()
{
    m_label->toggle();
}