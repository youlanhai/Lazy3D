#include "Button.h"


#define CR_SELECT   0x7fffffff
#define CR_TIME 0.5f
COLORREF buttonColor[] = {0xaf808080, 0xafd0d0d0, 0xaf606060, 0xaf404040, 0xafa0a0a0};

//////////////////////////////////////////////////////////////////////////
CButton::CButton(void)
{
}

CButton::CButton(int id, 
                 PControl parent, 
                 LPCSTR caption, 
                 LPCSTR image, 
                 int x, 
                 int y, 
                 int w, 
                 int h)
{
    create(id, parent, caption, image, x, y, w, h);
}

CButton::~CButton(void)
{
    m_pLabel = NULL;
}

void CButton::create(
                    int id, 
                    PControl parent, 
                    LPCSTR caption, 
                    LPCSTR image, 
                    int x, 
                    int y, 
                    int w, 
                    int h)
{
    memcpy(m_stateColor, buttonColor, sizeof(buttonColor));
    m_pLabel = new CLabel(0, this, caption, 0, 0);
    __super::create(id, parent, image, x, y, w, h);
    m_pLabel->setSize(w, h);
    m_pLabel->setAlign("center");
    m_crFade.reset(m_stateColor[0], m_stateColor[0], CR_TIME);
    m_crFade.setSource(this);
    setBackColor(m_stateColor[0]);
    setBtnState(BS_NORMAL);
    enableTrack(true);
    enableDrag(false);
    enableColor(true);
    enablePosMove(true);
    enableClickTop(false);
}

UINT CButton::messageProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    return __super::messageProc(msg,wParam,lParam);
}


void CButton::render(IDirect3DDevice9 * pDevice)
{
    CPoint old = getPosition();
    if (m_state == BS_ACTIVE && m_posMoveEnable)
    {
        setPosition(old.x-1, old.y-1);
    }
    if (m_colorEnable)
    {
        //setBackColor(m_crFade.getCurColor());
    }
    else
    {
        setBackColor(0xffffffff);
    }
    m_image->setImage(getImage());
    __super::render(pDevice);
    if (m_posMoveEnable)
    {
        setPosition(old.x, old.y);
    }
}


void CButton::onMouseMove(CPoint pt, CPoint old)
{

}

void CButton::onMouseEnter(CPoint pt)
{
    setBtnState(BS_ACTIVE);
}

void CButton::onMouseLeave(CPoint pt)
{
    setBtnState(BS_NORMAL);
}

void CButton::onMouseLButton(bool isDown, CPoint pt)
{
    if (!isDown)
    {
        setBtnState(BS_ACTIVE);
    }
    else
    {
        setBtnState(BS_DOWN);
    }
    if (m_parent)
    {
        m_parent->onButton(m_id, isDown);
    }
}

void CButton::setSize(int w, int h)
{
    __super::setSize(w, h);
    m_pLabel->setSize(w, h);
}

void CButton::setCaption(stdstring caption)
{
    __super::setCaption(caption);
    m_pLabel->setCaption(caption);
}

void CButton::setBtnState(BUTTON_STATE state)
{
    m_state = state;
    m_crFade.setDestColor(m_stateColor[state]);
}

stdstring CButton::getImage(void)
{
    if (m_bkImage.find("%d") != m_bkImage.npos)
    {
        char buffer[256];
        sprintf_s(buffer, 256, m_bkImage.c_str(), m_state);
        return buffer;
    }
    return m_bkImage;
}

void CButton::enableUserDraw(bool d)
{
    enablePosMove(!d);
    enableColor(!d);

}

void CButton::setStateColor(DWORD state, DWORD cr)
{
    m_stateColor[state] = cr;
}

DWORD CButton::getStateColor(DWORD state)
{
    return m_stateColor[state];
}
//////////////////////////////////////////////////////////////////////////
CSelect::CSelect(void)
{

}

CSelect::CSelect(
                 int id, 
                 PControl parent, 
                 PSelectGroup group, 
                 LPCSTR caption, 
                 LPCSTR image, 
                 int x, 
                 int y, 
                 int w, 
                 int h)
{
    create(id, parent, group, caption, image, x, y, w, h);
}

CSelect::~CSelect()
{
    m_group = NULL;
}

void CSelect::create(
                    int id, 
                    PControl parent, 
                    PSelectGroup group, 
                    LPCSTR caption, 
                    LPCSTR image, 
                    int x, 
                    int y, 
                    int w, 
                    int h)
{
    __super::create(id, parent, caption, image, x, y, w, h);
    m_pLabel->setPosition(w, 0);
    m_pLabel->setSize(1024, m_pLabel->getSize().cy);
    m_pLabel->setAlign("left");
    m_bSelect = false;
    m_group = group;
    if (m_group)
    {
        m_group->addSelect(this);
    }
}

void CSelect::select(bool s)
{ 
    m_bSelect = s ;
    if (m_bSelect && m_group)
    {
        m_group->select(this);
    }
    else if (!m_bSelect)
    {
        setBtnState(BS_NORMAL);
    }
}

void CSelect::render(IDirect3DDevice9 * pDevice)
{
    __super::render(pDevice);
}

void CSelect::onMouseLButton(bool isDown, CPoint pt)
{
    __super::onMouseLButton(isDown, pt);
    if (!isDown)
    {
        select(!m_bSelect);
    }
}

void CSelect::setBtnState(BUTTON_STATE state)
{
    if (m_state == BS_DOWN)
    {
        m_state = state;
        m_crFade.setDestColor(CR_SELECT);//(CR_SELECT);
    }
    else if(!m_bSelect)
    {
        m_crFade.setDestColor(m_stateColor[state]);
        m_state = state;
    }
}

//////////////////////////////////////////////////////////////////////////
CSelectGroup::CSelectGroup(
                           int id, 
                           PControl parent, 
                           LPCSTR image, 
                           int x, 
                           int y, 
                           int w, 
                           int h)
{
    create(id, parent, image, x, y, w, h);
}

CSelectGroup::CSelectGroup()
{

}

void CSelectGroup::create(
                    int id, 
                    PControl parent, 
                    LPCSTR image, 
                    int x, 
                    int y, 
                    int w, 
                    int h)
{
    __super::create(id, parent, image, x, y, w, h);
}

void CSelectGroup::addSelect(CSelect* ctrl)
{
    if (find(m_selects.begin(), m_selects.end(), ctrl) != m_selects.end())
    {
        return ;
    }
    m_selects.push_back(ctrl);
}

CSelect* CSelectGroup::getSelected(void)
{
    for (SelectIterator it = m_selects.begin(); 
        it != m_selects.end(); ++it)
    {
        if ((*it)->isSelected())
        {
            return (*it);
        }
    }
    return NULL;
}

void CSelectGroup::select(CSelect* ctrl)
{
    if (!ctrl->isSelected())
    {
        return;
    }
    for (SelectIterator it = m_selects.begin(); 
        it != m_selects.end(); ++it)
    {
        if ((*it) != ctrl)
        {
            (*it)->select(false);
        }
    }
}
