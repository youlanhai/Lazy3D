#include "MainPanel.h"
#include "Game.h"

CMainPanel * g_pMainPanel = NULL;

void togleFps(void)
{
    g_pMainPanel->m_lblFps->toggle();
    Lazy::callBack(0.2f, togleFps);
}
//////////////////////////////////////////////////////////////////////////
CFPSLabel::CFPSLabel(int id, PControl parent, LPCSTR caption, int x, int y)
    :  CLabel(id, parent, caption, x, y)
{
    m_crColor = 0xffff0000;
    m_size.set(1024,24);
    setFont("s16b");

    m_pFPS = new Lazy::Fps();
    m_pFPS->init(); 
}


void CFPSLabel::update(float fElapse)
{
    char buffer[128];
    sprintf_s(buffer,128,"fps:%.2f", m_pFPS->getFps());
    m_caption = buffer;
}

void CFPSLabel::render(IDirect3DDevice9 * pDevice)
{
    m_pFPS->update();
    CLabel::render(pDevice);
}

//////////////////////////////////////////////////////////////////////////

CTestForm::CTestForm(int id, PControl parent, LPCSTR image, int x, int y, int w, int h)
    : CForm(id, parent, image, x, y, w, h)
{
    createControl();
}
CTestForm::~CTestForm()
{
    OutputDebugStringA("CTest Form destruct.");
}
void CTestForm::createControl(void)
{
    GUIManagerBase *pMgr = getGUIMgr();
    pMgr->addToPool(new CLabel(4, this, "这是测试文本。哈哈哈哈！", 10,10));
    pMgr->addToPool(new CButton(5, this, "这是按钮1", "", 10,30,80,30 ));
    pMgr->addToPool(new CButton(6, this, "这是按钮2", "", 10,70,80,30 ));
    pMgr->addToPool(new CButton(7, this, "这是按钮3", "", 10,110,80,30 ));
    m_group = new CSelectGroup();
    pMgr->addToPool(new CSelect(11, this, m_group.get(), "单选1", "", 100, 30, 20,20));
    pMgr->addToPool(new CSelect(12, this, m_group.get(), "单选2", "", 100, 60, 20,20));
    pMgr->addToPool(new CSelect(13, this, m_group.get(), "单选3", "", 100, 90, 20,20));
    pMgr->addToPool(new CButton(14, this, "X", "", m_size.cx-40, 20, 20, 20));
    CButton* btn = new CButton(8, this, "这是按钮4", "gui/commen/btn%d.png", 10,150,100,100 );
    btn->enableUserDraw(true);
    pMgr->addToPool(btn);

    m_pScroll = new CScroll(9, this, "", 10, 270, 200,20);
    m_pScroll->setVertical(false);
    m_pScrollLabel = new CLabel(10, this, "", 210, 265);
}

void CTestForm::update(float fElapse)
{
    __super::update(fElapse);
}

void CTestForm::onScroll(int id, float pos)
{
    if ( id == 9)
    {
        char buffer[128];
        sprintf_s(buffer,128,"位置：%.2f", pos*100);
        m_pScrollLabel->setCaption(buffer);
    }
}

void CTestForm::onButton(int id, bool isDown)
{
    if (!isDown)
    {
        if (id == 14)
        {
            show(false);
        }
    }
}

CPoint CTestForm::getHidePos(void)
{
    return CPoint(getApp()->getWidth(), getApp()->getHeight());
}
//////////////////////////////////////////////////////////////////////////
CSysPanel::CSysPanel(int id, PControl parent, int x, int y)
    : CPanel(id, parent, x, y)
{
    pButtonExit = new CButton(1, this, "退出", "", 0,0,40,30 );;
    pBtnShowForm = new CButton(2, this, "窗口1", "", 50,0,40,30 );;
    pBtnShowForm2 = new CButton(3, this, "窗口2", "", 100,0,40,30 );;
}

void CSysPanel::onButton(int id, bool isDown)
{
    if (!isDown)
    {
        if (id == 1)
        {
            PostQuitMessage(0);
        }
        else if (id == 2)
        {
            g_pMainPanel->pForm->toggle();
        }
        else if (id == 3)
        {
            g_pMainPanel->pForm2->toggle();
        }
    }
}
//////////////////////////////////////////////////////////////////////////
CMainPanel::CMainPanel(void)
{
    g_pMainPanel = this;
}

CMainPanel::~CMainPanel(void)
{
    pForm = NULL;
    pForm2 = NULL;
    pSysPanel = NULL;
    m_lblFps = NULL;
}

void CMainPanel::createControl(void)
{
    m_lblFps = new CFPSLabel(2, getGUIMgr()->topMostPanel(), "", 2, 50);
    //RefPtr<CImage> pImage = new CImage(0, getGUIMgr()->bottomMostPanel().get(), "gui/DemonGate.bmp", 10,10,200,200);

    pForm = new CTestForm(3, getGUIMgr(), "gui/ui/goods.bmp", 100, 100, 300, 300);
    pForm2 = new CTestForm(4, getGUIMgr(), "gui/ui/goods.bmp", 200, 100, 300, 300);
    pSysPanel = new CSysPanel(5, getGUIMgr(), 100,0);
    
    togleFps();
}
