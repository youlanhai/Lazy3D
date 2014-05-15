
#include "Control.h"
#include "Form.h"
#include "GUIMgr.h"



//////////////////////////////////////////////////////////////////////////


void UIVertex::set(float x_, float y_, float z_, float h_, D3DCOLOR cr_, float tu_, float tv_)
{
    x = x_;
    y = y_;
    z = z_;
    h = h_;
    color = cr_;
    tu = tu_;
    tv = tv_;
}
DWORD UIVertex::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
int UIVertex::SIZE = sizeof(UIVertex);
//////////////////////////////////////////////////////////////////////////

void UIVertex_UNTEX::set(float x_, float y_, float z_, float h_, D3DCOLOR cr_)
{
    x = x_;
    y = y_;
    z = z_;
    h = h_;
    color = cr_;
}
DWORD UIVertex_UNTEX::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
int UIVertex_UNTEX::SIZE = sizeof(UIVertex_UNTEX);
//////////////////////////////////////////////////////////////////////////

CColorFade::CColorFade(void)
{
    m_source = NULL;
    m_crCur = m_crDest = 0;
    m_time = m_elapse = 0.0f;
    getApp()->addUpdater(this);

}
void CColorFade::reset(COLORREF crCur, COLORREF crDest, float time/*=-1.0f*/)
{
    m_crCur = crDest;
    m_time = time;
    setDestColor(crDest);
    getApp()->addUpdater(this);
}

void CColorFade::update(float fElapse)
{
    if ( (m_elapse <= 0.0f) || (m_crCur == m_crDest) )
    {
        return;
    }
    float v = 0.0f;
    BYTE *ps = (BYTE*)&m_crCur;
    for (int i=0; i<3; ++i)
    {
        v = ps[i] + m_v[i]*fElapse;
        if (v<0.0f)
        {
            v = 0.0f;
        }
        else if (v>255.0f)
        {
            v = 255.0f;
        }
        ps[i] = BYTE(v);
    }
    m_elapse -= fElapse;
    if (m_source)
    {
        m_source->setBackColor(m_crCur);
    }
}

void CColorFade::setDestColor(COLORREF cr)
{
    m_crDest = cr;
    m_elapse = m_time;
    BYTE *pd = (BYTE*)&cr;
    BYTE *ps = (BYTE*)&m_crCur;
    for (int i=0; i<3; ++i)
    {
        m_v[i] = (pd[i] - ps[i])/m_time;
    }
}

//////////////////////////////////////////////////////////////////////////
IControl::IControl(void)
{
    m_id = 0;					//编号。
    m_crBackColor = 0xff000000;	//前景色
    m_crColor = 0xffffffff;	//背景色
    m_alpha = 0x7f;
    m_fontName = "宋体";
    m_bVisible = true;		//窗体是否可见
    m_bEnable = true;		//是否可用
    m_bDrag = false;        //是否可拖拽
    m_bMouseIn = false;
    m_bTrackMouse = false;
    m_bLButtonDown = false;
    m_bHandleMsg = true;
    m_bClickTop = false;
    m_parent = NULL;

    MEMORY_CHECK_CONS;
}

IControl::~IControl(void)
{
    clearChildren();

    if (m_parent)
    {
        m_parent->removeChild(this);
    }

    MEMORY_CHECK_DEST;
}

UINT IControl::preProcessMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if ( !isVisible() || !canHandleMsg())
    {
        return PRE_MSG_NO;//不可见，不处理。
    }
    CPoint pt(lParam);
    if (!(getClientRect().isIn(pt)))
    {
        return PRE_MSG_NO;//不在区域，不处理。
    }
    if( !isEnable())
    {
        return PRE_MSG_BREAK;//消息拦截，不再传递下去。
    }
    return PRE_MSG_OK;//可以处理。
}

UINT IControl::messageProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    trackMouse(CPoint(lParam));

    if (msg == WM_LBUTTONDOWN )
    {
        m_bLButtonDown = true;
    }
    else if (msg == WM_MOUSEMOVE)
    {
        if (m_bLButtonDown && m_bDrag)
        {
            getGUIMgr()->addDragTracker(this);
        }
    }

    msgProc(msg, wParam, lParam);

    if (msg == WM_LBUTTONUP)
    {
        m_bLButtonDown = false;
    }
    return 1;
}

void IControl::msgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    CPoint pt(lParam);
    if (msg == WM_LBUTTONDOWN )
    {
        onMouseLButton(true, pt);
    }
    else if (msg == WM_LBUTTONUP)
    {
        if (m_bLButtonDown)
        {
            onMouseLButton(false, pt);
        }
    }
    else if (msg == WM_LBUTTONDBLCLK)
    {
        onMouseDBClick(true, pt);
    }
    else if (msg == WM_RBUTTONDBLCLK)
    {
        onMouseDBClick(false, pt);
    }
    else if (msg == WM_RBUTTONDOWN )
    {
        onMouseRButton(true, pt);
    }
    else if (msg == WM_RBUTTONUP)
    {
        onMouseRButton(false, pt);
    }
    else if (msg == WM_MOUSEMOVE)
    {
        onMouseMove(pt, CPoint());
    }
    else if (msg == WM_MOUSEWHEEL)
    {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        onMouseWheel(zDelta, pt);
    }
    else if (msg == WM_MOUSEENTERS)
    {
        onMouseEnter(pt);
    }
    else if(msg == WM_MOUSELEAVES)
    {
        onMouseLeave(pt);
    }
    else if (msg == WM_KEYDOWN)
    {
        onKey(true, wParam,lParam);
    }
    else if (msg == WM_KEYUP)
    {
        onKey(false, wParam, lParam);
    }
    onMessageProc(msg, wParam, lParam);
}

void IControl::onMessageProc(UINT msg, WPARAM wParam, LPARAM lParam)
{

}

void IControl::trackMouse(CPoint pt)
{
    if (m_bTrackMouse)
    {
        if (!m_bMouseIn)
        {
            m_bMouseIn = true;
            //onMouseEnter(pt);
            msgProc(WM_MOUSEENTERS, 0, pt.toLParam());
            getGUIMgr()->addMouseTracker(this);
        }
    }
}

void IControl::onKey(bool isDown, UINT key, UINT param)
{

}

void IControl::onMouseMove(CPoint pt, CPoint old)
{
    
}

void IControl::onMouseEnter(CPoint pt)
{

}

void IControl::onMouseLeave(CPoint pt)
{ 
}

void IControl::onMouseLButton(bool isDown, CPoint pt)
{
    
}

void IControl::onMouseRButton(bool isDown, CPoint pt)
{
}

void IControl::onMouseDBClick(bool left, CPoint pt)
{

}

void IControl::onMouseWheel(int zDelta, CPoint pt)
{

}

void IControl::onFocus()
{

}

void IControl::onLostFocus()
{

}


void IControl::show(bool bShow)
{
    m_bVisible = bShow;
    onShow(bShow);
}

void IControl::toggle(void)
{
    show(!isVisible());
}

void IControl::onShow(bool show)
{
    if (show)
    {
        active();
    }
}

void IControl::onClose(void)
{

}

void IControl::drag(CPoint cur, CPoint old)
{
    CPoint pt(cur - old);
    pt += m_position;
    setPosition(pt.x, pt.y);
}

void IControl::onDragEnd(void)
{

}

void IControl::active(void)
{
    if (!canClickTop())
    {
        return ;
    }
    if (m_parent)
    {
        if(!m_parent->isChildActive(this))
        {
            PControl parent = m_parent;
            parent->removeChild(this);
            parent->addChild(this);
            onActive();
        }
    }
}

void IControl::onActive(void)
{

}

//初始化窗体
void IControl::init(int id, PControl parent, stdstring caption, int x, int y)
{
    m_id = id;
    m_caption = caption;
    m_position.set(x,y);
    m_parent = parent;
    if (m_parent)
    {
        m_parent->addChild(this);
    }
}

CRect IControl::getControlRect(void)
{
    return CRect(m_position, m_size);
}

CRect IControl::getClientRect(void)
{
    return CRect(0, 0, m_size.cx, m_size.cy);
}

//坐标系转换
CPoint IControl::localToParent(CPoint pt)
{
    return pt+m_position;
}

CRect IControl::localToParent(CRect rc)
{
    CPoint pt = m_position;
    CRect rcNew = rc;
    rcNew.left += pt.x;
    rcNew.right += pt.x;
    rcNew.top += pt.y;
    rcNew.bottom += pt.y;
    return rcNew;
}

CPoint IControl::localToGlobal(CPoint pt)
{
    pt = localToParent(pt);
    if (!m_parent)
    {
        return pt;
    }
    return m_parent->localToGlobal(pt);
}
CRect IControl::localToGlobal(CRect rc)
{
    rc = localToParent(rc);
    if (!m_parent)
    {
        return rc;
    }
    return m_parent->localToGlobal(rc);
}


CPoint IControl::parentToLocal(CPoint pt)
{
    return pt-m_position;
}

CRect IControl::parentToLocal(CRect rc)
{
    CPoint pt = m_position;
    CRect rcNew = rc;
    rcNew.left -= pt.x;
    rcNew.right -= pt.x;
    rcNew.top -= pt.y;
    rcNew.bottom -= pt.y;
    return rcNew;
}

CPoint IControl::globalToLocal(CPoint pt)
{
    if (m_parent)
    {
        pt = m_parent->globalToLocal(pt);
    }
    return parentToLocal(pt);
}

CRect IControl::globalToLocal(CRect rc)
{
    if (m_parent)
    {
       rc = m_parent->globalToLocal(rc);
    }
    return parentToLocal(rc);
}

///清除子控件
void IControl::clearChildren()
{

}
//////////////////////////////////////////////////////////////////////////
