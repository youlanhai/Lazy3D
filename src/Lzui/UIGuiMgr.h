#pragma once

#include "UIControl.h"

namespace Lazy
{
    class GUIRender;
    class CEdit;

    /** UI管理器基类。*/
    class LZUI_API CGUIManager : public Widget
    {
    public:
        MAKE_UI_HEADER(CGUIManager)


        CGUIManager();
        CGUIManager(dx::Device * pDevice, HWND hWnd, HINSTANCE hInst);
        virtual ~CGUIManager(void);

        virtual void render(IUIRender * pDevice) override;
        virtual void render(dx::Device * pDevice);

        ///激活edit控件。全局只能有一个edit是激活状态的。
        void activeEdit(CEdit * pEdit);
        void unactiveEdit(CEdit * pEdit);
        bool isEditActive(const CEdit *pEdit) const { return pEdit == s_pActiveEdit; }

        ///处理事件。由操作系统调用。
        bool processEvent(const SEvent & event);

        CPoint getCursorPos(void) { return m_cursorPos; }
        CPoint getLastCursorPos(void) { return m_lastCursorPos; }
        HWND getHWnd(void) { return m_hWnd; }
        HINSTANCE getInstance(void) { return m_hInstance; }
        GUIRender * getUIRender() { return m_render; }

    protected:

        bool init(dx::Device * pDevice, HWND hWnd, HINSTANCE hInst);

        GUIRender       *m_render;
        HWND            m_hWnd;
        HINSTANCE       m_hInstance;
        CPoint          m_cursorPos;    //< 当前鼠标位置
        CPoint          m_lastCursorPos;    //< 上一次鼠标位置

        static CEdit*   s_pActiveEdit;
    };


}//namespace Lazy