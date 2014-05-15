//Game.cpp
#include "Game.h"

#ifndef LZ3DENGINE_STATIC_LINK
#   ifdef _DEBUG
#       pragma comment(lib, "../lib/LZ3DEngine_d")
#       pragma comment(lib, "../lib/LZGUI_d")
#   else
#       pragma comment(lib, "../lib/LZ3DEngine")
#       pragma comment(lib, "../lib/LZGUI")
#   endif
#endif

#include "resource.h"
#include "..\LZ3DEngine\Collision.h"
#include "..\LZ3DEngine\FileTool.h"
#include "..\LZ3DEngine\BSP.h"

const float simpleRate = 2.0f;

//////////////////////////////////////////////////////////////////////////
CGame g_game;
CGame* getGampApp(void)
{
    return &g_game;
}


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    LZUtility::ChangeCurDirectory();
    INIT_LOG("gui_log.txt");

    if(g_game.create(hInstance, "LZ3DEngie Game Frame", 640, 480, false))
    {
        g_game.mainLoop();
    }
    return 0;
}

void gererateBspData()
{
    StringPool vect;
    findFiles(vect, "../bin/res/mode", "*.x");
    std::string destPath = "../bin/res/bsp/";
    int total = (int)vect.size();
    int progress = 0;
    for (StringPool::iterator it = vect.begin();
        it != vect.end(); ++it)
    {
        ++progress;
        float rate = progress*100.0f/total;
        WRITEX_LOG("INFO: 进度[%.0f%%]。正在处理模型【%s】。。。", rate, it->c_str());

        cResMesh* mesh = getResMgr()->getMesh(*it);
        if(!mesh)
        {
            continue;
        }

        LPD3DXMESH tempMesh = NULL; 
        if (simpleRate < 1.0f)
        {
            WRITEX_LOG("INFO: 正在生成简化mesh。");
            tempMesh = generateSimpleMesh(mesh->getMesh(), simpleRate);
            if (!tempMesh)
            {
                WRITEX_LOG("ERROR: 生成simple mesh 数据%s失败！", it->c_str());
                continue;
            }
        }
        else
        {
            tempMesh = mesh->getMesh();
            if (tempMesh)
            {
                tempMesh->AddRef();
            }
        }

        AVLBspTree tree;
        if (!tree.generateBsp(tempMesh))
        {
            WRITEX_LOG("ERROR: 生成bsp数据%s失败！", it->c_str());
            SafeRelease(tempMesh);
            continue;
        }
        SafeRelease(tempMesh);

        std::string modelName = getPathFile(*it);
        modelName = getFileName(modelName);

        std::string tempPath = destPath + modelName;
        tempPath += ".bsp";

        if(!tree.save(tempPath))
        {
            WRITEX_LOG("ERROR: 保存bsp数据【%s】失败!", tempPath.c_str());
        }
        else
        {
            WRITEX_LOG("INFO: 保存bsp数据【%s】成功。", tempPath.c_str());
        }

    }

}

///////////////////class CGame start/////////////////////////

/*注册窗口类消息。请实现该消息来修改窗口风格。*/
void CGame::onRegisterClass(WNDCLASSEX *pwc)
{
    pwc->hIcon = LoadIcon(m_hInstance,MAKEINTRESOURCE(IDI_LZ));
    pwc->hIconSm = pwc->hIcon;
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
        m_bMsgHooked = false;
        if(m_pGUIMgr->messageProc(uMsg, wParam, lParam) > 0)
        {
            m_bMsgHooked = true;
            hooked = true;
        }
    }
#endif
    if (!hooked && m_pCamera)
    {
        m_pCamera->handleMessage(uMsg, wParam, lParam);
    }
    getPick()->handleMouseEvent(uMsg, wParam, lParam);
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

#ifdef USE_GUI
    m_pGUIMgr = new CGUIManager(m_pd3dDevice, m_hWnd, m_hInstance);
    addUpdater(m_pGUIMgr.get());
#endif
    m_ui = new ui(1, m_pGUIMgr->normalPanel(), "", 0, 20, 120, 100 );
    addUpdateRender(&m_sample);

    m_pCamera = new CCursorCamera(m_pd3dDevice);
    m_pCamera->setPosition(D3DXVECTOR3(0, 100.8f, -200.0f));
    m_pCamera->setSpeed(50.0f);
    m_pCamera->setDistRange(0.5f, 30.0f);
    m_pCamera->setNearFar(1.0f, 10000.0f);
	
	m_pFPS = new cFpsRender();
	m_pFPS->init();

	m_pCube = new CCube();
	m_pCube->init(m_pd3dDevice,1.0f, 1.0f, 1.0f);

    m_pSquare = new CSquare();
    m_pSquare->init(m_pd3dDevice, 1, 1, 100, 100);

    m_bGameStart = true;

    WRITE_LOG("游戏资源初始化完毕。");

#if 0
    gererateBspData();
#endif

#if 0
    cResMesh* mesh = getResMgr()->getMesh("传送门.X");
    if(mesh)
    {
        AVLBspTree tree;
        if (tree.generateBsp(mesh->getMesh()))
        {
            tree.testSave("testBsp.xml");

            tree.save("testBsp.txt");

            tree.load("testBsp.txt");
            tree.save("testBsp2.txt");

        }
    }
#endif

	return true;
}

//更新
void CGame::update()
{
	m_pFPS->update(m_fElapse);

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

        //设置灯光
        CLight light;
        light.SetDirectionLight(m_pd3dDevice, D3DXVECTOR3(0, -1, 0),
            D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

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
        //m_pSquare->render(m_pd3dDevice);

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
		//m_pCube->render(m_pd3dDevice);

		m_pRenderTaskMgr->render(m_pd3dDevice);

        //getPick()->render(m_pd3dDevice);

#ifdef USE_GUI
        m_pGUIMgr->render(m_pd3dDevice);
#endif

		m_pFPS->render(m_pd3dDevice);
		/////////////////////////
		m_pd3dDevice->EndScene();
	}
	else
	{
		WRITE_LOG("绘制场景(m_pd3dDevice->BeginScene())失败!");
	}
	m_pd3dDevice->Present(NULL,NULL,NULL,NULL);
}

///////////////////class CGame end/////////////////////////