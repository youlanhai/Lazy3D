
#include "CursorCamera.h"

CCursorCamera* g_pCamera_ = NULL;
CCursorCamera* getCamera(void)
{
    return g_pCamera_;
}
//////////////////////////////////////////////////////////////////////////
CCursorCamera::CCursorCamera(HWND hWnd, LPDIRECT3DDEVICE9 pDevice, CameraType type /*= THIRD*/)
    : CCamera(pDevice, type)
    , m_hWnd(hWnd)
{
    g_pCamera_ = this;
    m_bMouseDown = false;
    m_bCurShow = true;
    m_draged = false;
    m_height = 60.0f;
    m_realDistToPlayer = m_fDistToPlayer;

    float angleX = D3DX_PI/600*0.5f;
    setCurRoSpeed(angleX);
}


CCursorCamera::~CCursorCamera(void)
{
}


UINT CCursorCamera::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg==WM_LBUTTONDOWN || msg==WM_RBUTTONDOWN)
    {
        m_bMouseDown = true;
        m_ptDown.x = LOWORD(lParam);
        m_ptDown.y = HIWORD(lParam);
        m_draged = false;
    }
    else if(msg==WM_LBUTTONUP || msg==WM_RBUTTONUP)
    {
        m_bMouseDown = false;
        if (!m_bCurShow)
        {
            showCursor(true);
            return 1;
        }
    }
    else if (msg == WM_MOUSEMOVE)
    {
        if(m_bMouseDown)
        {
            POINT pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);
            drag(pt);   
            return 1;
        }
    }
    else if (msg == WM_MOUSELEAVE)
    {
        m_bMouseDown = false;
    }
    else if (msg == WM_MOUSEWHEEL)
    {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        float dt = (m_distMax - m_distMin)/40.0f;
        if (zDelta < 0)
        {
            m_fDistToPlayer += dt;
        }
        else if(zDelta > 0)
        {
            m_fDistToPlayer -= dt;
        }
        correctDist();
        return 1;
    }
    return 0;
}

void CCursorCamera::drag(POINT pt)
{
    if (pt.x == m_ptDown.x && pt.y == m_ptDown.y)
    {
        return;
    }
    POINT dp;
    dp.x = pt.x - m_ptDown.x;
    dp.y = pt.y - m_ptDown.y;

    rotYaw( dp.x*m_curSpeedX );
    rotPitch( dp.y*m_curSpeedY);

    showCursor(false);

    pt = m_ptDown;
    ClientToScreen(m_hWnd, &pt);
    SetCursorPos(pt.x, pt.y);

    m_draged = true;
}


void CCursorCamera::update(float fElapse)
{
    __super::update(fElapse);

    if (m_realDistToPlayer > 2*m_distMax)
    {
        m_realDistToPlayer = 2*m_distMax;
    }
    
    //左右旋转
    if(IS_KEY_DOWN('A'))
    {
        //rotYaw(-3.14f*fElapse);
        moveRight(false);
    }
    else if(IS_KEY_DOWN('D'))
    {
        //rotYaw(3.14f*fElapse);
        moveRight(true);
    }
    //前进后退
    if(IS_KEY_DOWN('W'))
    {
        moveLook(true);
    }
    else if(IS_KEY_DOWN('S'))
    {
        moveLook(false);
    }
}


void CCursorCamera::setCurRoSpeed(float speed)
{
    m_curSpeedX = speed;
    m_curSpeedY = speed;

}

void CCursorCamera::showCursor(bool show)
{
    if (show == m_bCurShow)
    {
        return;
    }
    m_bCurShow = show;
    ShowCursor(show);
}
