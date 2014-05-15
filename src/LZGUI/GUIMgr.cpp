#include "GUIMgr.h"

//gui管理器

GUIManagerBase* g_ptrGUIMgr = NULL;
LZGUI_API GUIManagerBase* getGUIMgr(void) ///获得gui管理器
{
    return g_ptrGUIMgr;
}

//////////////////////////////////////////////////////////////////////////
///构造函数。使用默认参数。noArgs无意义。
GUIManagerBase::GUIManagerBase(bool noArgs)
{
    init(getApp()->getDevice(), getApp()->getHWnd(), getApp()->getInstance());
}

GUIManagerBase::GUIManagerBase(IDirect3DDevice9 * pDevice, HWND hWnd, HINSTANCE hInst)
: CPanel(0, NULL, 0, 0)
{
    init(pDevice, hWnd, hInst);
}

GUIManagerBase::~GUIManagerBase(void)
{
    m_topMost = NULL;
    m_normal = NULL;
    m_bottomMost = NULL;
    m_mouseTracker.clear();
    m_dragTracker.clear();
    releasePool();
    SafeRelease(m_pRenderDevice);
}


bool GUIManagerBase::init(IDirect3DDevice9 * pDevice, HWND hWnd, HINSTANCE hInst)
{
    m_pRenderDevice = pDevice;
    m_pRenderDevice->AddRef();
    m_hWnd = hWnd;
    m_hInstance = hInst;
    g_ptrGUIMgr = this;

    GetCursorPos(&m_lastCursorPos);
    ScreenToClient(m_hWnd, &m_lastCursorPos);

    UINT msgFilter[] = {
        WM_LBUTTONDOWN, WM_LBUTTONUP, WM_LBUTTONDBLCLK,
        WM_RBUTTONDOWN, WM_RBUTTONUP, WM_RBUTTONDBLCLK,
        WM_MOUSEMOVE, WM_MOUSEWHEEL, WM_MOUSELEAVE, 
        WM_KEYDOWN, WM_KEYUP,};
#if 1
    for (int i=0; i<10; ++i)
    {
        m_msgFilter.push_back(msgFilter[i]);
    }
#else
    m_msgFilter.insert(m_msgFilter.end(), m_msgFilter, m_msgFilter+10);
#endif
    return true;
}

void GUIManagerBase::initLayotPanel(CPanel* top, CPanel* normal, CPanel* bottom)
{
    assert(top && normal && bottom && "GUIManagerBase::initLayotPanel");
    m_topMost = top;
    m_normal = normal;
    m_bottomMost = bottom;
    m_childrenEx.add(m_bottomMost);
    m_childrenEx.add(m_normal);
    m_childrenEx.add(m_topMost);
}

bool GUIManagerBase::messageFilter(UINT msg)
{
    for (size_t i=0; i<m_msgFilter.size(); ++i)
    {
        if (msg == m_msgFilter[i])
        {
            return true;
        }
    }
    return false;
}

UINT GUIManagerBase::messageProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (preProcessMsg(msg, wParam, lParam) != PRE_MSG_OK)
    {
        return 0;
    }
    if (!messageFilter(msg))
    {
        return 0;
    }
    if (msg == WM_MOUSEMOVE)
    {
        m_cursorPos.formatLParam(lParam);
    }
    else if(msg == WM_MOUSEWHEEL)
    {
        CPoint pt(lParam);
        getApp()->screenToClient(&pt);
        lParam = pt.toLParam();
    }
    else if (msg == WM_LBUTTONUP)
    {
        cancelAllDrag();
    }

    UINT result = CPanel::messageProc(msg,wParam,lParam);

    updateMouseTracker();

    if (msg == WM_MOUSEMOVE)
    {
        updateDragTracker();
        m_lastCursorPos = m_cursorPos;
    }

    return result;
}

PControl GUIManagerBase::getFocusCtl(void)
{
    return m_ptrFocusCtl;
}

void GUIManagerBase::setFocusCtl(PControl ptr)
{
    if (m_ptrFocusCtl)
    {
        m_ptrFocusCtl->onLostFocus();
    }
    m_ptrFocusCtl = ptr;
    if (m_ptrFocusCtl)
    {
        m_ptrFocusCtl->onFocus();
    }
}

CPanel* GUIManagerBase::topMostPanel(void)
{
    return m_topMost;
}

CPanel* GUIManagerBase::normalPanel(void)
{
    return m_normal;
}

CPanel* GUIManagerBase::bottomMostPanel(void)
{
    return m_bottomMost;
}

void GUIManagerBase::addChild(PControl pCtrl)
{
    m_normal->addChild(pCtrl);
}

PControl GUIManagerBase::getChild( int id )
{
    PControl ptr = m_normal->getChild(id);
    if (ptr)
    {
        return ptr;
    }
    ptr = m_bottomMost->getChild(id);
    if (ptr)
    {
        return ptr;
    }
    ptr = m_topMost->getChild(id);
    return ptr;
}

void GUIManagerBase::addFilterMsg(UINT msg)
{
    for (size_t i=0; i<m_msgFilter.size(); ++i)
    {
        if (msg == m_msgFilter[i])
        {
            return ;
        }
    }
    //     if (find(m_msgFilter.begin(), m_msgFilter.end(), msg))
    //     {
    //         return;
    //     }
    m_msgFilter.push_back(msg);
}
void GUIManagerBase::removeFilterMsg(UINT msg)
{
    m_msgFilter.erase(std::remove(m_msgFilter.begin(), m_msgFilter.end(), msg));
}

void GUIManagerBase::render(IDirect3DDevice9 * pDevice)
{
    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    pDevice->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );
    pDevice->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA);
    __super::render(pDevice);
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

/** 添加一个被跟踪者。用于标记当前正活动的控件，主要用于发出mouseLeave消息。*/
void GUIManagerBase::addMouseTracker(PControl ctrl)
{
    if (find(m_mouseTracker.begin(), m_mouseTracker.end(), ctrl) != m_mouseTracker.end())
    {
        return;
    }
    m_mouseTracker.push_back(ctrl);
}

void GUIManagerBase::updateMouseTracker(void)
{
    ControlIter it = m_mouseTracker.begin();
    while(it != m_mouseTracker.end())
    {
        IControl* ctrl = *it;
        CPoint pt = ctrl->globalToLocal(m_cursorPos);
        if (!ctrl->getClientRect().isIn(pt))
        {
            //必须先从列表删除控件，否则msgProc中的阻塞函数将会造成消息死循环。
            it = m_mouseTracker.erase(it); 
            //it = m_mouseTracker.begin();
            ctrl->mouseLeave();//将MouseIN设置为false
            ctrl->msgProc(WM_MOUSELEAVES, 0, pt.toLParam() );
        }
        else ++it;
    }
}
void GUIManagerBase::addDragTracker(PControl ctrl)
{
    if (find(m_dragTracker.begin(), m_dragTracker.end(), ctrl) != m_dragTracker.end())
    {
        return;
    }
    m_dragTracker.push_back(ctrl);
}

void GUIManagerBase::updateDragTracker(void)
{
    for (ControlIter it = m_dragTracker.begin(); 
        it != m_dragTracker.end(); ++it)
    {
        if ((*it)->canDrag() && (*it)->isLButtonDown())
        {
            (*it)->drag(m_cursorPos, m_lastCursorPos);
        }
    }
}

void GUIManagerBase::cancelAllDrag(void)
{
    for (ControlIter it = m_dragTracker.begin(); 
        it != m_dragTracker.end(); ++it)
    {
        (*it)->setLButtonDown(false);
        (*it)->onDragEnd();
    }
    m_dragTracker.clear();
}

///添加到管理池
void GUIManagerBase::addToPool(PControl pctrl)
{
    if (find(m_managePool.begin(), m_managePool.end(), pctrl) == m_managePool.end())
    {
        m_managePool.push_back(pctrl);
    }
}

///从管理池移除。仅从链表移除，不释放资源。
void GUIManagerBase::removeFromPool(PControl pctrl)
{
    m_managePool.remove(pctrl);
}

///释放pool里的资源。
void GUIManagerBase::releasePool(void)
{
    for (ControlIter it=m_managePool.begin();
        it!=m_managePool.end();
        ++it)
    {
        (*it)->setParent(NULL);
        delete (*it);
    }
    m_managePool.clear();
}
//////////////////////////////////////////////////////////////////////////

/** UI管理器。*/

/** 构造函数。*/
CGUIManager::CGUIManager(IDirect3DDevice9 * pDevice, HWND hWnd, HINSTANCE hInst)
    : GUIManagerBase(pDevice, hWnd, hInst)
{
    m_topPanel = new CPanel(1, NULL, 0, 0); //< 最顶部控件
    m_normalPanel = new CPanel(2, NULL, 0, 0);   //< 正常控件
    m_bottomPanel = new CPanel(3, NULL, 0, 0);//< 最底层控件
    initLayotPanel(m_topPanel.get(), m_normalPanel.get(), m_bottomPanel.get());
}

CGUIManager::~CGUIManager(void)
{
    
}
