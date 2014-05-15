#pragma once

#include "camera.h"

/*  玩家（player）控制相机旋转规则。
    1.第一人称，相机完全有玩家控制。
    2.自由模式，相机完全“自己”控制。
    3.第3人称，根据需要，进行玩家和自己切换。
*/

/** 鼠标控制摄像机。*/
class CCursorCamera :  public CCamera
{
public:
    CCursorCamera(HWND hWnd, LPDIRECT3DDEVICE9 pDevice, CameraType type = THIRD);

    ~CCursorCamera(void);

    virtual UINT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    virtual void update(float elapse);

    /** 设置鼠标旋转速度。单位为：弧度/像素*/
    void setCurRoSpeed(float speed);
    float getRotateSpeed(){ return m_curSpeedX; }

    void showCursor(bool show);

    virtual void drag(POINT pt);

    bool isDraged(void){ return m_draged; }

    void setHeight(float h){ m_height = h; }

protected:
    HWND    m_hWnd;
    bool    m_bMouseDown;
    POINT   m_ptDown;
    float   m_curSpeedX;
    float   m_curSpeedY;
    bool    m_bCurShow;
    bool    m_draged;
    float   m_height;
    float   m_realDistToPlayer;

};

CCursorCamera* getCamera(void);