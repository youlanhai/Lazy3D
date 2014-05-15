#pragma once

#include "LZGUI/LZGUI.h"


class CFPSLabel : public CLabel
{
public:
    CFPSLabel(int id, PControl parent, LPCSTR caption, int x, int y);
    virtual LPCSTR getClassName(void){ return "CFPSLabel"; }
    void update(float fElapse);
    void render(IDirect3DDevice9 * pDevice);

    RefPtr<Lazy::Fps>	    m_pFPS;
};

class CTestForm : public CForm
{
public:
    CTestForm(int id, PControl parent, LPCSTR image, int x, int y, int w, int h);
    ~CTestForm();
    virtual LPCSTR getClassName(void){ return "CTestForm"; }
    void createControl(void);
    void update(float fElapse);
    void onScroll(int id, float pos);
    void onButton(int id, bool isDown);
    CPoint getHidePos(void);
private:
    RefPtr<CScroll>     m_pScroll;
    RefPtr<CLabel>      m_pScrollLabel;
    RefPtr<CSelectGroup> m_group;
};

class CSysPanel : public CPanel
{
public:
    CSysPanel(int id, PControl parent, int x, int y);
    virtual LPCSTR getClassName(void){ return "CSysPanel"; }
    void onButton(int id, bool isDown);
protected:
    RefPtr<CButton>     pButtonExit;
    RefPtr<CButton>     pBtnShowForm;
    RefPtr<CButton>     pBtnShowForm2;
};

class CMainPanel : public IBase
{
public:
    CMainPanel();
    ~CMainPanel(void);

    void createControl(void);

public:
    RefPtr<CFPSLabel>  m_lblFps;
    RefPtr<CTestForm>   pForm;
    RefPtr<CTestForm>   pForm2;
    RefPtr<CSysPanel>      pSysPanel;
   
};
