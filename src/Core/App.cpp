//D3DApp.cpp

#include "stdafx.h"
#include "Core.h"
#include "App.h"

#include "../Render/Texture.h"
#include "../Font/Font.h"
#include "../Render/RenderDevice.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")

#define CLASS_NAME _T("Lazy3dClass")

namespace Lazy
{

    static CApp * g_pApp = NULL;

    /** 获得游戏类指针。*/
    LZDLL_API CApp * getApp(void)
    {
        return g_pApp;
    }

    //////////////////////////////////////////////////////////////////////////
    LRESULT CALLBACK app_defWndProc_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (msg == WM_DESTROY)
        {
            PostQuitMessage(0);
            return 0 ;
        }

        if (g_pApp != nullptr && processWindowMsg(g_pApp, hWnd, msg, wParam, lParam))
            return 0;

        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    ///////////////////class CApp start/////////////////////////
    CApp::CApp()
        : m_isLoading(false)
        , m_isLoadingOK(false)
        , m_loadingElapse(0.0f)
        , m_loadingType(LoadingType::None)
        , m_timeScale(1.0f)
        , m_pd3dDevice(nullptr)
        , m_hWnd(nullptr)
        , m_hInstance(nullptr)
        , m_bFullScreen(false)
        , m_nWidth(640)
        , m_nHeight(480)
        , m_bMsgHooked(false)
        , m_fElapse(0.0f)
    {
        g_pApp = this;

        createSingleton();
    }

    CApp::~CApp()
    {
        releaseSingleton();

        g_pApp = NULL;
        SafeRelease(m_pd3dDevice);
    }

    void CApp::createSingleton()
    {
        MemoryChecker::initInstance();
        MemoryPool::init();
        TerrainMap::initInstance();
    }

    void CApp::releaseSingleton()
    {
        cSoundMgr::instance()->release();
        TerrainMap::finiInstance();

        //////////////////////////////////////////////////////////////////////////
        clearSectionCache();
        MemoryPool::fini();
        MemoryChecker::finiInstance();
    }

    bool CApp::create(HINSTANCE hInstance, const std::wstring & caption,
                      int nWidth, int nHeight, bool bFullScr)
    {
        m_hInstance = hInstance;
        m_nWidth = nWidth;
        m_nHeight = nHeight;
        m_bFullScreen = bFullScr;
        m_caption = caption;

        LOG_INFO(L"register window class...");
        registerClass();//注册窗口类

        LOG_INFO(L"create window...");
        if (!createWindow())//创建窗口
        {
            LOG_ERROR(L"create window failed!");
            return false;
        }

        return createEx(hInstance, m_hWnd, caption, nWidth, nHeight, bFullScr);
    }

    bool CApp::createEx(HINSTANCE hInstance,
                        HWND hWnd,
                        const std::wstring & strCaption,
                        int nWidth,
                        int nHeight,
                        bool bFullSrcreen)
    {
        m_hInstance = hInstance;
        m_hWnd = hWnd;
        m_nWidth = nWidth;
        m_nHeight = nHeight;
        m_bFullScreen = bFullSrcreen;
        m_caption = strCaption;


        LOG_INFO(L"create d3d device...");
        if (!createDevice())//创建设备
        {
            LOG_ERROR(L"create d3d device failed!");
            return false;
        }


        LOG_INFO(L"initialize game...");
        if (!init())//初始化游戏
        {
            LOG_ERROR(L"initialize failed!");
            return false;
        }

        return true;
    }


    void CApp::mainLoop(void)
    {
        LOG_INFO(_T("start game loop."));
        static MSG msg = {0};
        while(msg.message != WM_QUIT)
        {
            if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                if(GetActiveWindow() != getHWnd())
                {
                    Sleep(100);
                }
                run();
            }
        }
        LOG_INFO(_T("game loop end."));

        clear();//释放资源
    }

    ///处理消息。返回false，表示程序退出。
    bool CApp::processMessage()
    {
        static MSG msg = {0};
        while(msg.message != WM_QUIT)
        {
            if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else break;
        }

        return msg.message != WM_QUIT;
    }

    /**运行*/
    void CApp::run(void)
    {
        DWORD curTime = timeGetTime();
        static DWORD oldUpdateTime = curTime;
        m_fElapse = (curTime - oldUpdateTime) * 0.001f;
        m_fElapse *= m_timeScale;

        //防止时间为0，引发浮点数除0。
        if (m_fElapse < 0.00001f) return;

        oldUpdateTime = curTime;

        update();//更新逻辑
        render();//渲染画面

        //Sleep(100);
    }

    /** 退出游戏*/
    void CApp::quitGame(void)
    {
        PostQuitMessage(0);
    }


    //初始化Direct3D对象及设备对象
    bool CApp::createDevice()
    {
        if (!rcDevice()->create(m_hWnd, m_hInstance, m_bFullScreen))
            return false;

        m_pd3dDevice = rcDevice()->getDevice();
        m_pd3dDevice->AddRef();

        return true;
    }


    //创建窗口
    bool CApp::createWindow()
    {
        DWORD dwStyle ;
        int x, y;
        int width, height;
        if (m_bFullScreen)
        {
            dwStyle = WS_VISIBLE | WS_POPUP;
            x = 0;
            y = 0;
            width = m_nWidth = GetSystemMetrics(SM_CXSCREEN);
            height = m_nHeight = GetSystemMetrics(SM_CYSCREEN);
        }
        else
        {
            width = m_nWidth + ::GetSystemMetrics(SM_CXBORDER) * 2;
            height = m_nHeight + ::GetSystemMetrics(SM_CYBORDER) + ::GetSystemMetrics(SM_CYCAPTION);

            x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
            y = (GetSystemMetrics(SM_CYSCREEN) - height - 30) / 2;
            dwStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        }
        m_hWnd = CreateWindowW(
                     CLASS_NAME,
                     m_caption.c_str(),
                     dwStyle,
                     x,
                     y,
                     width,
                     height,
                     NULL,
                     NULL,
                     m_hInstance,
                     NULL);

        if (m_hWnd)
        {
            ShowWindow(m_hWnd, SW_SHOW);
            UpdateWindow(m_hWnd);
        }

        return (m_hWnd != NULL);
    }

    //注册窗口类
    void CApp::registerClass()
    {
        WNDCLASSEX wc;
        memset(&wc, 0, sizeof(wc));
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = (WNDPROC)app_defWndProc_;
        wc.hInstance = m_hInstance;
        wc.lpszClassName = CLASS_NAME;
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hIconSm = wc.hIcon;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.style = CS_CLASSDC;

        onRegisterClass(&wc);

        RegisterClassEx(&wc);
    }


    /**游戏初始化*/
    bool CApp::init(void)
    {
        srand(timeGetTime());

        m_pRenderTaskMgr = new CRenderTask();
        m_pUpdateTaskMgr = new CRenderTask();
        m_pBillboardMgr = new CRenderTask();

        FontMgr::instance()->init();
        FontMgr::instance()->registerFontFile(L"C:/Windows/Fonts/simhei.ttf", L"def");

        m_pKeyboard = new CKeyboard();
        addUpdater(m_pKeyboard.get());

        addUpdateRender(TerrainMap::instance());

        addUpdateRender(EntityMgr::instance());
        addUpdater(CallbackMgr::instance());

        m_pSkyBox = new SkyBox();
        addRender(m_pSkyBox.get());
        return true;
    }

    /**游戏更新*/
    void CApp::update(void)
    {

    }

    /**渲染*/
    void CApp::render(void)
    {

    }

    /**释放资源*/
    void CApp::clear(void)
    {
        m_pSkyBox = NULL;
        m_pRenderTaskMgr = NULL;
        m_pUpdateTaskMgr = NULL;
        m_pBillboardMgr = NULL;
        m_pKeyboard = NULL;

        FontMgr::instance()->fini();
        EntityMgr::instance()->clear();
    }

    /**注册窗口类消息。请实现该消息来修改窗口风格。*/
    void CApp::onRegisterClass(WNDCLASSEX *)
    {

    }

    /**创建设备消息。实现该方法来修改设备。*/
    void CApp::onCreateDevice(D3DPRESENT_PARAMETERS *)
    {

    }

    bool CApp::onEvent(const SEvent & event)
    {
        if (event.eventType == EET_KEY_EVENT)
        {
            if (!event.keyEvent.down && event.keyEvent.key == KEY_ESCAPE)
            {
                DestroyWindow(m_hWnd);
                return true;
            }
        }

        return false;
    }


    void CApp::setCaption(std::wstring const & caption)
    {
        m_caption = caption;
        SetWindowTextW(m_hWnd, m_caption.c_str());
    }

    void CApp::screenToClient(POINT* pt)
    {
        ScreenToClient(m_hWnd, pt);
    }

    void CApp::clientToScreen(POINT* pt)
    {
        ClientToScreen(m_hWnd, pt);
    }

    CPoint CApp::getCursorPos(void)
    {
        CPoint pt;
        GetCursorPos(&pt);
        screenToClient(&pt);
        return pt;
    }

    void CApp::setCursorPos(CPoint pt)
    {
        clientToScreen(&pt);
        SetCursorPos(pt.x, pt.y);
    }

    CRect CApp::getClientRect(void)
    {
        CRect rc;
        GetClientRect(m_hWnd, &rc);
        return rc;
    }

    void CApp::addRender(IRenderObj* rend)
    {
        m_pRenderTaskMgr->add(rend);
    }
    void CApp::addUpdater(IRenderObj* up)
    {
        m_pUpdateTaskMgr->add(up);
    }

    void CApp::addUpdateRender(IRenderObj *pObj)
    {
        addUpdater(pObj);
        addRender(pObj);
    }

    void CApp::addBillboard(IRenderObj* pObj)
    {
        m_pBillboardMgr->add(pObj);
    }

    void CApp::removeRender(IRenderObj* rend)
    {
        if (!m_pRenderTaskMgr) return;

        m_pRenderTaskMgr->remove(rend);
    }

    void CApp::removeUpdater(IRenderObj* up)
    {
        if (!m_pUpdateTaskMgr) return;

        m_pUpdateTaskMgr->remove(up);
    }

    void CApp::removeUpdateRender(IRenderObj* pObj)
    {
        removeUpdater(pObj);
        removeRender(pObj);
    }

    void CApp::removeBillboard(IRenderObj* pObj)
    {
        if (m_pBillboardMgr) return;

        m_pBillboardMgr->remove(pObj);
    }

    void CApp::startGameLoading(int type)
    {
        m_isLoading = true;
        m_loadingType = type;
        m_loadingElapse = 0.0f;
    }

///////////////////class CApp end/////////////////////////

} // end namespace Lazy
