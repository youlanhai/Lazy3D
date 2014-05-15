
#include "Label.h"
#include "GUIMgr.h"

CLabel::CLabel(void)
{
}

CLabel::CLabel(int id, IControl* parent, stdstring caption, int x, int y)
{
    create(id, parent, caption, x, y);
}

CLabel::~CLabel(void)
{
    //getGuiMgr()->remove(this);
}

void CLabel::create(int id, IControl *parent, stdstring caption, int x, int y)
{
    //getGuiMgr()->add(this);
    m_pFont = NULL;
    m_style = 0;
    init(id, parent, caption, x, y);
    m_size.set(1024, 20);
    setFont("s14");
    setAlign("");
}

void CLabel::setAlign(stdstring style)
{
    if (style == "right")
    {
        m_style = DT_RIGHT ;
    }
    else if (style == "center")
    {
        m_style = DT_CENTER ;
    }
    else
    {
        m_style = DT_LEFT ;
    }
    m_style |= DT_SINGLELINE| DT_VCENTER;
}

UINT CLabel::messageProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

void CLabel::update(float fElapse)
{

}

void CLabel::render(IDirect3DDevice9 * pDevice)
{
    if (!m_bVisible)
    {
        return;
    }
    if((m_pFont != NULL) && (!m_caption.empty()))
    {
        CRect rc = getClientRect();
        rc = localToGlobal(rc);
#ifdef LZGUI_2D
        //[todo]
        //setFont
        //setColor
        DrawText(NULL, Lazy::charToWChar(m_caption).c_str(), 
            -1, &(rc.toRect()), m_style);
#else
        m_pFont->DrawText(0, Lazy::charToWChar(m_caption).c_str(),
            -1, &(rc.toRect()), m_style, m_crColor);
#endif

    }
}

void CLabel::setFont(stdstring fontName)
{
    IControl::setFont(fontName);
#ifdef LZGUI_2D

#else
    m_pFont = getResMgr()->getFontEx(Lazy::charToWChar(fontName));
#endif
    m_size.cy = 20;//getFontLib()->getFontSize(fontName);
}

LZGUI_API CLabel* createLabel(int id, stdstring caption, int x, int y)
{
    return new CLabel(id, (IControl*)getGUIMgr(), caption, x, y);
}

void CLabel::setMutiLine(bool m)
{
    if (m)
    {
        m_style = DT_WORDBREAK;
    }
    else
    {
        m_style = DT_LEFT | DT_SINGLELINE| DT_VCENTER;
    }
}