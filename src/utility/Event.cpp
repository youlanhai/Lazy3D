#include "stdafx.h"
#include "Event.h"


namespace Lazy
{

    namespace
    {

        struct messageMap
        {
            int group;
            UINT winMessage;
            UINT mMessage;
        };

        static messageMap mouseMap [] =
        {
            { 0, WM_LBUTTONDOWN, EME_LMOUSE_DOWN },
            { 1, WM_LBUTTONUP, EME_LMOUSE_UP },
            { 0, WM_RBUTTONDOWN, EME_RMOUSE_DOWN },
            { 1, WM_RBUTTONUP, EME_RMOUSE_UP },
            { 0, WM_MBUTTONDOWN, EME_MMOUSE_DOWN },
            { 1, WM_MBUTTONUP, EME_MMOUSE_UP },
            { 2, WM_LBUTTONDBLCLK, EME_LMOUSE_DOUBLE_CLICK },
            { 2, WM_RBUTTONDBLCLK, EME_RMOUSE_DOUBLE_CLICK },
            { 2, WM_MBUTTONDBLCLK, EME_MMOUSE_DOUBLE_CLICK },
            { 2, WM_MOUSEMOVE, EME_MOUSE_MOVE },
            { 3, WM_MOUSEWHEEL, EME_MOUSE_WHEEL },
            { -1, 0, 0 }  //结束标志
        };

        static int ClickCount = 0;
    }

    bool processWindowMsg(IEventReceiver * reciver, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        SEvent event;
        memset(&event, 0, sizeof(event));


        // handle grouped events
        messageMap * m = mouseMap;
        while (m->group >= 0 && m->winMessage != uMsg)
            ++m;

        if (m->group >= 0)//鼠标消息
        {
            if (m->group == 0)	// down
            {
                ClickCount++;
                SetCapture(hWnd);
            }
            else if (m->group == 1)	// up
            {
                ClickCount--;
                if (ClickCount < 1)
                {
                    ClickCount = 0;
                    ReleaseCapture();
                }
            }

            event.eventType = EET_MOUSE_EVENT;
            event.mouseEvent.x = getXLParam(lParam);
            event.mouseEvent.y = getYLParam(lParam);
            event.mouseEvent.event = (EMOUSE_EVENT)(m->mMessage);

            if (wParam & MK_LBUTTON) event.mouseEvent.buttonStates |= EMBSM_LEFT;
            if (wParam & MK_RBUTTON) event.mouseEvent.buttonStates |= EMBSM_RIGHT;
            if (wParam & MK_MBUTTON)  event.mouseEvent.buttonStates |= EMBSM_MIDDLE;
            if (wParam & MK_SHIFT) event.mouseEvent.buttonStates |= EMBSM_SHIFT;
            if (wParam & MK_CONTROL) event.mouseEvent.buttonStates |= EMBSM_CONTROL;

#if(_WIN32_WINNT >= 0x0500)
            if (wParam & MK_XBUTTON1) event.mouseEvent.buttonStates |= EMBSM_EXTRA1;
            if (wParam & MK_XBUTTON2) event.mouseEvent.buttonStates |= EMBSM_EXTRA2;
#endif
            event.mouseEvent.wheel = 0.f;

            // wheel
            if (m->group == 3)
            {
                POINT p; // fixed by jox
                p.x = 0; p.y = 0;
                ClientToScreen(hWnd, &p);
                event.mouseEvent.x -= p.x;
                event.mouseEvent.y -= p.y;
                event.mouseEvent.wheel = ((float)((short) HIWORD(wParam))) / (float)WHEEL_DELTA;
            }

        }
        else
        {
            switch (uMsg)
            {
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP:
                event.eventType = EET_KEY_EVENT;
                event.keyEvent.key = (EKEY_CODE) wParam;
                event.keyEvent.control = (0 != (GetAsyncKeyState(VK_CONTROL) & 0x8000));
                event.keyEvent.shift = (0 != (GetAsyncKeyState(VK_SHIFT) & 0x8000));
                event.keyEvent.down = (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN);
                break;

            case WM_CHAR:
                event.eventType = EET_CHAR_EVENT;
                event.charEvent.ch = wParam;
                break;

            default:
                event.eventType = EET_SYS_EVENT;
                event.sysEvent.message = uMsg;
                event.sysEvent.wparam = wParam;
                event.sysEvent.lparam = lParam;
                break;
            }
        }

        return reciver->onEvent(event);
    }
}