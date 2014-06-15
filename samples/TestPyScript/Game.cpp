//Game.cpp
#include "resource.h"
#include "Game.h"
#include "Render/RenderDevice.h"
#include "Font/Font.h"

#include "Lzpy/Lzpy.h"
#include "PyScript/LPyScript.h"
#include "PyUI/PyUI.h"

//////////////////////////////////////////////////////////////////////////
CGame g_game;
RefPtr<Lzpy::ConsolePanel> g_pyConsole;

const std::string Script = "main";


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    Lazy::changeCurDirectory();
    Lazy::defaultCodePage = Lazy::CP::utf8;
    Lazy::getfs()->addResPath(L"res/");

    INIT_LOG(L"test_pyscript.log");

    if (g_game.create(hInstance, L"Lazy Test", 800, 600, false))
    {
        g_game.mainLoop();
    }
    return 0;
}


bool initPython()
{
    Py_SetPythonHome(L"../src/third_part/python33");

    PyImport_AppendInittab("helper", Lzpy::PyInit_helper);
    PyImport_AppendInittab("Lazy", Lzpy::PyInit_Lazy);
    PyImport_AppendInittab("lui", Lzpy::PyInit_lui);
    PyImport_AppendInittab("lzd", Lzpy::PyInit_lzd);

    Py_Initialize();
    if (!Py_IsInitialized())
    {
        debugMessage(_T("初始化python失败！"));
        return false;
    }

    try
    {
        Lzpy::import("helper");
        Lzpy::import("Lazy");
        Lzpy::import("lui");
        Lzpy::import("lzd");

        Lzpy::object sys = Lzpy::import(L"sys");
        Lzpy::object ouput = Lzpy::new_reference(Lzpy::helper::new_instance_ex<Lzpy::PyOutput>());
        sys.setattr("stdout", ouput);
        sys.setattr("stderr", ouput);

        PyRun_SimpleString("print('hahah')");

        Lzpy::addSysPath(L"res/script");

        Lzpy::LzpyResInterface::initAll();

        Lzpy::object entry = Lzpy::import(Script.c_str());
        if (!entry)
        {
            LOG_ERROR(L"Can't find script '%s'", Script.c_str());
            PyErr_Print();
            return false;
        }

        if (!entry.call_method("init"))
        {
            LOG_ERROR(L"Failed to init script.");
            PyErr_Print();
            return false;
        }
    }
    catch (std::exception e)
    {
        Lazy::debugMessageA("ERROR: init python failed! %s", e.what());

        if (PyErr_Occurred())
            PyErr_Print();
        return false;
    }

    return true;
}

bool finiPython()
{
    if (!Py_IsInitialized())
    {
        return false;
    }

    try
    {
        g_pyConsole = nullptr;

        Lzpy::object entry = Lzpy::import(Script.c_str());
        entry.call_method("fini");

        Lzpy::LzpyResInterface::finiAll();
    }
    catch (...)
    {
        if (PyErr_Occurred())
            PyErr_Print();
        return false;
    }


    Py_Finalize();
    return true;
}

class TestForm : public Lazy::CForm
{
public:

    TestForm()
    {
        m_btnOk = new Lazy::CButton();
        m_btnOk->create(1, L"OK", L"", 100, 100, 50, 20);
        addChild(m_btnOk.get());
    }

#if 0
    virtual void onButton(int id, bool isDown) override
    {
        if (id == m_btnOk->getID())
        {
            debugMessage(_T("onButton OK!"));
        }
    }
#endif

private:

    RefPtr<Lazy::CButton> m_btnOk;
};

///////////////////class CGame start/////////////////////////

/*注册窗口类消息。请实现该消息来修改窗口风格。*/
void CGame::onRegisterClass(WNDCLASSEX *pwc)
{
    pwc->hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_LAZY));
    pwc->hIconSm = pwc->hIcon;
}

/*创建设备消息。实现该方法来修改设备。*/
void CGame::onCreateDevice(D3DPRESENT_PARAMETERS * pParam)
{

}

//窗口过程，内部使用
///消息处理
bool CGame::onEvent(const Lazy::SEvent & event)
{
    if (event.eventType == Lazy::EET_MOUSE_EVENT)
    {
        switch (event.mouseEvent.event)
        {
        case Lazy::EME_LMOUSE_DOWN:
            SetCapture(m_hWnd);
            break;

        case Lazy::EME_LMOUSE_UP:
            ReleaseCapture();
            break;

        default:
            break;
        }
    }

#ifdef USE_GUI
    if (m_pGUIMgr && m_pGUIMgr->processEvent(event))
    {
        m_bMsgHooked = true;
        return true;
    }
#endif

    if (event.eventType == Lazy::EET_KEY_EVENT)
    {
        auto keyEvent = event.keyEvent;

        if (g_pyConsole &&
            keyEvent.key == Lazy::KEY_F1 &&
            !keyEvent.down)
        {
            g_pyConsole->toggle();
            return true;
        }

        Lzpy::object script = Lzpy::import(Script.c_str());
        if (script.call_method("onKeyEvent", keyEvent.down, int(keyEvent.key)))
            return true;
    }

    if (m_pCamera && m_pCamera->handleEvent(event)) return 1;

#if 0
    if (cPick::instance()->handleMouseEvent(uMsg, wParam, lParam)) return 1;
    
    TerrainMap::instance()->handeMouseEvent(uMsg, wParam, lParam);

    if (EntityMgr::instance()->handleMouseEvent(uMsg, wParam, lParam)) return 1;
#endif

    return false;
    //return CApp::onEvent(event);
}

CGame::CGame()
{
	m_bGameStart=false;
}

void CGame::clear()
{
    LOG_INFO(L"Release resource start...");
    finiPython();

    Lazy::LoadingMgr::instance()->fini();

    CApp::clear();

    WRITE_LOG(L"Release resource finished.");
}
CGame::~CGame()
{
}

/*游戏初始化*/
bool CGame::init(void)
{
    LOG_INFO(L"Initialize game start...");

    if (!CApp::init()) return false;

#ifdef USE_GUI
    m_pGUIMgr = new Lazy::CGUIManager(m_pd3dDevice, m_hWnd, m_hInstance);

    g_pyConsole = new Lzpy::ConsolePanel();
    g_pyConsole->setSize(m_nWidth, m_nHeight);
    m_pGUIMgr->addChild(g_pyConsole.get());

    TestForm *pForm = new TestForm();
    pForm->create(1, L"gui/ui/goods.bmp", 500, 0);
    pForm->setSize(300, 200);
    m_pGUIMgr->addChildManage(pForm);


#endif

    m_pCamera = new CCursorCamera();
    m_pCamera->setPosition(D3DXVECTOR3(0, 1.8f, -2.0f));
    m_pCamera->setSpeed(2.0f);
    m_pCamera->setDistRange(2.0f, 10.0f);
    m_pCamera->setNearFar(1.0f, 10000.0f);
    m_pCamera->setHeight(1.5f);
	
	m_pFPS = new cFpsRender();
	m_pFPS->init();

	m_pCube = new CCube();
	m_pCube->init(m_pd3dDevice,1.0f, 1.0f, 1.0f);

    m_pSquare = new CSquare();
    m_pSquare->init(m_pd3dDevice, 1, 1, 100, 100);

    m_bGameStart = true;

    if (!initPython()) return false;

    Lazy::LoadingMgr::instance()->init();

    LOG_INFO(L"Initialize game finished.");
	return true;
}

//更新
void CGame::update()
{
    CApp::update();

    Lazy::LoadingMgr::instance()->dispatchFinishTask();

    //在此添加更新代码
    m_pUpdateTaskMgr->update(m_fElapse);

    m_pCamera->update(m_fElapse);
    //////////////////////
	updateCamera(m_fElapse);

#ifdef USE_GUI
    m_pGUIMgr->update(m_fElapse);
#endif

    if (PyErr_Occurred())
        PyErr_Print();
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
    m_pFPS->update();

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

        CApp::render();

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
		LOG_ERROR(L"m_pd3dDevice->BeginScene() failed!");
	}
	m_pd3dDevice->Present(NULL,NULL,NULL,NULL);
}

///////////////////class CGame end/////////////////////////