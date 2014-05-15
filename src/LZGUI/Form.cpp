
#include "Form.h"

int distance2(POINT* a, POINT* b)
{
    int x = a->x - b->x;
    int y = a->y - b->y;
    return x*x + y*y;
}

float distance(POINT* a, POINT* b)
{
    return (float)sqrt((float)distance2(a,b));
}

CPosFade::CPosFade(void)
{
    m_vx = 0;
    m_vy = 0;
    m_fadeOver = 1;
    m_speed = 0.0f;
}

void CPosFade::reset(CPoint cur, CPoint dest, float speed/* = -1.0f*/)
{
    m_cur = cur;
    setDest(dest, speed);
}

void CPosFade::setDest(CPoint dest, float speed/* = -1.0f*/)
{
    m_dest = dest;
    if (speed > 0.0f)
    {
        m_speed = speed;
    }
    float t = distance(&m_dest, &m_cur)/m_speed;
    m_vx = (m_dest.x - m_cur.x)/t;
    m_vy = (m_dest.y - m_cur.y)/t;
    m_dirvx = m_dest.x > m_cur.x;
    m_dirvy = m_dest.y > m_cur.y;
    m_fadeOver = -1;
}

void CPosFade::update(float fElapse)
{
    if (m_fadeOver>=0 || m_dest == m_cur)
    {
        m_fadeOver =  1;
        return;
    }
    m_cur.x += (LONG)(m_vx*fElapse);
    m_cur.y += (LONG)(m_vy*fElapse);
    if(m_dest.x > m_cur.x != m_dirvx || m_dest.y > m_cur.y != m_dirvy)
    {
        m_fadeOver =  0;
        m_cur = m_dest;
    }
}

//////////////////////////////////////////////////////////////////////////
CForm::CForm()
{

}

CForm::CForm(int id, PControl parent, LPCSTR image, int x, int y, int w, int h)
{
    create(id, parent, image, x, y, w, h);
}

CForm::~CForm()
{
    m_image = NULL;
}

void CForm::create(int id, PControl parent, LPCSTR image, int x, int y, int w, int h)
{
    m_image = new CImage(-1, this, image, 0, 0, w, h);
    __super::create(id, parent, x, y);
    setSize(w, h);
    setImage(image);
    enableDrag(true);
    enableClickTop(true);
}


UINT CForm::preProcessMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
    return IControl::preProcessMsg(msg, wParam, lParam);
}

UINT CForm::messageProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN)
    {
        active();
    }
    UINT result = CPanel::messageProc(msg, wParam, lParam);
    if (result)
    {
        return result;
    }
    return IControl::messageProc(msg, wParam, lParam);
}


//设置背景色
void CForm::setBackColor(COLORREF cr)
{ 
    __super::setBackColor(cr);
    m_image->setColor(cr);
}

void CForm::setImage(stdstring img)
{
    __super::setImage(img);
    m_image->setImage(img);
}

