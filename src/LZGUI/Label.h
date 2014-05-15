#pragma once

#include "Control.h"

class LZGUI_API CLabel : public IControl
{
public:
    CLabel(void);
    CLabel(int id, IControl *parent, stdstring caption, int x, int y);
    ~CLabel(void);

    virtual void create(int id, IControl *parent, stdstring caption, int x, int y);

    /** 设置对齐方式。
        @param style : left:左, right：右，center : 右。*/
    void setAlign(stdstring style);

    void setMutiLine(bool m);

    virtual UINT messageProc(UINT msg, WPARAM wParam, LPARAM lParam);

    LPCSTR getClassName(void){ return "CLabel";}

    virtual void update(float fElapse);
    virtual void render(IDirect3DDevice9 * pDevice);

    virtual void setFont(stdstring fontName);
protected:

#ifdef LZGUI_2D
    HFONT       m_pFont;
#else
    LPD3DXFONT m_pFont;
#endif

    DWORD	m_style;    //字体风格
};

LZGUI_API CLabel* createLabel(int id, stdstring caption, int x, int y);
//////////////////////////////////////////////////////////////////////////
