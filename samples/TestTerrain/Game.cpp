//Game.cpp
#include "Game.h"
#include "Physics/PhysicsDebug.h"

//#include "resource.h"

static const char * compileTime = __DATE__ " " __TIME__;
static CGame g_game;
//////////////////////////////////////////////////////////////////////////


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    Lazy::defaultCodePage = Lazy::CP::utf8;
    Lazy::changeCurDirectory();
    Lazy::getfs()->addResPath(L"res/");

    INIT_LOG(_T("TestTerrain.log"));
    XWRITE_LOGA("compile time: %s", compileTime);

    if(g_game.create(hInstance, L"Terrain", 640, 480, false))
    {
        g_game.mainLoop();
    }

    return 0;
}

class Player : public iEntity
{
public:
    Player()
    {
    }
        
    bool isPlayer() override { return true; }
};

EntityPtr g_player;

///////////////////class CGame start/////////////////////////

/*注册窗口类消息。请实现该消息来修改窗口风格。*/
void CGame::onRegisterClass(WNDCLASSEX *pwc)
{
    //pwc->hIcon = LoadIcon(m_hInstance,MAKEINTRESOURCE(IDI_LZ));
    //pwc->hIconSm = pwc->hIcon;
}

/*创建设备消息。实现该方法来修改设备。*/
void CGame::onCreateDevice(D3DPRESENT_PARAMETERS * pParam)
{
    pParam->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}

//窗口过程，内部使用
LRESULT CGame::wndProc(HWND hWnd,UINT uMsg,
							WPARAM wParam,LPARAM lParam)
{
    if (m_bGameStart)
    {
        if(m_pCamera->handleMessage(uMsg, wParam, lParam)) 
            return 0;

        else if(cPick::instance()->handleMouseEvent(uMsg, wParam, lParam)) 
            return 0;
        
        TerrainMap::instance()->handeMouseEvent(uMsg, wParam, lParam) ;
    }
    return __super::wndProc(hWnd,uMsg,wParam,lParam);
}

CGame::CGame()
{
	m_bGameStart=false;
    m_bUseLineMode = false;
}

void CGame::clear()
{
    WRITE_LOG(_T("TRACE: Start clear..."));
    
    //TerrainMap::instance()->saveMap("");
    Lazy::LoadingMgr::instance()->fini();

    __super::clear();

    WRITE_LOG(_T("TRACE: Clear finished."));
}
CGame::~CGame()
{
}

/*游戏初始化*/
bool CGame::init(void)
{
    WRITE_LOG(_T("TRACE: start initialize..."));
    __super::init();

	//在此添加初始化代码
	srand(timeGetTime());

    Lazy::LZDataPtr ptrRoot  = Lazy::openSection(L"terrain.lzd");
    if (!ptrRoot)
    {
        WRITE_LOG(_T("load game config failed!"));
        return false;
    }

    bool useNewStyle = ptrRoot->readBool(L"use_new_style", false);
    MapConfig::UseNewFormat = useNewStyle;

    bool mapUpgrade = ptrRoot->readBool(L"upgrade", false);

    g_player = new Player();
    g_player->setPhysics(new iPhysics());
    g_player->setSpeed(500.0f);

    ModelPtr model = ModelFactory::loadModel(L"model/jingtian/jingtian.x", ModelType::SkinModel);
    if (model)
    {
        model->setYaw(3.14159f);
        g_player->setModel(model);
    }

    addUpdateRender(g_player.get());
    TerrainMap::instance()->setSource(g_player.get());

	m_pCamera = new CCursorCamera(CCamera::THIRD);
    m_pCamera->setSource(g_player.get());
	m_pCamera->setSpeed(100.0f);

    Lazy::LZDataPtr ptrPos = ptrRoot->read(L"camera/pos");
	m_pCamera->setPosition(D3DXVECTOR3(
        ptrPos->readFloat(L"x"),
        ptrPos->readFloat(L"y"),
        ptrPos->readFloat(L"z")));
    m_pCamera->setSpeed(ptrRoot->readFloat(L"camera/speed"));
    m_pCamera->setNearFar(1.0f, 100000.0f);
    m_pCamera->setDistRange(10.0f, 1000.0f);
    m_pCamera->setDistance(600.0f);
	
    m_pFPS = new cFpsRender();
    m_pFPS->init();

	m_pCube=new CCube();
	m_pCube->init(m_pd3dDevice,100.0f,100.0f,100.0f);

    m_pSkyBox->setSource(g_player.get());
    m_pSkyBox->setSkyImage(ptrRoot->readString(L"sky"));
    m_pSkyBox->setSkyRange(D3DXVECTOR3(-10000,-8000,-10000), D3DXVECTOR3(10000,12000,10000));

    if (mapUpgrade)
    {
        MapConfig::UseMultiThread = false;

        Lazy::LZDataPtr maps = ptrRoot->read(L"maps");
        if (!maps || maps->countChildren() == 0)
        {
            LOG_ERROR(L"tag maps inviliad!");
            return false;
        }

        for (Lazy::LZDataPtr ptr : (*maps))
        {
            if (!TerrainMap::instance()->loadMap(ptr->value()))
            {
                LOG_ERROR(_T("load map '%s' failed!"), ptr->cvalue());
                continue;
            }

            TerrainMap::instance()->doDataConvert();
        }
    }
    else
    {

        Lazy::tstring mapName = ptrRoot->readString(L"map");
        if (mapName.empty())
        {
            LOG_ERROR(L"Read MapPath failed!");
            return false;
        }

        if (!TerrainMap::instance()->loadMap(mapName))
        {
            WRITE_LOG(_T("load map failed!"));
            return false;
        }
    }

    Lazy::LoadingMgr::instance()->init();

    WRITE_LOG(_T("TRACE: Initialize finished."));
    m_bGameStart=true;
	return true;
}

//更新
void CGame::update()
{
    __super::update();
	m_pFPS->update();

	if(!m_bGameStart)
	{
		return;
	}
    
    Lazy::LoadingMgr::instance()->dispatchFinishTask();

    m_pUpdateTaskMgr->update(m_fElapse);

	//在此添加更新代码
    if (m_pKeyboard->isKeyUp('J'))
    {
        //TerrainMap::instance()->setShowLevel(TerrainMap::instance()->getShowLevel()-1);
        TerrainMap::instance()->setShowRadius(TerrainMap::instance()->getShowRadius() - 1000.0f);
    }
    else if (m_pKeyboard->isKeyUp('K'))
    {
        //TerrainMap::instance()->setShowLevel(TerrainMap::instance()->getShowLevel()+1);
        TerrainMap::instance()->setShowRadius(TerrainMap::instance()->getShowRadius() + 1000.0f);
    }
    else if(m_pKeyboard->isKeyPress(VK_CONTROL) && m_pKeyboard->isKeyUp('S'))
    {
        TerrainMap::instance()->doDataConvert();
    }

    if (m_pKeyboard->isKeyUp('L'))
    {
        m_bUseLineMode = !m_bUseLineMode;
    }

    TerrainMap::instance()->update(m_fElapse);
	updateCamera(m_fElapse);
    m_pCamera->update(m_fElapse);
}
void CGame::updateCamera(float fEla)
{
    //////////////////////////////////////////////////////////////////////////
    //更新摄像机
    if(m_pKeyboard->isKeyUp('1'))
    {
        m_pCamera->setCamareType(CCamera::FIRST);
    }
    else if(m_pKeyboard->isKeyUp('2'))
    {
        m_pCamera->setCamareType(CCamera::THIRD);
    }
    else if (m_pKeyboard->isKeyUp('3'))
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
            CLight light;
            light.SetDirectionLight(m_pd3dDevice, D3DXVECTOR3(-1, -1, 0), 0xffffffff);
        }

        //设置材质
        CMaterial::setMaterial(m_pd3dDevice,D3DXCOLOR(0.8f,0.8f,0.8f,1.0f));

        //设置纹理
        m_pd3dDevice->SetTexture(0,NULL);

        D3DXMATRIXA16 matWord;
        D3DXMatrixIdentity(&matWord);
        m_pd3dDevice->SetTransform(D3DTS_WORLD,&matWord);
        
        if (m_bUseLineMode)
        {
            m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
        }
        else
        {
            m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        }

        __super::render();

        //m_pSkyBox->render(m_pd3dDevice);

        //渲染地面
        TerrainMap::instance()->render(m_pd3dDevice);

        //if (cPick::instance()->isIntersectWithTerrain())
        {
            Math::AABB aabb;
            aabb.min = aabb.max = cPick::instance()->getPosOnTerrain();
            float dt = 50.0f;
            aabb.min.x -= dt;
            aabb.min.z -= dt;
            aabb.max.x += dt;
            aabb.max.z += dt;
            aabb.max.y += dt * 4;

            D3DXMATRIXA16 matWord;
            D3DXMatrixIdentity(&matWord);
            m_pd3dDevice->SetTransform(D3DTS_WORLD, &matWord);
            Physics::drawAABB(m_pd3dDevice, aabb, 0xffff0000);
        }

        m_pRenderTaskMgr->render(m_pd3dDevice);
		renderText();
        m_pFPS->render();
		/////////////////////////
		m_pd3dDevice->EndScene();
	}
	else
	{
		WRITE_LOG(_T("ERROR: Render scene failed!(m_pd3dDevice->BeginScene())"));
        Sleep(100);
	}
	m_pd3dDevice->Present(NULL,NULL,NULL,NULL);
}

void CGame::renderText()
{
    Lazy::tstring buffer;
    CRect rc(0, 20, 500, 40);
    formatString(buffer, _T("show radius: %0.2f. visible chuns:%u"),
        TerrainMap::instance()->getShowRadius(), TerrainMap::instance()->numVisibleChunk());
    LPD3DXFONT pFont = m_pResMgr->getFontEx(L"");
    pFont->DrawText(NULL, buffer.c_str(), -1, &rc, DT_LEFT, 0xffffffff);
}
///////////////////class CGame end/////////////////////////