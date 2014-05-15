//Game.cpp
#include "stdafx.h"
#include "Game.h"
#include "Navigation.h"

const Lazy::tstring ConfigFile = _T("navgen.lzd");


//////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    Lazy::changeCurDirectory();
    Lazy::defaultCodePage = Lazy::CP::utf8;
    Lazy::getfs()->addResPath(L"res");

    INIT_LOG(_T("navgen.log"));

    CGame game;
    if (game.create(hInstance, L"Navigation Mesh", 800, 600, false))
    {
        game.mainLoop();
    }

    return 0;
}

class Player : public iEntity
{
public:

    virtual bool isPlayer() override { return true; }

    Player()
    {
    }

    ~Player()
    {

    }
};

//生成路点
void CGame::generateWayPoint()
{
    Physics::ChunkNodeMgr *pMgr = Physics::ChunkNodeMgr::instance();

    WRITE_LOG(_T("INFO: start generate..."));

    m_generateUseTime = Lazy::getCurTime();
    pMgr->doGenerate();
    m_generateUseTime = Lazy::getCurTime() - m_generateUseTime;

    LOG_INFO(_T("Generate finished. use time:%f"), m_generateUseTime);
}

void CGame::generateWayPointMany(Lazy::LZDataPtr maps)
{
    Physics::ChunkNodeMgr *pMgr = Physics::ChunkNodeMgr::instance();

    Lazy::LZDataBase::DataConstIterator it;
    FOR_EACH((*maps), it)
    {
        if ((*it)->tag() != _T("name"))
        {
            continue;
        }

        const Lazy::tstring & mapName = (*it)->value();
        if (mapName.empty())
        {
            WRITE_LOG(_T("ERROR: invalid mapName!"));
            continue;
        }

        if (!TerrainMap::instance()->loadMap(mapName))
        {
            WRITE_LOG(_T("ERROR: load map failed!"));
            break;
        }

        XWRITE_LOG(_T("INFO: start generate '%s'..."), mapName.c_str());

        m_generateUseTime = Lazy::getCurTime();
        if (pMgr->doGenerate())
        {
            pMgr->doSave();
        }
        m_generateUseTime = Lazy::getCurTime() - m_generateUseTime;

        XWRITE_LOG(_T("INFO: generate finished. use time:%f"), m_generateUseTime);

        if (!getApp()->processMessage())
        {
            PostQuitMessage(0);
            break;
        }
    }
}

//鼠标左键点地寻路
void CGame::searchPath()
{
    if (!cPick::instance()->queryWithTerrain()) return;
    m_destPos = cPick::instance()->getPosOnTerrain();

    m_pathData.clear();
    m_searchTime = Lazy::getCurTime();

    if (!m_player->getPhysics()->searchPathEx(m_pathData, m_destPos))
    {
        debugMessage(_T("ERORR: find way failed! "));
    }

    m_searchTime = Lazy::getCurTime() - m_searchTime;
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
LRESULT CGame::wndProc(HWND hWnd, UINT uMsg,
    WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_KEYUP)
    {
        bool isCtlDown = m_pKeyboard->isKeyPress(VK_CONTROL);
        switch (wParam)
        {
        case '1':
            m_pCamera->setCamareType(CCamera::FIRST);
            break;
        case '2':
            m_pCamera->setCamareType(CCamera::THIRD);
            break;
        case '3':
            m_pCamera->setCamareType(CCamera::FREE);
            break;
        case 'J'://减少可视范围
            TerrainMap::instance()->setShowLevel(TerrainMap::instance()->getShowLevel() - 1);
            break;
        case 'K'://增加可视范围
            TerrainMap::instance()->setShowLevel(TerrainMap::instance()->getShowLevel() + 1);
            break;
        case 'G'://计算路点
            if (isCtlDown)
            {
                m_generateWp = !m_generateWp;
                if (m_generateWp) TerrainMap::instance()->loadAllChunks();
            }
            break;
        case 'S'://保存路点
            if (isCtlDown)
            {
                Physics::ChunkNodeMgr::instance()->doSave();
            }
            break;
        case VK_RETURN://刷新路点数据
            {
                Physics::WayChunkPtr ptr = Physics::WayChunkMgr::instance()->getChunkByPos(
                    m_pCamera->position().x, m_pCamera->position().z);

                if (ptr) m_focusWayChunk = ptr->getId();
            }
            break;
        case VK_F1:
            Physics::NagenDebug::showMesh = !Physics::NagenDebug::showMesh;
            break;
        case VK_F2:
            Physics::NagenDebug::showLayer = !Physics::NagenDebug::showLayer;
            break;
        case VK_F3:
            Physics::NagenDebug::showHF = !Physics::NagenDebug::showHF;
            break;
        case VK_F4:
            Physics::NagenDebug::showTriangle = !Physics::NagenDebug::showTriangle;
            break;
        case VK_F5:
            Physics::NagenDebug::showWP = !Physics::NagenDebug::showWP;
            break;
        case VK_F6:
            Physics::NagenDebug::showPath = !Physics::NagenDebug::showPath;
            break;
        case VK_F7:
            MapConfig::ShowTerrain = !MapConfig::ShowTerrain;
            break;
        }
    }

    if (m_pCamera && m_pCamera->handleMessage(uMsg, wParam, lParam)) return 0;

    cPick::instance()->handleMouseEvent(uMsg, wParam, lParam);

    if (uMsg == WM_LBUTTONUP) searchPath();

    return CApp::wndProc(hWnd, uMsg, wParam, lParam);
}

CGame::CGame()
    : m_destPos(0, 0, 0)
{
    m_bGameStart = false;

    m_generateWp = false;

    m_generateUseTime = 0.0f;
    m_searchTime = 0.0f;

    m_focusWayChunk = 0;
}

void CGame::clear()
{
    WRITE_LOG(_T("start clear..."));
    Lazy::LoadingMgr::instance()->fini();

    CApp::clear();

    WRITE_LOG(_T("clear finished."));
}

CGame::~CGame()
{
}

/*游戏初始化*/
bool CGame::init(void)
{
    WRITE_LOG(_T("start initialize..."));
    CApp::init();

    //在此添加初始化代码
    srand(timeGetTime());

    m_bGameStart = true;

    m_pFPS = new cFpsRender();
    m_pFPS->init();

    m_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0xff7f7f7f);

    //创建player
    m_player = new Player();
    addUpdateRender(m_player.get());
    m_player->m_vPos.set(0.0f, 0.0f, 0.0f);
    m_player->setSpeed(500.0f);
    m_player->setPhysics(new iPhysics(m_player.get()));
    ModelPtr model = ModelFactory::loadModel(L"model/jingtian/jingtian.x", ModelType::SkinModel);
    if (model)
    {
        model->setYaw(3.14f);
        m_player->setModel(model);
    }

    TerrainMap::instance()->setSource(m_player.get());

    MapConfig::UseNewFormat = false;
    MapConfig::UseMultiThread = true;

    //创建相机
    m_pCamera = new CCursorCamera(CCamera::THIRD);
    m_pCamera->setSource(m_player.get());
    m_pCamera->setPosition(Math::Vector3(100, 2000.0f, -500.0f));
    m_pCamera->setSpeed(500.0f);
    m_pCamera->setDistRange(10.0f, 1000.0f);
    m_pCamera->setNearFar(1.0f, 10000.0f);
    m_pCamera->setDistance(200.0f);

    //设置寻路配置
    Physics::FindWay::Config.clampHeight = 30.0f;
    Physics::FindWay::Config.modelHeight = 80.0f;
    Physics::FindWay::Config.cubeSize = 100.0f;
    Physics::FindWay::Config.minSetPoints = 8;

    //加载地图
    Lazy::LZDataPtr root = Lazy::openSection(ConfigFile);
    if (!root)
    {
        LOG_ERROR(_T("Load game config file '%s' failed!"), ConfigFile.c_str());
        return false;
    }


    Lazy::tstring mapName = root->readString(_T("workmap"));
    if (!mapName.empty())
    {
        if (!TerrainMap::instance()->loadMap(mapName))
        {
            LOG_ERROR(_T("Load map failed!"));
            return false;
        }
    }
    else
    {
        Lazy::LZDataPtr maps = root->read(_T("workgroup"));
        if (maps)
        {
            generateWayPointMany(maps);
        }
        else
        {
            LOG_ERROR(_T("Doen't found any config map!"));
            return false;
        }
    }

    Lazy::LoadingMgr::instance()->init();

    LOG_INFO(_T("initialize finished."));
    return true;
}

//更新
void CGame::update()
{
    CApp::update();
    m_pFPS->update();

    Lazy::LoadingMgr::instance()->dispatchFinishTask();

    //在此添加更新代码
    m_pUpdateTaskMgr->update(m_fElapse);

    //如果所有chunk都加载出来了，就开始计算路点
    if (m_generateWp && TerrainMap::instance()->isAllChunkLoaded())
    {
        generateWayPoint();
        m_generateWp = false;
    }

    m_pCamera->update(m_fElapse);
}

//渲染
void CGame::render()
{
    Lazy::rcDevice()->clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 100), 1.0f, 0);
    if (Lazy::rcDevice()->beginScene())
    {
        CLight light;
        light.SetDirectionLight(m_pd3dDevice,
            D3DXVECTOR3(0, -1.0f, 0),
            D3DXCOLOR(0.8f, 0.8f, 0.8f, 0.8f));

        //添加渲染代码
        m_pCamera->render(m_pd3dDevice);
        CApp::render();

        m_pRenderTaskMgr->render(m_pd3dDevice);

        //渲染路点数据
        m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

        Math::Matrix4x4 matIdentity;
        matIdentity.makeIdentity();
        m_pd3dDevice->SetTransform(D3DTS_WORLD, &matIdentity);
        m_pd3dDevice->SetTexture(0, NULL);

        //路点数据
        if (Physics::NagenDebug::showWP)
        {
            //Physics::WayChunkMgr::instance()->render(m_pd3dDevice);
            Physics::WayChunkPtr ptr = Physics::WayChunkMgr::instance()->getChunk(m_focusWayChunk);
            if (ptr) ptr->render(m_pd3dDevice);
        }

        //绘制寻路路径
        if (Physics::NagenDebug::showPath && !m_pathData.empty())
        {
            Physics::WpPtrArray::iterator it;
            FOR_EACH(m_pathData, it)
            {
                (*it)->render(m_pd3dDevice, 0xffff0000);
            }
        }

        //绘制寻路目标点
        {
            Physics::AABB aabb;
            Physics::makeRoleAABB(Physics::FindWay::Config, aabb, m_destPos);
            Physics::drawAABB(m_pd3dDevice, aabb, 0xff0000ff);
        }

        m_pFPS->render();

        //绘制帮助文本
        LPD3DXFONT pFont = getResMgr()->getFontEx(L"宋体/17/0/b");
        if (pFont)
        {
            Lazy::tstring strInfo;
            DWORD color = 0xffffff00;
            DWORD dh = 20;
            CRect rc = getClientRect();

            rc.top += 20;
            formatString(strInfo, _T("gen wp use:%fs"), m_generateUseTime);
            pFont->DrawText(NULL, strInfo.c_str(), -1, &rc, DT_LEFT, color);

            rc.top += 20;
            formatString(strInfo, _T("search use:%fs. num waypoint:%u"), m_searchTime, m_pathData.size());
            pFont->DrawText(NULL, strInfo.c_str(), -1, &rc, DT_LEFT, color);

            Physics::WayChunkPtr pChunk = Physics::WayChunkMgr::instance()->getChunk(0, false);
            if (pChunk)
            {
                formatString(strInfo, _T("[chunk 0]waySet num: %d, wayPoint num: %d"),
                    pChunk->numWaySet(), pChunk->numWayPoint());
                rc.top += 20;
                pFont->DrawText(NULL, strInfo.c_str(), -1, &rc, DT_LEFT | DT_WORDBREAK, color);
            }

            strInfo = L" [F1]-show mesh\r\n"
                L" [F2]-show layer\r\n"
                L" [F3]-show height field\r\n"
                L" [F4]-show triangle\r\n"
                L" [F5]-show way point(Enter key refresh to camera.)\r\n"
                L" [F6]-show path\r\n [Ctrl+G]-generate waypoint\r\n [Ctrl+S]-save waypoints\r\n"
                L" [F7]-show terrain ";
            rc.top = rc.bottom - dh * 8;

            pFont->DrawText(NULL, strInfo.c_str(), -1, &rc, DT_LEFT | DT_WORDBREAK, color);
        }
        /////////////////////////
        Lazy::rcDevice()->endScene();
    }
    else
    {
        LOG_ERROR(_T("Render scene failed!"));
        Sleep(100);
    }

    Lazy::rcDevice()->present();
}

///////////////////class CGame end/////////////////////////