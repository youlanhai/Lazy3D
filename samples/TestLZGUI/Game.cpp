//Game.cpp
#include "Game.h"


//////////////////////////////////////////////////////////////////////////
CGame g_game;

CGame* getGampApp(void)
{
    return &g_game;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    Lazy::changeCurDirectory();
    Lazy::getfs()->addResPath(L"res/");
    Lazy::defaultCodePage = Lazy::CP::utf8;
    
    INIT_LOG(L"lzgui.log");

    if(g_game.create(hInstance, L"Test LZGUI", 640, 480, false))
    {
        g_game.mainLoop();
    }

    return 0;
}

///////////////////class CGame start/////////////////////////

/*注册窗口类消息。请实现该消息来修改窗口风格。*/
void CGame::onRegisterClass(WNDCLASSEX *pwc)
{
}

/*创建设备消息。实现该方法来修改设备。*/
void CGame::onCreateDevice(D3DPRESENT_PARAMETERS * pParam)
{

}

//窗口过程，内部使用
LRESULT CGame::wndProc(HWND hWnd,UINT uMsg,
							WPARAM wParam,LPARAM lParam)
{
    bool hooked = false;
#ifdef USE_GUI
    if (m_pGUIMgr)
    {
        if(m_pGUIMgr->messageProc(uMsg, wParam, lParam) > 0)
        {
            hooked = true;
            m_bMsgHooked = true;
        }
    }
#endif
    if (!hooked && m_pCamera)
    {
        m_pCamera->handleMessage(uMsg, wParam, lParam);
    }
    return __super::wndProc(hWnd,uMsg,wParam,lParam);
}

CGame::CGame()
{
	m_bGameStart=false;
}

void CGame::clear()
{
    __super::clear();
}
CGame::~CGame()
{
}

/*游戏初始化*/
bool CGame::init(void)
{
    LOG_INFO(L"游戏资源初始化开始...");
    __super::init();

	//在此添加初始化代码
    srand(timeGetTime());

#ifdef USE_GUI
    m_pGUIMgr = new CGUIManager(m_pd3dDevice, m_hWnd, m_hInstance);
    addUpdater(m_pGUIMgr.get());
#endif

    m_pCamera = new CCursorCamera();
    m_pCamera->setPosition(D3DXVECTOR3(0, 1.8f, -2.0f));
    m_pCamera->setSpeed(2.0f);
    m_pCamera->setDistRange(0.5f, 30.0f);
    m_pCamera->setNearFar(1.0f, 10000.0f);
	
	m_pFPS = new cFpsRender();
	m_pFPS->init();

    m_pMainPanel = new CMainPanel();
    m_pMainPanel->createControl();
	m_pCube = new CCube();
	m_pCube->init(m_pd3dDevice,1.0f, 1.0f, 1.0f);

    m_pSquare = new CSquare();
    m_pSquare->init(m_pd3dDevice, 1, 1, 100, 100);

    m_bGameStart = true;

    LOG_INFO(L"游戏资源初始化完毕。");
	return true;
}

//更新
void CGame::update()
{
	m_pFPS->update();

    __super::update();

    //在此添加更新代码
    m_pUpdateTaskMgr->update(m_fElapse);

    m_pCamera->update(m_fElapse);
    //////////////////////
	updateCamera(m_fElapse);
}
void CGame::updateCamera(float fEla)
{
    //////////////////////////////////////////////////////////////////////////
    //更新摄像机
    if(m_pKeyboard->isKeyDown('1'))
    {
        m_pCamera->setCamareType(CCamera::FIRST);
    }
    else if(m_pKeyboard->isKeyDown('2'))
    {
        m_pCamera->setCamareType(CCamera::THIRD);
    }
    else if (m_pKeyboard->isKeyDown('3'))
    {
        m_pCamera->setCamareType(CCamera::FREE);
    }

}
//渲染
void CGame::render()
{
	m_pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,100),1.0f,0);
	if(SUCCEEDED(m_pd3dDevice->BeginScene()))
	{
		//添加渲染代码
		m_pCamera->render(m_pd3dDevice);

        {
            D3DXVECTOR3 vecPos = m_pCamera->position();
            vecPos.y += 2;
            //设置灯光
		    CLight light;
		    light.SetPointLight(m_pd3dDevice,
			    vecPos,
			    D3DXCOLOR(0.8f,0.8f,0.8f,1.0f));
        }

        //设置材质
        CMaterial::setMaterial(m_pd3dDevice,D3DXCOLOR(0.3f,0.8f,0.3f,1.0f));

        //设置纹理
        m_pd3dDevice->SetTexture(0, NULL);

        __super::render();

        //设置时间矩阵
        D3DXMATRIX matWord;
        D3DXMatrixIdentity(&matWord);
        m_pd3dDevice->SetTransform(D3DTS_WORLD, &matWord);
        
        //渲染地面
        m_pSquare->render(m_pd3dDevice);

        //设置材质
        CMaterial::setMaterial(m_pd3dDevice,
            D3DXCOLOR(0.0f,0.0f,0.0f,0.0f),
            D3DXCOLOR(0.8f,0.0f,0.8f,1.0f));

		D3DXMatrixRotationAxis(&matWord,
			&D3DXVECTOR3(1.0f,1.0f,1.0f),
			timeGetTime()/1000.0f);
		D3DXMATRIX matTrans;
		D3DXMatrixTranslation(&matTrans, 0.0f, 1.0f, 0.0f);
		matWord *= matTrans;
		m_pd3dDevice->SetTransform(D3DTS_WORLD,&matWord);
        //渲染立方体
		m_pCube->render(m_pd3dDevice);

		m_pRenderTaskMgr->render(m_pd3dDevice);

#ifdef USE_GUI
        m_pGUIMgr->render(m_pd3dDevice);
#endif

		m_pFPS->render();
		/////////////////////////
		m_pd3dDevice->EndScene();
	}
	else
	{
        LOG_ERROR(L"绘制场景(m_pd3dDevice->BeginScene())失败!");
	}
	m_pd3dDevice->Present(NULL,NULL,NULL,NULL);
}

///////////////////class CGame end/////////////////////////