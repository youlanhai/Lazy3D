//Game.cpp
#include "Game.h"

#include "Render/RenderDevice.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

//////////////////////////////////////////////////////////////////////////

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    Lazy::changeCurDirectory();
    Lazy::defaultCodePage = Lazy::CP::utf8;
    Lazy::getfs()->addResPath(L"res/");

    INIT_LOG(L"ModelView.log");

    CGame game;
    if (game.create(hInstance, L"Lazy ModelView", 640, 480, false))
    {
        game.mainLoop();
    }
    return 0;
}

///////////////////class CGame start/////////////////////////

/*注册窗口类消息。请实现该消息来修改窗口风格。*/
void CGame::onRegisterClass(WNDCLASSEX *pwc)
{
    //pwc->hIcon = LoadIcon(m_hInstance,MAKEINTRESOURCE(IDI_LZ));
    //pwc->hIconSm = pwc->hIcon;
}

//窗口过程，内部使用
bool CGame::onEvent(const Lazy::SEvent & event)
{
    if (event.eventType == Lazy::EET_MOUSE_EVENT)
    {
        if (event.mouseEvent.event == Lazy::EME_LMOUSE_DOWN)
        {
            SetCapture(m_hWnd);
        }
        else if (event.mouseEvent.event == Lazy::EME_LMOUSE_UP)
        {
            ReleaseCapture();
        }
    }

    if (m_guimgr && m_guimgr->processEvent(event))
    {
        m_bMsgHooked = true;
        return true;
    }

    if (m_pCamera && m_pCamera->handleEvent(event))
    {
        return true;
    }

    return CApp::onEvent(event);
}

CGame::CGame()
    : m_bGameStart(false)
{
}

void CGame::clear()
{
    LOG_INFO(L"Release resource start...");
    CApp::clear();

    m_guimgr->destroy();
    m_guimgr = nullptr;

    LOG_INFO(L"Release resource finished.");
}
CGame::~CGame()
{
}

/*游戏初始化*/
bool CGame::init(void)
{
    LOG_INFO(L"Initialize game start...");
    CApp::init();

	//在此添加初始化代码
    srand(timeGetTime());

    //设置shader
    Lazy::rcDevice()->setFontShader(L"shader/font.fx");

    //创建ui
    m_guimgr = new Lazy::CGUIManager(m_pd3dDevice, m_hWnd, m_hInstance);
    m_guimgr->getUIRender()->setShader(L"shader/ui.fx");
    
    //创建相机
    m_pCamera = new CCursorCamera();
    m_pCamera->setPosition(D3DXVECTOR3(0, 1.8f, -2.0f));
    m_pCamera->setSpeed(2.0f);
    m_pCamera->setDistRange(0.5f, 30.0f);
    m_pCamera->setNearFar(1.0f, 10000.0f);
	
	m_pFPS = new cFpsRender();
	m_pFPS->init();

    m_modelView = new Lazy::ModelView();
    m_modelView->create(0, L"", 0, 10, 200, 500);
    m_guimgr->addChild(m_modelView.get());

    //以下是测试代码

	m_pCube = new CCube();
	m_pCube->init(m_pd3dDevice,1.0f, 1.0f, 1.0f);

    m_pSquare = new CSquare();
    m_pSquare->init(m_pd3dDevice, 1, 1, 100, 100);

    //以上是测试代码

    m_bGameStart = true;

    LOG_INFO(L"Initialize game finished.");
	return true;
}

//更新
void CGame::update()
{
    m_pFPS->update();

    //在此添加更新代码
    m_pUpdateTaskMgr->update(m_fElapse);

    m_pCamera->update(m_fElapse);
    m_guimgr->update(m_fElapse);
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

    Lazy::rcDevice()->clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 100), 1.0f, 0);

	if(Lazy::rcDevice()->beginScene())
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

        //设置世界矩阵
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
        m_guimgr->render(m_pd3dDevice);
		m_pFPS->render();
		/////////////////////////
        Lazy::rcDevice()->endScene();
	}
	else
	{
		LOG_ERROR(L"BeginScene() failed!");
	}

    Lazy::rcDevice()->present();
}

///////////////////class CGame end/////////////////////////