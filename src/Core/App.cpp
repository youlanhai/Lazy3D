//D3DApp.cpp

#include "stdafx.h"
#include "App.h"

#include "TerrainMap.h"
#include "Pick.h"
#include "Sound.h"
#include "Entity.h"
#include "SceneNodeFactory.h"

#include "../Render/Texture.h"
#include "../Render/Font.h"
#include "../Render/RenderDevice.h"
#include "../Render/ShadowMap.h"
#include "../Render/Mesh.h"

//#pragma comment(lib, "d3d9.lib")
//#pragma comment(lib, "d3dx9.lib")
//#pragma comment(lib, "winmm.lib")

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
    static LRESULT CALLBACK defaultWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
        : m_timeScale(1.0f)
        , m_pd3dDevice(nullptr)
        , m_hWnd(nullptr)
        , m_hInstance(nullptr)
        , m_bFullScreen(false)
        , m_bWindowMinimized(false)
        , m_nWidth(640)
        , m_nHeight(480)
        , m_bMsgHooked(false)
        , m_fElapse(0.0f)
        , m_oldWindowStyle(0)
        , m_oldWindowRect(0, 0, 640, 480)
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
        Pick::initInstance();
        TimerMgr::initInstance();
        SceneNodeFactory::init();
    }

    void CApp::releaseSingleton()
    {
        SoundMgr::instance()->release();
        TerrainMap::finiInstance();
        Pick::finiInstance();
        TimerMgr::finiInstance();

        //////////////////////////////////////////////////////////////////////////
        clearSectionCache();
        MemoryPool::fini();
        MemoryChecker::finiInstance();
    }

    bool CApp::create(HINSTANCE hInstance, const std::wstring & caption,
                      int nWidth, int nHeight, bool bFullScr)
    {
        m_hInstance = hInstance;
        m_nWidth = nWidth + ::GetSystemMetrics(SM_CXBORDER) * 2;
        m_nHeight = nHeight + ::GetSystemMetrics(SM_CYBORDER) + ::GetSystemMetrics(SM_CYCAPTION);
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

    //初始化Direct3D对象及设备对象
    bool CApp::createDevice()
    {
        if (!rcDevice()->create(m_hWnd, m_hInstance, m_bFullScreen != FALSE))
            return false;

        m_pd3dDevice = rcDevice()->getDevice();
        m_pd3dDevice->AddRef();

        return true;
    }


    //创建窗口
    bool CApp::createWindow()
    {
        CREATESTRUCT cs;
        ZeroMemory(&cs, sizeof(cs));

        if (m_bFullScreen)
        {
            cs.style = WS_VISIBLE | WS_POPUP;
            cs.cx = m_nWidth = GetSystemMetrics(SM_CXSCREEN);
            cs.cy = m_nHeight = GetSystemMetrics(SM_CYSCREEN);
        }
        else
        {
            cs.cx = m_nWidth;
            cs.cy = m_nHeight;

            cs.x = (GetSystemMetrics(SM_CXSCREEN) - cs.cx) / 2;
            cs.y = (GetSystemMetrics(SM_CYSCREEN) - cs.cy - 30) / 2;
            cs.style = WS_OVERLAPPEDWINDOW; // WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        }
        cs.hInstance = m_hInstance;

        onCreateWindow(&cs);

        if (cs.lpszClass == NULL)
            cs.lpszClass = CLASS_NAME;

        if (cs.lpszName == NULL)
            cs.lpszName = m_caption.c_str();

        m_hWnd = CreateWindow(
                     cs.lpszClass,
                     cs.lpszName,
                     cs.style,
                     cs.x, cs.y, cs.cx, cs.cy,
                     cs.hwndParent,
                     cs.hMenu,
                     cs.hInstance,
                     cs.lpCreateParams);

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
        wc.lpfnWndProc = (WNDPROC) defaultWindowProc;
        wc.hInstance = m_hInstance;
        wc.lpszClassName = CLASS_NAME;
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.style = CS_CLASSDC;

        onRegisterClass(&wc);

        if (wc.hIcon == 0)
            wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        if (wc.hIconSm == 0)
            wc.hIconSm = wc.hIcon;
        if (wc.hCursor == 0)
            wc.hCursor = LoadCursor(NULL, IDC_ARROW);

        RegisterClassEx(&wc);
    }

    /** 退出游戏*/
    void CApp::quitGame(void)
    {
        PostQuitMessage(0);
    }

    ///处理消息。返回false，表示程序退出。
    bool CApp::processMessage()
    {
        static MSG msg = { 0 };
        while (msg.message != WM_QUIT)
        {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else break;
        }

        return msg.message != WM_QUIT;
    }

    /**游戏初始化*/
    bool CApp::init(void)
    {
        srand(timeGetTime());

        m_pRenderTaskMgr = new RenderTask();
        m_pUpdateTaskMgr = new RenderTask();

        FontMgr::instance()->init();
        FontMgr::instance()->registerFontFile(L"C:/Windows/Fonts/simhei.ttf", L"def");

        m_pKeyboard = new Keyboard();
        addTickTask(m_pKeyboard.get());

        addDrawTickTask(TerrainMap::instance());
        addDrawTickTask(EntityMgr::instance());

        return true;
    }

    void CApp::mainLoop(void)
    {
        LOG_INFO(_T("start game loop."));
        static MSG msg = { 0 };
        while (msg.message != WM_QUIT)
        {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                if (GetActiveWindow() != getHWnd())
                {
                    Sleep(100);
                }
                run();
            }
        }
        LOG_INFO(_T("game loop end."));

        clear();//释放资源
    }


    /**运行*/
    void CApp::run(void)
    {
        DWORD curTime = timeGetTime();
        static DWORD oldUpdateTime = curTime;
        m_fElapse = (curTime - oldUpdateTime) * 0.001f;
        m_fElapse *= m_timeScale;

        //防止时间为0，引发浮点数除0。
        if (m_fElapse < 0.00001f)
        {
            Sleep(1);
            return;
        }

        if (m_bWindowResized)
        {
            m_bWindowResized = false;
            rcDevice()->onWindowResize(m_nWidth, m_nHeight);
        }

        m_fps.update();

        oldUpdateTime = curTime;

        update();//更新逻辑

        if (m_bWindowMinimized)
        {
            Sleep(100);
        }
        else
        {
            rcDevice()->clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 100), 1.0f, 0);
            if (rcDevice()->beginScene())
            {
                render();//渲染画面
                rcDevice()->endScene();
                rcDevice()->present();
            }
            else
            {
                Sleep(100);
            }
        }
    }

    /**游戏更新*/
    void CApp::update(void)
    {
        TimerMgr::instance()->update(m_fElapse);
        m_pUpdateTaskMgr->update(m_fElapse);
    }

    /**渲染*/
    void CApp::render(void)
    {

#ifdef USE_SHADOW_MAP
        if (ShadowMap::instance()->begin())
        {
            Mesh::selectTechnique("shadowmap");
            rcDevice()->clear(D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER,
                D3DXCOLOR(0, 0, 0, 0), 1, 0);

            m_pRenderTaskMgr->render(m_pd3dDevice);
            ShadowMap::instance()->end();

            static bool isFirst = true;
            if (isFirst)
            {
                isFirst = false;
                D3DXSaveTextureToFile(L"test.bmp", D3DXIFF_BMP,
                    ShadowMap::instance()->getTexture(), NULL);
            }
        }
#endif
        Mesh::selectTechnique("default");

        m_pRenderTaskMgr->render(m_pd3dDevice);
    }

    /**释放资源*/
    void CApp::clear(void)
    {
        m_pRenderTaskMgr = NULL;
        m_pUpdateTaskMgr = NULL;
        m_pKeyboard = NULL;

        FontMgr::instance()->fini();
        EntityMgr::instance()->clear();
    }

    bool CApp::onEvent(const SEvent & event)
    {
        if (event.eventType == EET_KEY_EVENT)
        {
            if (!event.keyEvent.down)
            {
                switch (event.keyEvent.key)
                {
                case KEY_ESCAPE:
                    DestroyWindow(m_hWnd);
                    break;
                case KEY_RETURN:
                    if (m_pKeyboard->isKeyPress(VK_MENU))
                        changeFullScreen(!m_bFullScreen);
                    break;
                default:
                    return false;
                }
                return true;
            }
        }
        else if (event.eventType == EET_SYS_EVENT)
        {
            switch (event.sysEvent.message)
            {
            case WM_SIZE:
                if (event.sysEvent.wparam == SIZE_MINIMIZED)
                {
                    m_bWindowMinimized = true;
                }
                else
                {
                    m_bWindowMinimized = false;
                    m_bWindowResized = true;
                }
                return false;

            default:
                return false;
            }
            return true;
        }

        return false;
    }


    void CApp::setCaption(const tstring & caption)
    {
        m_caption = caption;
        SetWindowText(m_hWnd, m_caption.c_str());
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

    void CApp::addDrawTask(IRenderable* rend)
    {
        m_pRenderTaskMgr->add(rend);
    }
    void CApp::addTickTask(IRenderable* up)
    {
        m_pUpdateTaskMgr->add(up);
    }

    void CApp::addDrawTickTask(IRenderable *pObj)
    {
        addDrawTask(pObj);
        addTickTask(pObj);
    }

    void CApp::delDrawTask(IRenderable* rend)
    {
        if (m_pRenderTaskMgr)
            m_pRenderTaskMgr->remove(rend);
    }

    void CApp::delTickTask(IRenderable* up)
    {
        if (m_pUpdateTaskMgr)
            m_pUpdateTaskMgr->remove(up);
    }

    void CApp::delDrawTickTask(IRenderable* pObj)
    {
        delDrawTask(pObj);
        delTickTask(pObj);
    }


    bool CApp::changeWindowSize(int width, int height)
    {
        if (m_bFullScreen) return false;
        if (width == m_nWidth && height == m_nHeight) return false;

        m_nWidth = width;
        m_nHeight = height;

        RECT rect;
        ::GetWindowRect(m_hWnd, &rect);
        ::MoveWindow(m_hWnd, rect.left, rect.top, width, height, TRUE);

        m_bWindowResized = true;
        return true;
    }

    bool CApp::changeClientSize(int width, int height)
    {
        width += ::GetSystemMetrics(SM_CXBORDER) * 2;
        height += ::GetSystemMetrics(SM_CYBORDER) + ::GetSystemMetrics(SM_CYCAPTION);

        return changeWindowSize(width, height);
    }

    bool CApp::changeFullScreen(bool fullScreen)
    {
        if ((BOOL) fullScreen == m_bFullScreen) return false;
        m_bFullScreen = fullScreen;

        int x = 0;
        int y = 0;
        DWORD style = 0;
        if (m_bFullScreen)
        {
            ::GetWindowRect(m_hWnd, &m_oldWindowRect);
            m_oldWindowStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);

            m_nWidth = ::GetSystemMetrics(SM_CXSCREEN);
            m_nHeight = ::GetSystemMetrics(SM_CYSCREEN);

            style = WS_VISIBLE | WS_POPUP;
        }
        else
        {
            m_nWidth = m_oldWindowRect.width();
            m_nHeight = m_oldWindowRect.height();

            x = m_oldWindowRect.left;
            y = m_oldWindowRect.top;
            style = m_oldWindowStyle;
        }

        ::SetWindowLong(m_hWnd, GWL_STYLE, style);
        ::MoveWindow(m_hWnd, x, y, m_nWidth, m_nHeight, TRUE);

        LOG_DEBUG(_T("changeFullScreen fullscreen=%d size(%d, %d, %d, %d)"),
            m_bFullScreen, x, y, m_nWidth, m_nHeight);

        return rcDevice()->onChangeFullScreen(m_bFullScreen != FALSE);
    }

    ///////////////////class CApp end/////////////////////////

} // end namespace Lazy
