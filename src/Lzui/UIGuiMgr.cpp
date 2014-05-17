
#include "stdafx.h"
#include "UIGuiMgr.h"
#include "UIEdit.h"
#include "UIFactory.h"
#include "UIRender.h"

namespace Lazy
{
    //gui管理器

    CGUIManager* g_ptrGUIMgr = NULL;
    LZUI_API CGUIManager* getGUIMgr(void) ///获得gui管理器
    {
        return g_ptrGUIMgr;
    }
    /*static*/ CEdit* CGUIManager::s_pActiveEdit = NULL;


    //////////////////////////////////////////////////////////////////////////
    CGUIManager::CGUIManager()
        : m_render(nullptr)
        , m_hWnd(0)
        , m_hInstance(0)
    {
        setName(L"guimgr");
    }

    CGUIManager::CGUIManager(dx::Device * pDevice, HWND hWnd, HINSTANCE hInst)
    {
        init(pDevice, hWnd, hInst);
    }

    CGUIManager::~CGUIManager(void)
    {
        g_ptrGUIMgr = NULL;
        delete m_render;
    }

    void CGUIManager::addChildManage(RefPtr<IControl> child)
    {
        addToManagePool(child);
        addChild(child.get());
    }

    void CGUIManager::addToManagePool(RefPtr<IControl> ui)
    {
        m_managePool.push_back(ui);
    }

    void CGUIManager::destroy()
    {
        IControl::destroy();

        m_managePool.clear();
    }


    bool CGUIManager::init(dx::Device * pDevice, HWND hWnd, HINSTANCE hInst)
    {
        m_hWnd = hWnd;
        m_hInstance = hInst;
        g_ptrGUIMgr = this;

        //默认渲染器
        m_render = new GUIRender(pDevice, hWnd);
        m_render->setTextureShader(L"shader/ui.fx");
        m_render->setColorShader(L"shader/ui.fx");
        m_render->setFontShader(L"shader/font.fx");

        //注册默认ui工厂
        setUIFactory(new NormalUIFactor());

        //获取默认坐标
        GetCursorPos(&m_lastCursorPos);
        ScreenToClient(m_hWnd, &m_lastCursorPos);

        //启用默认字体
        setDefaultFont(L"def|17");

        //附加参数
        enableClickTop(false);
        enableChangeChildOrder(false);
        enableSelfMsg(false);
        enableLimitInRect(false);

        return true;
    }

    bool CGUIManager::processEvent(const SEvent & sysEvent)
    {
        SEvent event = sysEvent;
        if (event.eventType == EET_MOUSE_EVENT)
        {
            event.mouseEvent.x -= m_position.x;
            event.mouseEvent.y -= m_position.y;
        }

        if (event.isMouseEvent())
        {
            m_cursorPos.set(event.mouseEvent.x, event.mouseEvent.y);

            if (event.mouseEvent.event == EME_MOUSE_MOVE)
            {
                if (event.mouseEvent.isLeftDown())
                {
                    //拖拽
                    if (m_pSelected && m_pSelected->canDrag())
                    {
                        CPoint dp = m_cursorPos - m_lastCursorPos;
                        CPoint pt = m_cursorPos;
                        m_pSelected->globalToLocal(pt);

                        SEvent dragEvent;
                        fillGuiEvent(dragEvent, GuiMsg::mouseLeftDrag, dp.toLParam(), pt.toLParam());
                        m_pSelected->sendEvent(dragEvent);

                        m_lastCursorPos = m_cursorPos;
                        return true;
                    }
                }
            }

            bool ret = sendEvent(event);
            if (!ret)
            {
                if (event.mouseEvent.event == EME_MOUSE_MOVE) setFocus(NULL);
                else if (event.mouseEvent.event == EME_LMOUSE_UP) setSeleced(NULL);
            }

            m_lastCursorPos = m_cursorPos;
            return ret;
        }
        else if (event.isCharEvent())
        {
            if (s_pActiveEdit) return s_pActiveEdit->sendEvent(event);

            //只有edit能处理char消息，所以消息就没必要向后传递了
            return false;
        }
        else if (event.isKeyEvent())
        {
            //edit处理按键消息的优先级最高
            if (s_pActiveEdit) return s_pActiveEdit->sendEvent(event);

            //按键消息不传递给ui
            return false;
        }

        if (event.eventType == EET_SYS_EVENT) return false;

        return sendEvent(event);
    }

    void CGUIManager::render(dx::Device * pDevice)
    {
        m_render->renderBegin();

        render(m_render);

        m_render->renderEnd();
    }

    void CGUIManager::render(IUIRender * pDevice)
    {
        IControl::render(pDevice);
    }

    void CGUIManager::activeEdit(CEdit * pEdit)
    {
        s_pActiveEdit = pEdit;
    }

    void CGUIManager::unactiveEdit(CEdit * pEdit)
    {
        if (s_pActiveEdit == pEdit)
        {
            s_pActiveEdit = NULL;
        }
    }




}//namespace Lazy

