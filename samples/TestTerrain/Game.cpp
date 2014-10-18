//Game.cpp
#include "Game.h"
#include "Physics/PhysicsDebug.h"

//#include "resource.h"

static const char * compileTime = __DATE__ " " __TIME__;

//////////////////////////////////////////////////////////////////////////


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    CGame g_game;

    Lazy::defaultCodePage = Lazy::CP::utf8;
    Lazy::changeCurDirectory();
    Lazy::getfs()->addResPath(L"res/");

    INIT_LOG(_T("TestTerrain.log"));
    LOG_DEBUG(L"compile time: %S", compileTime);

    if (g_game.create(hInstance, L"Terrain", 900, 600, false))
    {
        g_game.mainLoop();
    }

    return 0;
}

class Player : public IEntity
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

bool CGame::onEvent(const SEvent & event)
{
    if (m_bGameStart)
    {
        if (m_pCamera->handleEvent(event))
            return true;

        else if (Pick::instance()->handeEvent(event))
            return true;

        if (TerrainMap::instance()->handeEvent(event))
            return true;
    }

    return CApp::onEvent(event);
}


CGame::CGame()
    : m_pGuiMgr(nullptr)
    , m_bGameStart(false)
    , m_bUseLineMode(false)
{
}

void CGame::clear()
{
    LOG_DEBUG(_T("Start clear..."));

    //TerrainMap::instance()->saveMap("");
    Lazy::LoadingMgr::instance()->fini();

    m_pGuiMgr->destroy();
    CApp::clear();

    LOG_DEBUG(_T("Clear finished."));
}
CGame::~CGame()
{
    delete m_pGuiMgr;
}

/*游戏初始化*/
bool CGame::init(void)
{
    LOG_ERROR(_T("Start initialize..."));
    CApp::init();

    Lazy::LZDataPtr ptrRoot = Lazy::openSection(L"terrain.lzd");
    if (!ptrRoot)
    {
        LOG_DEBUG(_T("load game config failed!"));
        return false;
    }

    m_pGuiMgr = new GUIMgr(m_pd3dDevice, m_hWnd, m_hInstance);

    g_player = new Player();
    g_player->setPhysics(new IPhysics());
    g_player->setSpeed(8.0f);

    ModelPtr model = ModelFactory::loadModel(L"model/jingtian/jingtian.x", ModelType::SkinModel);
    if (model)
    {
        model->setScale(0.025f);
        model->setYaw(3.14159f);
        g_player->setModel(model);
    }

    addUpdateRender(g_player.get());
    TerrainMap::instance()->setSource(g_player.get());

    m_pCamera = new CCursorCamera(Camera::THIRD);
    m_pCamera->setSource(g_player.get());
    m_pCamera->setSpeed(10.0f);

    Lazy::LZDataPtr ptrPos = ptrRoot->read(L"camera/pos");
    m_pCamera->setPosition(D3DXVECTOR3(
        ptrPos->readFloat(L"x"),
        ptrPos->readFloat(L"y"),
        ptrPos->readFloat(L"z")));
    m_pCamera->setSpeed(ptrRoot->readFloat(L"camera/speed"));
    m_pCamera->setNearFar(1.0f, 1000.0f);
    m_pCamera->setDistRange(2.0f, 20.0f);
    m_pCamera->setDistance(6.0f);

    m_pFPS = new FpsRender();
    m_pFPS->init();

    m_pCube = new CCube();
    m_pCube->init(m_pd3dDevice, 1.0f, 1.0f, 1.0f);

    m_pSkyBox->setSource(g_player.get());
    m_pSkyBox->setSkyImage(ptrRoot->readString(L"sky"));
    m_pSkyBox->setSkyRange(D3DXVECTOR3(-200, -200, -200), D3DXVECTOR3(200, 200, 200));


    Lazy::tstring mapName = ptrRoot->readString(L"map");
    if (mapName.empty())
    {
        LOG_ERROR(L"Read MapPath failed!");
        return false;
    }

    if (!TerrainMap::instance()->loadMap(mapName))
    {
        LOG_ERROR(_T("load map failed!"));
        return false;
    }

    Lazy::LoadingMgr::instance()->init();

    LOG_DEBUG(_T("Initialize finished."));
    m_bGameStart = true;
    return true;
}

//更新
void CGame::update()
{
    CApp::update();
    m_pFPS->update();

    if (!m_bGameStart)
    {
        return;
    }

    Lazy::LoadingMgr::instance()->dispatchFinishTask();

    m_pUpdateTaskMgr->update(m_fElapse);

    //在此添加更新代码
    if (m_pKeyboard->isKeyUp('J'))
    {
        TerrainMap::instance()->setShowRadius(TerrainMap::instance()->getShowRadius() - 50.0f);
    }
    else if (m_pKeyboard->isKeyUp('K'))
    {
        TerrainMap::instance()->setShowRadius(TerrainMap::instance()->getShowRadius() + 50.0f);
    }
    else if (m_pKeyboard->isKeyPress(VK_CONTROL) && m_pKeyboard->isKeyUp('S'))
    {
        TerrainMap::instance()->saveMap();
    }
    else if (m_pKeyboard->isKeyUp('L'))
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
    if (m_pKeyboard->isKeyUp('1'))
    {
        m_pCamera->setCamareType(Camera::FIRST);
    }
    else if (m_pKeyboard->isKeyUp('2'))
    {
        m_pCamera->setCamareType(Camera::THIRD);
    }
    else if (m_pKeyboard->isKeyUp('3'))
    {
        m_pCamera->setCamareType(Camera::FREE);
    }

}
//渲染
void CGame::render()
{
    rcDevice()->clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 100), 1.0f, 0);

    if (rcDevice()->beginScene())
    {
        //添加渲染代码
        m_pCamera->render(m_pd3dDevice);

        {
            CLight light;
            D3DXVECTOR3 dir(-1, -1, -1);
            D3DXVec3Normalize(&dir, &dir);
            light.SetDirectionLight(m_pd3dDevice, dir, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
        }

        //设置材质
        CMaterial::setMaterial(m_pd3dDevice, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

        //设置纹理
        m_pd3dDevice->SetTexture(0, NULL);

        D3DXMATRIXA16 matWord;
        D3DXMatrixIdentity(&matWord);
        m_pd3dDevice->SetTransform(D3DTS_WORLD, &matWord);

        if (m_bUseLineMode)
        {
            m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
        }
        else
        {
            m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        }

        CApp::render();

        m_pSkyBox->render(m_pd3dDevice);

        //渲染地面
        TerrainMap::instance()->render(m_pd3dDevice);

        //if (cPick::instance()->isIntersectWithTerrain())
        {
            AABB aabb;
            aabb.min = aabb.max = Pick::instance()->getPosOnTerrain();
            float dt = 0.5f;
            aabb.min.x -= dt;
            aabb.min.z -= dt;
            aabb.max.x += dt;
            aabb.max.z += dt;
            aabb.max.y += dt * 4;

            D3DXMATRIXA16 matWord;
            D3DXMatrixIdentity(&matWord);
            m_pd3dDevice->SetTransform(D3DTS_WORLD, &matWord);
            drawAABB(m_pd3dDevice, aabb, 0xffff0000);
        }

        m_pRenderTaskMgr->render(m_pd3dDevice);
        renderText();
        m_pFPS->render();
        /////////////////////////
        rcDevice()->endScene();
    }
    else
    {
        LOG_DEBUG(_T("ERROR: Render scene failed!(m_pd3dDevice->BeginScene())"));
        Sleep(100);
    }

    rcDevice()->present();
}

void CGame::renderText()
{
#if 0
    Lazy::tstring buffer;
    CRect rc(0, 20, 500, 40);
    formatString(buffer, _T("show radius: %0.2f. visible chuns:%u"),
        TerrainMap::instance()->getShowRadius(), TerrainMap::instance()->numVisibleChunk());
    LPD3DXFONT pFont = m_pResMgr->getFontEx(L"");
    pFont->DrawText(NULL, buffer.c_str(), -1, &rc, DT_LEFT, 0xffffffff);
#endif
}
///////////////////class CGame end/////////////////////////