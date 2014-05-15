//Game.cpp
#include "Game.h"


#ifdef _DEBUG
#   pragma comment(lib, "LZ3DEngine_d")
#   pragma comment(lib, "LZGUI_d")
#else
#   pragma comment(lib, "LZ3DEngine")
#   pragma comment(lib, "LZGUI")
#endif


//////////////////////////////////////////////////////////////////////////
CGame g_game;
CGame* getGampApp(void)
{
    return &g_game;
}


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    LZUtility::ChangeCurDirectory();
    INIT_LOG("use_pywrapper_log.txt");

    if(g_game.create(hInstance, "LZ3DEngie Game Frame", 640, 480, false))
    {
        g_game.mainLoop();
    }
    return 0;
}

///////////////////class CGame start/////////////////////////

/*注册窗口类消息。请实现该消息来修改窗口风格。*/
void CGame::onRegisterClass(WNDCLASSEX *pwc)
{
//     pwc->hIcon = LoadIcon(m_hInstance,MAKEINTRESOURCE(IDI_LZ));
//     pwc->hIconSm = pwc->hIcon;
}

/*创建设备消息。实现该方法来修改设备。*/
void CGame::onCreateDevice(D3DPRESENT_PARAMETERS * pParam)
{

}

//窗口过程，内部使用
LRESULT CGame::wndProc(HWND hWnd,UINT uMsg,
							WPARAM wParam,LPARAM lParam)
{
    if (m_pGUIMgr)
    {
        if(m_pGUIMgr->messageProc(uMsg, wParam, lParam) > 0)
        {
            m_bMsgHooked = true;
        }
        else
        {
            m_bMsgHooked = false;
            if (m_pCamera)
            {
                m_pCamera->handleMessage(uMsg, wParam, lParam);
            }
        }
    }
    return __super::wndProc(hWnd,uMsg,wParam,lParam);
}

CGame::CGame()
{
	m_bGameStart=false;
}

void CGame::clear()
{
	WRITE_LOG("开始释放资源...");
    __super::clear();

    m_pyScript->release();

    WRITE_LOG("资源释放完毕。");
}
CGame::~CGame()
{
}

/*游戏初始化*/
bool CGame::init(void)
{
    WRITE_LOG("游戏资源初始化开始...");
    __super::init();

	//在此添加初始化代码
    srand(timeGetTime());

    m_pGUIMgr = new CGUIManager(m_pd3dDevice, m_hWnd, m_hInstance);
    addUpdater(m_pGUIMgr.get());

    m_pCamera=new CCursorCamera(m_pd3dDevice);
    m_pCamera->setup3DCamera();
    m_pCamera->setPos(D3DXVECTOR3(0,100,-500));
    m_pCamera->setSpeed(200.0f);
	
	m_pFPS=new CFPS();
	m_pFPS->init();

	m_pCube=new CCube();
	m_pCube->init(m_pd3dDevice,100.0f,100.0f,100.0f);

    m_pSquare = new CSquare();
    m_pSquare->init(m_pd3dDevice,50,50,100,100);

    m_pyScript = new cPyScript();
    if( !m_pyScript->init() )
    {
        WRITE_LOG("加载脚本出错！");
        return false;
    }

    m_bGameStart=true;

    addUpdater(m_pCamera.get());
    WRITE_LOG("游戏资源初始化完毕。");
	return true;
}

//更新
void CGame::update()
{
    __super::update();
	m_pFPS->updateFPS();
	if(!m_bGameStart)
	{
		return;
	}
	float curTime=(float)timeGetTime();
	static float fOldUpdateTime=0;
    float fElapse = (curTime - fOldUpdateTime)/1000.0f;
	fOldUpdateTime=curTime;

    __super::update();
    //在此添加更新代码
    m_pUpdateTaskMgr->update(fElapse);

    //////////////////////
	updateCamera(fElapse);
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
        float curTime=(float)timeGetTime();
        static float fOldUpdateTime=0;
        float fElapse = (curTime - fOldUpdateTime)/1000.0f;
        fOldUpdateTime = curTime;
		//添加渲染代码
		m_pCamera->update3DCamera();

        {
            D3DXVECTOR3 vecPos = m_pCamera->getPos();
            vecPos.y += 200;
            //设置灯光
		    CLight light;
		    light.SetPointLight(m_pd3dDevice,
			    vecPos,
			    D3DXCOLOR(0.8f,0.8f,0.8f,1.0f));
        }

        //设置材质
        CMaterial::setMaterial(m_pd3dDevice,D3DXCOLOR(0.3f,0.8f,0.3f,1.0f));

        //设置纹理
        m_pd3dDevice->SetTexture(0,NULL);

        __super::render();

        //设置时间矩阵
        D3DXMATRIXA16 matWord;
        D3DXMatrixIdentity(&matWord);
        m_pd3dDevice->SetTransform(D3DTS_WORLD,&matWord);
        
        //渲染地面
        m_pSquare->render(m_pd3dDevice);

        //设置材质
        CMaterial::setMaterial(m_pd3dDevice,
            D3DXCOLOR(0.0f,0.0f,0.0f,0.0f),
            D3DXCOLOR(0.8f,0.0f,0.8f,1.0f));

		D3DXMatrixRotationAxis(&matWord,
			&D3DXVECTOR3(1.0f,1.0f,1.0f),
			timeGetTime()/1000.0f);
		D3DXMATRIXA16 matTrans;
		D3DXMatrixTranslation(&matTrans,0.0f,100.0f,0.0f);
		matWord *= matTrans;
		m_pd3dDevice->SetTransform(D3DTS_WORLD,&matWord);
        //渲染立方体
		m_pCube->render(m_pd3dDevice);

		m_pRenderTaskMgr->render(m_pd3dDevice);
        m_pGUIMgr->render(m_pd3dDevice);
		renderText();
		/////////////////////////
		m_pd3dDevice->EndScene();
	}
	else
	{
		WRITE_LOG("绘制场景(m_pd3dDevice->BeginScene())失败!");
	}
	m_pd3dDevice->Present(NULL,NULL,NULL,NULL);
//     if(m_pFPS->getFPS()>50.0f)
//     {
//         Sleep(1);
//     }
}

void CGame::renderText()
{
	char buffer[128];
	sprintf_s(buffer,128,"fps:%.3f",m_pFPS->getFPS());
	RECT rc;
	rc.left=rc.top=0;
	rc.right=500;
	rc.bottom=20;
    LPD3DXFONT font = m_pResMgr->getFontEx("");
    if (font)
    {
        font->DrawText(NULL, LZUtility::CharToWChar(buffer), -1, &rc, DT_LEFT|DT_SINGLELINE, 0xffff0000);
    }
}
///////////////////class CGame end/////////////////////////