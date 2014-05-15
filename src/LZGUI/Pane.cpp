#include "Panel.h"

struct PredIfIDEqual
{
    PredIfIDEqual(int id) : m_id(id) { }
    bool operator()(VisitControl::VisitNode pc)
    {
        return ((PControl)(pc.obj()))->getID() == m_id;
    }
private:
    int m_id;
};

//CPanel
CPanel::CPanel(void)
{
    enableHandleSelfMsg(false);
}

CPanel::CPanel(int id, PControl parent, int x, int y)
{
    enableHandleSelfMsg(false);
    create(id, parent, x, y);
}

CPanel::~CPanel()
{
}


/** 初始化*/
void CPanel::create(int id, PControl parent, int x, int y)
{
    IControl::init(id, parent, "", x, y );
}

void CPanel::update(float fElapse)
{
    if (!isVisible())
    {
        return;
    }
    m_childrenEx.update(fElapse);
}

void CPanel::render(IDirect3DDevice9 * pDevice)
{
    if (!isVisible())
    {
        return;
    }
    m_childrenEx.render(pDevice);
}

UINT CPanel::preProcessMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (canHandleSelfMsg())//此情况应用于Form
    {
        return IControl::preProcessMsg(msg, wParam, lParam);
    }
    if ( !isVisible() || !canHandleMsg() || !isEnable())
    {
        return PRE_MSG_NO;//不可见，不处理。
    }
    return PRE_MSG_OK;//可以处理。
}

UINT CPanel::messageProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    int returnCode = 0;
    CPoint ptLocal(lParam);

    m_childrenEx.lock(true);
    for (VisitControl::VisitRIterator it = m_childrenEx.rbegin(); 
        it!=m_childrenEx.rend(); 
        ++it)
    {
        if (!it->canOperate())
        {
            continue ;
        }
        PControl pCtrl = it->obj();
        CPoint pt = pCtrl->parentToLocal(ptLocal);
        UINT result = pCtrl->preProcessMsg(msg, wParam, pt.toLParam());
        if(result == PRE_MSG_OK)
        {
            if( pCtrl->messageProc( msg, wParam, pt.toLParam()) )//防止Panel在此返回
            {
                returnCode = 1;
                break;
            }
        }
        else if (result == PRE_MSG_BREAK)
        {
            returnCode = PRE_MSG_BREAK;
            break;
        }
    }
    m_childrenEx.lock(false);
    if (returnCode == 0 && canHandleSelfMsg())
    {
        return __super::messageProc(msg, wParam, ptLocal.toLParam());
    }
    return returnCode;
}

//添加控件，获得控件
void CPanel::addChild(PControl pCtrl)
{
    if (NULL == pCtrl)
    {
        return ;
    }
    pCtrl->setParent(this);
    m_childrenEx.add(pCtrl);
}

PControl CPanel::getChild( int id )
{
    VisitControl::VisitIterator it = m_childrenEx.find_if(PredIfIDEqual(id));
    if (it != m_childrenEx.end())
    {
        return (PControl)(it->obj());
    }
    return NULL;
}

void CPanel::removeChild(PControl pCtrl)
{
    if (NULL == pCtrl)
    {
        return ;
    }
    pCtrl->setParent(NULL);
    //m_children.remove(pCtrl);
    m_childrenEx.remove(pCtrl);
}

bool CPanel::isChildActive(PControl ctrl)
{
    VisitControl::VisitRIterator it = m_childrenEx.rbegin();
    if (it->obj() == ctrl)
    {
        return true;
    }
    else
    {
        return false;
    }
}

///清除子控件
void CPanel::clearChildren()
{
    for (VisitControl::VisitIterator it = m_childrenEx.begin(); 
        it != m_childrenEx.end(); 
        ++it)
    {
        if (it->canOperate())
        {
            PControl ctl = it->obj();
            ctl->setParent(NULL);
        }
    }
    m_childrenEx.clear();
}

//////////////////////////////////////////////////////////////////////////
