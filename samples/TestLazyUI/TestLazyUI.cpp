﻿// Test.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "TestLazyUI.h"

#include "utility/Utility.h"
#include "Math/Math.h"

#include "Render/Config.h"
#include "Render/RenderDevice.h"
#include "Render/Texture.h"
#include "Render/Effect.h"

#include "Font/Font.h"

#include "Lzui/Lzui.h"

#define MAX_LOADSTRING 100

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

//-----------------------------------------------------------------------------
// Desc: 全局变量
//-----------------------------------------------------------------------------
Lazy::RefPtr<Lazy::CGUIManager>     g_guiMgr;
Lazy::Fps           g_fps;

#define USE_EDITOR 0

const Lazy::tstring LayoutFile = L"layout/layout.lzd";
const Lazy::tstring LayoutFile2 = L"layout/layout2.lzd";

namespace Lazy
{
    class CTestForm;


    class CTestForm : public CForm
    {
        CButton       *m_btnOk;
        CSlidebar     *m_scroll;
        CEdit         *m_edit;

    public:
        CTestForm()
        {
        }

        void create(int id, int x, int y)
        {
            CForm::create(id, L"", x, y);
            setSize(300, 200);

            for(size_t i=0; i<100; ++i)
            {
                m_textVeryLong.append(L"这是中文okabcdKJKLJFDFJipjgdsgkkfj个gj发的萨芬反倒是发电量开了房uogfdg番窠倒\n臼");
            }
            m_label.show(false);
            m_label.setPosition(0, 0);
            m_label.setSize(Lazy::rcDevice()->windowWidth(), Lazy::rcDevice()->windowHeight());
            m_label.enableMutiLine(true);
            m_label.setMaxWidth(Lazy::rcDevice()->windowWidth());
            m_label.setColor(0xff00ff00);
            m_label.setText(m_textVeryLong);
            g_guiMgr->addChild(&m_label);

            m_fpsLabel.setColor(0xffff0000);
            g_guiMgr->addChild(&m_fpsLabel);

    #if USE_EDITOR

            if (!loadFromFile(LayoutFile))
            {
                LOG_ERROR(L"load layout failed!");
                return;
            }

            if (!saveToFile(LayoutFile2))
            {
                LOG_ERROR(L"Save layout 2 failed!");
            }

    #else

            setImage(L"gui/login.png");
            setBgColor(0xffffffff);

            CButton *button = (CButton*)Lazy::uiFactory()->create(Lazy::uitype::Button);
            button->create(1, _T("OK"), _T(""), 0, 0, 100, 40);
            button->setEditable(true);
            button->setManaged(true);
            addChild(button);


            CSlidebar* scroll = (CSlidebar*) Lazy::uiFactory()->create(Lazy::uitype::Slidebar);
            scroll->create(2, _T(""), 0, 50);
            scroll->setSize(100, 20);
            scroll->setEditable(true);
            scroll->setManaged(true);
            addChild(scroll);

            CEdit *edit = (CEdit*) Lazy::uiFactory()->create(Lazy::uitype::Edit);
            edit->setID(3);
            edit->setPosition(120, 20);
            edit->setSize(100, 50);
            edit->setEditable(true);
            edit->setManaged(true);
            addChild(edit);

            m_name = L"MainView";
            if (!saveToFile(LayoutFile))
            {
                LOG_ERROR(L"Save layout '%s' failed!", LayoutFile.c_str());
            }
    #endif

            m_btnOk = (CButton*)getChild(1);
            m_scroll = (CSlidebar*) getChild(2);
            m_edit = (CEdit*)getChild(3);

            m_edit->setText(L"哈哈abc");
            m_edit->setColor(0xff00ff00);
            m_edit->setMutiLine(true);
        }

        void update(float elapse)
        {
            CForm::update(elapse);

            tstring str;
            formatString(str, L"fps:%.2f", g_fps.getFps());
            m_fpsLabel.setText(str);
        }

        void toggleLabel()
        {
            m_label.toggle();
        }

    private:

        tstring m_textVeryLong;
        CLabel  m_label;
        CLabel  m_fpsLabel;
    };

}

//-----------------------------------------------------------------------------
// Desc: 顶点结构
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
    FLOAT x, y, z, rhw;
    DWORD color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)  //顶点格式


//-----------------------------------------------------------------------------
// Desc: 初始化Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd, HINSTANCE hInstance )
{
    //渲染设备
    if (!Lazy::rcDevice()->create(hWnd, 0)) return S_FALSE;

    Lazy::rcDevice()->setRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(100, 100, 100));
    Lazy::rcDevice()->setRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    Lazy::rcDevice()->setRenderState(D3DRS_ZENABLE, TRUE);
    //Lazy::rcDevice()->setUIShader(L"shader/ui.fx");
    //Lazy::rcDevice()->setFontShader(L"shader/font.fx");


    g_guiMgr = new Lazy::CGUIManager(Lazy::rcDevice()->getDevice(), hWnd, hInstance);

    //添加一个标签
    Lazy::CLabel *pLabel = (Lazy::CLabel*)Lazy::uiFactory()->create(Lazy::uitype::Label);
    pLabel->create(1, _T("haha你好啊ABCDEFGHIGKLMN\r\n哈哈哈\n"), 20, 100);
    pLabel->setAlign(Lazy::RelativeAlign::right | Lazy::RelativeAlign::vcenter);
    pLabel->enableMutiLine(true);
    pLabel->setSize(100, 40);
    pLabel->setColor(0xffff0000);
    g_guiMgr->addChildManage(pLabel);

    pLabel = (Lazy::CLabel*)Lazy::uiFactory()->create(Lazy::uitype::Label);
    pLabel->create(1, _T("测试中文标签abcd\n哈哈哈\n"), 20, 20);
    pLabel->setAlign(Lazy::RelativeAlign::center);
    pLabel->setSize(500, 40);
    pLabel->setColor(0xffff0000);
    g_guiMgr->addChildManage(pLabel);

    //添加测试窗体
    Lazy::CTestForm *pForm = new Lazy::CTestForm();
    pForm->create(2, 100, 100);
    g_guiMgr->addChildManage(pForm);

    return S_OK;
}



//-----------------------------------------------------------------------------
// Desc: 释放创建的对象
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    g_guiMgr->destroy();
    g_guiMgr = NULL;

    Lazy::clearSectionCache();
    Lazy::FontMgr::instance()->fini();
    Lazy::rcDevice()->destroy();

    Lazy::MemoryPool::fini();
    Lazy::MemoryChecker::finiInstance();
}


//-----------------------------------------------------------------------------
// Desc: 渲染图形 
//-----------------------------------------------------------------------------
VOID Render()
{
    g_fps.update();

    g_guiMgr->update(g_fps.getElapse());

    //清空后台缓冲区
    Lazy::rcDevice()->clear(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
        D3DCOLOR_XRGB(45, 50, 170), 1.0f, 0 );

    //开始在后台缓冲区绘制图形
    if(Lazy::rcDevice()->beginScene())
    {
        //在后台缓冲区绘制图形
        
        g_guiMgr->render(Lazy::rcDevice()->getDevice());

        //结束在后台缓冲区绘制图形
        Lazy::rcDevice()->endScene();
    }

    //将在后台缓冲区绘制的图形提交到前台缓冲区显示
    Lazy::rcDevice()->present();
}

void onSizing(int w, int h)
{
    debugMessage(_T("onSize w=%d h=%d"), w, h);
    Lazy::rcDevice()->changeSize(w, h);
}


// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
    Lazy::changeCurDirectory();
    Lazy::defaultCodePage = Lazy::CP::utf8;
    Lazy::getfs()->addResPath(L"res/");
    INIT_LOG(L"testui.log");

    Lazy::MemoryPool::init();

    Lazy::FontMgr::instance()->init();
    Lazy::FontMgr::instance()->registerFontFile(_T("C:/Windows/Fonts/simhei.ttf"), _T("def"));

	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);


	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEST));

    ZeroMemory( &msg, sizeof(msg) );

    //进入消息循环
    while( msg.message!=WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            Render();  //渲染图形
        }
    }

    Cleanup();
	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEST));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = NULL;// MAKEINTRESOURCE(IDC_TEST);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   //初始化Direct3D
   if (FAILED(InitD3D(hWnd, hInstance)))
   {
       return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

class EventHander : public Lazy::IEventReceiver
{
public:

    virtual bool onEvent(const Lazy::SEvent & event) override
    {
        if (g_guiMgr && g_guiMgr->processEvent(event)) return true;

        return false;
    }

    bool processWinMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        return Lazy::processWindowMsg(this, hWnd, message, wParam, lParam);
    }

};

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static EventHander hander;

    if (hander.processWinMsg(hWnd, message, wParam, lParam))
    {
        return 0;
    }

	switch (message)
	{
    case WM_KEYUP:
        switch(wParam )
        {
        case VK_ESCAPE:
            Lazy::rcDevice()->changeFullScreen(false);
            break;
        case VK_RETURN:
            Lazy::rcDevice()->changeFullScreen(true);
            break;
        }
	case WM_PAINT:
    case WM_ERASEBKGND:
        Render();
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

    case WM_SIZE:

        if (wParam != SIZE_MINIMIZED)
        {
            int w = LOWORD(lParam);
            int h = HIWORD(lParam);
            debugMessage(_T("WM_SIZE, %x %d %d"), wParam, w, h);
            Lazy::rcDevice()->resetDevice();
        }
        break;

    case WM_SIZING:
        {
            LPRECT prc = (LPRECT) lParam;
            onSizing(prc->right - prc->left, prc->bottom - prc->top);
        }
        break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

