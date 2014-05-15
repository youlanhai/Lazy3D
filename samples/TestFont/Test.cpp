// test.cpp : Defines the entry point for the application.
//

#include "utility/Utility.h"
#include "Math/Math.h"

#include "Render/Config.h"
#include "Render/RenderDevice.h"
#include "Font/Font.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[] = _T("Test Font");					// The title bar text
TCHAR szWindowClass[] = _T("LazyFont");			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")


//-----------------------------------------------------------------------------
// Desc: 全局变量
//-----------------------------------------------------------------------------
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL;    //顶点缓冲区对象
IDirect3DTexture9       *g_pTexture = NULL;
int g_texSize = 256;
HWND g_hWnd = NULL;


//-----------------------------------------------------------------------------
// Desc: 顶点结构
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
    FLOAT x, y, z, rhw;
    DWORD color;
    float u, v;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)  //顶点格式


//-----------------------------------------------------------------------------
// Desc: 初始化Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
    //创建dx设备
    if (!Lazy::rcDevice()->create(hWnd, 0)) return E_FAIL;
    
    dx::Device * pDevice = Lazy::rcDevice()->getDevice();
    pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(100, 100, 100));

    if (FAILED(D3DXCreateTexture(pDevice,
        g_texSize, g_texSize, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &g_pTexture)))
    {
        MessageBox(NULL, _T("create texture faild!"), NULL, 0);
        return E_FAIL;
    }

    Lazy::rcDevice()->setFontShader(L"font/font.fx");

    Lazy::FontMgr::instance()->init();
    Lazy::FontMgr::instance()->registerFontFile(L"font/STSONG.TTF", L"def");

    //将文字填充到贴图
    Lazy::FontPtr font = Lazy::FontMgr::instance()->getFont(L"def|24|0");
    if (font)
    {
        font->fillTextToTexture(g_pTexture, 0, 0, L"哈哈，发的说法", 0xff000000);
    }


    Math::Matrix4x4 view, proj;
    view.makeIdentity();
    pDevice->SetTransform(D3DTS_VIEW, &view);

    RECT rc;
    ::GetClientRect(g_hWnd, &rc);
    D3DXMatrixOrthoOffCenterLH(&proj, rc.left, rc.right, rc.bottom, rc.top, 1.0f, 10.0f);
    pDevice->SetTransform(D3DTS_PROJECTION, &proj);

    return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 创建并填充顶点缓冲区
//-----------------------------------------------------------------------------
HRESULT InitVB()
{
    //顶点数据
    CUSTOMVERTEX vertices[] =
    {
        { 100.0f, 100.0f, 0.5f, 1.0f, 0xffffffff, 0, 0},
        { 300.0f, 100.0f, 0.5f, 1.0f, 0xffffffff, 1, 0}, 
        { 100.0f, 300.0f, 0.5f, 1.0f, 0xffffffff, 0, 1},
        { 300.0f, 300.0f, 0.5f, 1.0f, 0xffffffff, 1, 1},
    };

    LPDIRECT3DDEVICE9 pDevice = Lazy::rcDevice()->getDevice();
    //创建顶点缓冲区
    if (FAILED(pDevice->CreateVertexBuffer(sizeof(vertices),
        0, D3DFVF_CUSTOMVERTEX,
        D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
    {
        return E_FAIL;
    }

    //填充顶点缓冲区
    VOID* pVertices;
    if( FAILED( g_pVB->Lock( 0, sizeof(vertices), (void**)&pVertices, 0 ) ) )
        return E_FAIL;
    memcpy( pVertices, vertices, sizeof(vertices) );
    g_pVB->Unlock();


    return S_OK;
}

//-----------------------------------------------------------------------------
// Desc: 释放创建的对象
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    if(g_pTexture)
    {
        D3DXSaveTextureToFile(_T("font/dest.png"), D3DXIFF_PNG, g_pTexture, NULL);
        g_pTexture->Release();
    }

    Lazy::FontMgr::instance()->fini();
    
    //释放顶点缓冲区对象
    if( g_pVB != NULL )        
        g_pVB->Release();

    Lazy::rcDevice()->destroy();
}


//-----------------------------------------------------------------------------
// Desc: 渲染图形 
//-----------------------------------------------------------------------------
VOID Render()
{
    float curTime = GetTickCount()*0.001f;
    static float lastTime = curTime;
    float elapse = curTime - lastTime;
    lastTime = curTime;

    //清空后台缓冲区
    Lazy::rcDevice()->clear(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
        D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0 );

    //开始在后台缓冲区绘制图形
    if (Lazy::rcDevice()->beginScene())
    {
#if 1 
        //渲染填充到贴图的文字

        dx::Device * pDevice = Lazy::rcDevice()->getDevice();

        pDevice->SetRenderState(D3DRS_ZENABLE,FALSE);
        pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        pDevice->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );
        pDevice->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA);
        pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
        pDevice->SetRenderState(D3DRS_ALPHAREF, 0X20);
        pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
        pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

        pDevice->SetTexture(0, g_pTexture);

        //在后台缓冲区绘制图形
        pDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
        pDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
        pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

        pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
        pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
#endif

#if 1
        //使用shader直接渲染文字
        Lazy::FontPtr font = Lazy::FontMgr::instance()->getFont(L"def|24|0");
        if (font)
        {
            Lazy::CRect rc(400, 100, 100, 100);
            font->drawText(L"使用Free Type绘制文字。", rc, 0, 0xff00afff);
        }

#endif
        //结束在后台缓冲区绘制图形
        Lazy::rcDevice()->endScene();
    }

    //将在后台缓冲区绘制的图形提交到前台缓冲区显示
    Lazy::rcDevice()->present();
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
    MSG msg;
    ZeroMemory( &msg, sizeof(msg) );

	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

    //进入消息循环
    while( msg.message!=WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
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
	wcex.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= wcex.hIcon;

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   g_hWnd = hWnd;

    //初始化Direct3D
   if( FAILED( InitD3D( hWnd ) ) )
   {
       return FALSE;
   }
   //创建并填充顶点缓冲区
   if( FAILED( InitVB() ) )
   {
       return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
