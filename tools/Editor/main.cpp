//Game.cpp

#include "targetver.h"

#include "main.h"
#include "Physics/PhysicsDebug.h"

#include "resource.h"

const char * compileTime = __DATE__ " " __TIME__;

namespace
{
    const Lazy::tstring ConfigFile = _T("editor.lzd");
    std::wstring gameModule;
}

//////////////////////////////////////////////////////////////////////////


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    CGame g_game;

    Lazy::defaultCodePage = Lazy::CP::utf8;
    Lazy::changeCurDirectory();
    Lazy::getfs()->addResPath(L"res/");

    INIT_LOG(_T("Editor.log"));
    LOG_DEBUG(L"compile time: %S", compileTime);

    if (g_game.create(hInstance, L"Editor", 900, 600, false))
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

    bool ifPlayer() const override { return true; }
};

EntityPtr g_player;

///////////////////class CGame start/////////////////////////

/*注册窗口类消息。请实现该消息来修改窗口风格。*/
void CGame::onRegisterClass(WNDCLASSEX *pwc)
{
    pwc->hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_TESTPY));
    pwc->hIconSm = pwc->hIcon;
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
    : m_guimgr(nullptr)
    , m_bGameStart(false)
    , m_bUseLineMode(false)
{
}

CGame::~CGame()
{
}

bool initPython()
{
    Lazy::LZDataPtr root = Lazy::openSection(ConfigFile);

    std::wstring pyHome = root->readString(L"python/home");
    if (pyHome.empty())
    {
        LOG_ERROR(L"Python home was not found in config file!");
        return false;
    }

    Lazy::LZDataPtr paths = root->read(L"python/paths");
    if (!paths)
    {
        LOG_ERROR(L"Python script path was not found in config file!");
        return false;
    }

    gameModule = root->readString(L"python/entry");
    if (gameModule.empty())
    {
        LOG_ERROR(L"Invalid python entry module.");
        return false;
    }

    Py_SetPythonHome(&pyHome[0]);

    PyImport_AppendInittab("helper", Lzpy::PyInit_helper);
    PyImport_AppendInittab("Lazy", Lzpy::PyInit_Lazy);

    Py_Initialize();
    if (!Py_IsInitialized())
    {
        LOG_ERROR(L"Initialize python failed!");
        return false;
    }

    try
    {
        //初始化内置模块
        Lzpy::import("helper");
        Lzpy::import("Lazy");

        Lzpy::LzpyResInterface::initAll();

        //输出重定向
        Lzpy::object sys = Lzpy::import(L"sys");
        if (!sys) return false;

        Lzpy::object out = Lzpy::new_reference(Lzpy::new_instance_ex<Lzpy::PyOutput>());
        sys.setattr("stdout", out);
        sys.setattr("stderr", out);
        PyRun_SimpleString("print('---------1234')");


        //设置脚本路径
        for (Lazy::LZDataPtr lzd : (*paths))
        {
            if (lzd->tag() == L"path")
            {
                Lazy::tstring path;
                if (!Lazy::getfs()->getRealPath(path, lzd->value()))
                {
                    LOG_ERROR(L"Invalid script path %s", path.c_str());
                    continue;
                }

                LOG_INFO(L"add script path: %s", path.c_str());
                Lzpy::addSysPath(path);
            }
        }

        //脚本入口
        Lzpy::object mygame = Lzpy::import(gameModule);
        if (!mygame)
        {
            if (PyErr_Occurred())
                PyErr_Print();

            LOG_ERROR(L"Can't find script %s!", gameModule.c_str());
            return false;
        }

        if (!mygame.call_method("init"))
        {
            if (PyErr_Occurred())
                PyErr_Print();

            LOG_ERROR(L"init script failed!");
            return false;
        }
    }
    catch (Lzpy::python_error & e)
    {
        LOG_ERROR(L"Python Error in initPython: %S", e.what());

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
        Lzpy::object mygame = Lzpy::import(gameModule);
        mygame.call_method("fini");

        Lzpy::LzpyResInterface::finiAll();
    }
    catch (Lzpy::python_error & e)
    {
        LOG_ERROR(L"Python Error in finiPython : %S", e.what());

        if (PyErr_Occurred())
            PyErr_Print();
        return false;
    }


    Py_Finalize();
    return true;
}

/*游戏初始化*/
bool CGame::init(void)
{
    LOG_DEBUG(_T("Start initialize..."));
    CApp::init();

    Lazy::LZDataPtr ptrRoot = Lazy::openSection(ConfigFile);
    if (!ptrRoot)
    {
        LOG_DEBUG(_T("load game config failed!"));
        return false;
    }


    m_guimgr = new GUIMgr(m_pd3dDevice, m_hWnd, m_hInstance);
    m_fpsLabel = new Label();
    m_fpsLabel->setPosition(0, 0);
    m_guimgr->addChild(m_fpsLabel.get());

    m_playerInfo = new Label();
    m_playerInfo->setPosition(0, 20);
    m_playerInfo->setMutiLine(true);
    m_playerInfo->setSize(500, 20);
    m_guimgr->addChild(m_playerInfo.get());

    g_player = new Player();
    g_player->setPhysics(new IPhysics());
    g_player->setSpeed(Vector3(8.0f, 8.0, 8.0f));
    g_player->setPosition(Vector3(-32, 4.0f, 46.0f));
    EntityMgr::instance()->add(g_player);
    
    tstring resource = L"model/jingtian/jingtian.x";
    //tstring resource = L"model/1000/m.x";
    ModelPtr model = ModelFactory::loadModel(resource);
    if (model)
    {
        model->setScale(Vector3(0.025f, 0.025f, 0.025f));
        model->rotationY(D3DX_PI);
        g_player->setModel(model.get());
    }

    TerrainMap::instance()->setSource(g_player.get());

    m_pCamera = new Camera(Camera::THIRD);
    m_pCamera->setSource(g_player.get());

    Lazy::LZDataPtr ptrPos = ptrRoot->read(L"camera/pos");
    m_pCamera->setPosition(D3DXVECTOR3(
        ptrPos->readFloat(L"x"),
        ptrPos->readFloat(L"y"),
        ptrPos->readFloat(L"z")));
    m_pCamera->setSpeed(ptrRoot->readFloat(L"camera/speed", 20.0f));
    m_pCamera->setDistRange(1.8f, 20.0f);
    m_pCamera->setHeight(1.0f);

    m_projection.setPerspective(D3DX_PI / 4.0f, float(m_nWidth) / m_nHeight, 1.0f, 1000.0f);

    m_pSkyBox = new SkyBox();
    addDrawTask(m_pSkyBox.get());
    m_pSkyBox->setSource(m_pCamera.get());
    m_pSkyBox->setSkyImage(ptrRoot->readString(L"sky"));
    m_pSkyBox->setSkyRange(Vector3(-200, -200, -200), Vector3(200, 200, 200));


    Lazy::tstring mapName = ptrRoot->readString(L"map");
    if (mapName.empty())
    {
        LOG_ERROR(L"Read MapPath failed!");
        return false;
    }

#if 1
    if (!TerrainMap::instance()->loadMap(mapName))
    {
        LOG_ERROR(_T("load map failed!"));
        return false;
    }
#endif

    Lazy::LoadingMgr::instance()->init();

    if (!initPython())
        return false;

    LOG_DEBUG(_T("Initialize finished."));
    m_bGameStart = true;
    return true;
}

void CGame::clear()
{
    LOG_DEBUG(_T("Start clear..."));

    finiPython();

    Lazy::LoadingMgr::instance()->fini();

    m_guimgr->destroy();
    CApp::clear();

    LOG_DEBUG(_T("Clear finished."));
}

//更新
void CGame::update()
{
    CApp::update();

    {
        tstring text;
        formatString(text, _T("FPS: %g"), m_fps.getFps());
        m_fpsLabel->setText(text);

        Vector3 pos = g_player->getPosition();
        Vector3 look = g_player->getLook();
        formatString(text, _T("Position: %g %g %g\nLook: %g %g %g"),
            pos.x, pos.y, pos.z,
            look.x, look.y, look.z);
        m_playerInfo->setText(text);
    }

    m_guimgr->update(m_fElapse);

    Lazy::LoadingMgr::instance()->dispatchFinishTask();

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
        TerrainMap::instance()->loadAllChunks(true);
        TerrainMap::instance()->saveMap();
        TerrainMap::instance()->loadAllChunks(false);
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
    //添加渲染代码
    rcDevice()->setView(m_pCamera->getViewMatrix());
    rcDevice()->setProj(m_projection.getProjection());

    rcDevice()->applyWorld();
    rcDevice()->applyView();
    rcDevice()->applyProj();

    {
        LightHelper light;
        D3DXVECTOR3 dir(-1, -1, -1);
        D3DXVec3Normalize(&dir, &dir);
        light.SetDirectionLight(m_pd3dDevice, dir, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
    }

    //设置材质
    Material::setMaterial(m_pd3dDevice, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

    //设置纹理
    m_pd3dDevice->SetTexture(0, NULL);
    m_pd3dDevice->SetTransform(D3DTS_WORLD, &matIdentity);

    m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, 
        m_bUseLineMode ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
    

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

        m_pd3dDevice->SetTransform(D3DTS_WORLD, &matIdentity);
        drawAABB(m_pd3dDevice, aabb, 0xffff0000);
    }

    CApp::render();
    m_guimgr->render(m_pd3dDevice);
}

///////////////////class CGame end/////////////////////////
