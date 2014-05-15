#include "Scroll.h"

//////////////////////////////////////////////////////////////////////////
CSilider::CSilider()
{

}

CSilider::CSilider(int id, PControl parent, LPCSTR image, int x, int y, int w, int h)
{
    create(id, parent, image, x, y, w, h);
}


void CSilider::create(int id, PControl parent, LPCSTR image, int x, int y, int w, int h)
{
    __super::create(id, parent, "", image, x, y, w, h);
    m_bVertical = false;
    enableDrag(true);
    enablePosMove(false);
    setRangePos(0, 100);
    m_nStep = 1;
    m_lowPos = x;
}

void CSilider::setVertical(bool v)
{
    m_bVertical = v;
}

void CSilider::drag(CPoint cur, CPoint old)
{
    if (m_bVertical)
    {
        roll( cur.y-old.y );
    }
    else
    {
        roll( cur.x-old.x );
    }
}

void CSilider::onDragEnd(void)
{
    if (m_state!=BS_NORMAL && m_state!=BS_DISABLE)
    {
        setBtnState(BS_NORMAL);
    }
}

void CSilider::onMouseLeave(CPoint pt)
{
    if (m_state!=BS_DOWN && m_state!=BS_DISABLE)
    {
        m_crFade.setDestColor(m_stateColor[0]);
    }
}

void CSilider::onMouseWheel(int zDelta, CPoint pt)
{
    if (m_parent)
    {
        m_parent->onMouseWheel(zDelta, pt);
    }
}

float CSilider::getPos(void)
{
    if (m_bVertical)
    {
        return (m_position.y-m_lowPos)/float(m_highPos-m_size.cy);
    }
    else
    {
        return (m_position.x-m_lowPos)/float(m_highPos-m_size.cx);
    }
}

void CSilider::setPos(float percent)
{
    if (m_bVertical)
    {
        m_position.y = int(m_lowPos + (m_highPos-m_size.cy)*percent);
    }
    else
    {
        m_position.x = int(m_lowPos + (m_highPos-m_size.cx)*percent);
    }
}

void CSilider::setRangePos(int low, int high)
{
    m_lowPos = low;
    m_highPos = high;
}

void CSilider::setStep(float percent)
{
    if (percent<=0.0f || percent>=1.0f)
    {
        return;
    }
    m_nStep = int((m_highPos-m_lowPos) * percent);
}

void CSilider::dec(void)
{
    roll(-m_nStep);
}

void CSilider::inc(void)
{
    roll(m_nStep);
}

void CSilider::roll(int dt)
{
    if (m_bVertical)
    {
        m_position.y += dt;
        if (m_position.y > m_highPos-m_size.cy)
        {
            m_position.y = m_highPos-m_size.cy;
        }
        else if (m_position.y < m_lowPos)
        {
            m_position.y = m_lowPos;
        }
    }
    else
    {
        m_position.x += dt;
        if (m_position.x > m_highPos-m_size.cx)
        {
            m_position.x = m_highPos-m_size.cx;
        }
        else if (m_position.x < m_lowPos)
        {
            m_position.x = m_lowPos;
        }
    }

    if (m_parent)
    {
        m_parent->onScroll(m_id, getPos());
    }
}

//////////////////////////////////////////////////////////////////////////
CScroll::CScroll()
{

}

CScroll::CScroll(int id, PControl parent, LPCSTR image, int x, int y, int w, int h)
{
    create(id, parent, image, x, y, w, h);
}


void CScroll::create(int id, PControl parent, LPCSTR image, int x, int y, int w, int h)
{
    __super::create(id, parent, image, x, y, w, h);
    m_silider = new CSilider(1, this, "", 0, 0, h, h);//水平
    m_vertical = false;
    setVertical(true);
    setStep(0.05f);
    enableDrag(false);
    setBackColor(0xff000000);
}

void CScroll::setSiliderSize(int w, int h)
{

}
void CScroll::setVertical(bool vertical)
{
    if (m_vertical == vertical)
    {
        return;
    }
    m_vertical = vertical;
    m_silider->setVertical(vertical);
    CSize size = getSize();
    if (vertical)
    {
        m_size.cx = size.cy;
        m_size.cy = size.cx;
        m_image->setSize(m_size.cx/4, m_size.cy);
        m_image->setPosition(3*m_size.cx/8, 0);
        m_silider->setRangePos(0, m_size.cy);
    }
    else
    {
        m_size.cx = size.cy;
        m_size.cy = size.cx;
        m_image->setSize(m_size.cx, m_size.cy/4);
        m_image->setPosition(0, 3*m_size.cy/8);
        m_silider->setRangePos(0, m_size.cx);
    }
}
void CScroll::setForeImage(LPCSTR)
{
}

void CScroll::setRate(float percent)
{
    m_silider->setPos(percent);
}

float CScroll::getRate(void)
{
    return m_silider->getPos();
}

/** 设置步长。[0, 1.0]*/
void CScroll::setStep(float percent)
{
    m_silider->setStep(percent);
}


void CScroll::onMouseWheel(int zDelta, CPoint pt)
{
    if (zDelta < 0)
    {
        m_silider->inc();
    }
    else
    {
        m_silider->dec();
    }
}
void CScroll::onScroll(int id, float pos)
{
    if (m_parent)
    {
        m_parent->onScroll(m_id, pos);
    }
}
