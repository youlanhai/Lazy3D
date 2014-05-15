//=============================================================================
// Desc: 文件网格模型的使用
//=============================================================================
#include <d3dx9.h>
#include "Physics/Physics.h"
#include "Physics/LazyConllision.h"
#include "CursorCamera.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")

//#define TEST_MESH L"res/arena.x"
#define TEST_MESH L"res/csm.x"

Physics::OCTree g_octree;

namespace PlayerInfo
{
    float width = 20;
    float height = 40;
    float depth = 20;
    Physics::Vector3 oldPos(-200, 0, -200);
    Physics::Vector3 newPos(0, 0, 0);
}

//-----------------------------------------------------------------------------
// Desc: 全局变量
//-----------------------------------------------------------------------------
HWND                    g_hWnd           = NULL;
LPDIRECT3D9             g_pD3D           = NULL;  //Direct3D对象
LPDIRECT3DDEVICE9       g_pd3dDevice     = NULL;  //Direct3D设备对象

LPD3DXMESH              g_pMesh          = NULL;  //网格模型对象
LPD3DXFONT              g_pFont          = NULL;
Lazy::RefPtr<CCursorCamera>   g_pCamera;

float g_timeCreate = 0.0f;
float g_timePick = 0.0f;
float g_timeCollid = 0.0f;

float getCurTime()
{
    return timeGetTime() * 0.001f;
}

//-----------------------------------------------------------------------------
// Desc: 设置观察矩阵和投影矩阵
//-----------------------------------------------------------------------------
VOID SetViewAndProjMatrix()
{
    Math::Matrix4x4 matView;

    //创建并设置观察矩阵
    D3DXVECTOR3 vEyePt( 0.0f, 500.0f, -800.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );

    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    //创建并设置投影矩阵
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 10000.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}


//-----------------------------------------------------------------------------
// Desc: 初始化Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
	//创建Direct3D对象, 该对象用于创建Direct3D设备对象
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

	//设置D3DPRESENT_PARAMETERS结构, 准备创建Direct3D设备对象
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    //创建Direct3D设备对象
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    if (FAILED(D3DXCreateFont(g_pd3dDevice, 16, 0, 2, 0, FALSE, 
        DEFAULT_CHARSET, 0, 0, 0, L"宋体", &g_pFont)))
    {
        return E_FAIL;
    }

	//设置环境光
    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );
    g_pCamera = new CCursorCamera(g_hWnd, g_pd3dDevice);
    g_pCamera->setSpeed(1000.0f);
    g_pCamera->setPosition(Math::Vector3(0, 200, -800));
    g_pCamera->rotPitch(-D3DX_PI / 4.0f);

    g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//设置观察矩阵和投影矩阵
	SetViewAndProjMatrix();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Desc: 创建场景图形
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
    //从磁盘文件加载网格模型
    if( FAILED( D3DXLoadMeshFromX( TEST_MESH, D3DXMESH_MANAGED, 
                                   g_pd3dDevice, NULL, 
                                   NULL, NULL, NULL, 
                                   &g_pMesh ) ) )
    {
        MessageBox(NULL, L"Could not find mesh.", L"Mesh", MB_OK);
        return E_FAIL;
    }

    g_timeCreate = getCurTime();
    g_octree.build(g_pMesh);
    g_timeCreate = getCurTime() - g_timeCreate;

    return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 释放创建的对象
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    if(g_pFont)
        g_pFont->Release();

	//释放网格模型对象
    if( g_pMesh != NULL )
        g_pMesh->Release();
    
	//释放Direct3D设备对象
    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

	//释放Direct3D对象
    if( g_pD3D != NULL )
        g_pD3D->Release();
}


//-----------------------------------------------------------------------------
// Desc: 渲染场景
//-----------------------------------------------------------------------------
VOID Render()
{
    //elapse
    float curTime = getCurTime();
    static float lastTime = curTime;
    float elapse = curTime - lastTime;
    lastTime = curTime;

    if (elapse < 0.00001f) return;

    //fps
    static float lastFpsTime = curTime;
    static int nbFrame = 0;
    static float fps = 0.0f;
    ++nbFrame;
    if (curTime - lastFpsTime > 1.0f)
    {
        fps = nbFrame/(curTime - lastFpsTime);
        nbFrame = 0;
        lastFpsTime = curTime;
    }


    g_pCamera->update(elapse);

    // 清除缓冲区
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
                         D3DCOLOR_XRGB(0,0,125), 1.0f, 0 );
    
    //开始渲染场景
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
        g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
        g_pCamera->render(g_pd3dDevice);

        Math::Matrix4x4 matWord;
        matWord.makeIdentity();
        g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWord);

        //x轴
        Physics::drawLine(g_pd3dDevice, D3DXVECTOR3(-1000, 0, 0), D3DXVECTOR3(1000, 0, 0), 0xffffff00);
        //z轴
        Physics::drawLine(g_pd3dDevice, D3DXVECTOR3(0, 0, -1000), D3DXVECTOR3(0, 0, 1000), 0xffff00ff);

        Math::Matrix4x4 matRot;
        matRot.makeRatateY(timeGetTime() / 1500.0f);

        if (g_octree.valid())
        {
            g_octree.render(g_pd3dDevice);

            //射线拾取
            Physics::Vector3 start(-1000, 500, 0);
            Physics::Vector3 end(0, 0, 0);

            start.applyMatrix(matRot);
            end.applyMatrix(matRot);

            Physics::Vector3 dir = end - start;
            dir.normalize();

            Physics::RayInfo rayInfo;
            rayInfo.start = start;
            rayInfo.dir = dir;
            rayInfo.hitDistance = 10000.0f;
            rayInfo.pMesh = g_pMesh;

            Physics::drawLine(g_pd3dDevice, rayInfo.start, 
                rayInfo.start + rayInfo.dir*rayInfo.hitDistance, 0xff00ff00);

            //开始拾取
            g_timePick = getCurTime();
            bool collisioned = g_octree.pick(rayInfo);
            g_timePick = getCurTime() - g_timePick;

            //绘制拾取图形
            if (collisioned)
            {
                Physics::drawTriangle(g_pd3dDevice, rayInfo.hitTriangle, 0xffff0000);
            }

            //扫掠体碰撞检测
            Physics::OCTreeCollider collider;

            collider.start = PlayerInfo::oldPos;
            collider.end = PlayerInfo::newPos;
            collider.extends.set(PlayerInfo::width, PlayerInfo::height, PlayerInfo::depth);

            collider.start.applyMatrix(matRot);
            collider.end.applyMatrix(matRot);
            collider.testOld = collider.start;
            collider.calAABB();

            collider.render(g_pd3dDevice);
            Physics::drawAABB(g_pd3dDevice, collider.aabb, 0xff000000);

            float collidDist = 0;

            //执行碰撞检测
            g_timeCollid = getCurTime();
            collisioned = g_octree.collied(collider, collidDist);
            g_timeCollid = getCurTime() - g_timeCollid;

            if(collisioned)
            {
                collider.start = collider.testOld;
                Physics::Vector3 dir = collider.end - collider.start;
                dir.normalize();
                collider.end = collider.start + dir * collidDist;

                collider.render(g_pd3dDevice, 0xffff0000);
            }
        }//end g_octree.valid()

        const int MAX_SIZE = 1024;
        const int DH = 20;
        TCHAR szBuffer[MAX_SIZE];
        RECT rc;
        GetClientRect(g_hWnd, &rc);

        swprintf_s(szBuffer, MAX_SIZE, L"fps %0.2f", fps);
        g_pFont->DrawText(NULL, szBuffer, -1, &rc, DT_LEFT, 0xffff0000);
        rc.top += DH;

        swprintf_s(szBuffer, MAX_SIZE, L"build time :%fs", g_timeCreate);
        g_pFont->DrawText(NULL, szBuffer, -1, &rc, DT_LEFT, 0xff00ff00);
        rc.top += DH;

        swprintf_s(szBuffer, MAX_SIZE, L"pick time :%fs", g_timePick);
        g_pFont->DrawText(NULL, szBuffer, -1, &rc, DT_LEFT, 0xff00ff00);
        rc.top += DH;

        swprintf_s(szBuffer, MAX_SIZE, L"collied time :%fs", g_timeCollid);
        g_pFont->DrawText(NULL, szBuffer, -1, &rc, DT_LEFT, 0xff00ff00);
        rc.top += DH;

        rc.top = rc.bottom - 40;
        swprintf_s(szBuffer, MAX_SIZE, 
            L"octree 1-show node, 2-show leaf aabb, 3-show leaf triangle.",
            g_timeCollid);
        g_pFont->DrawText(NULL, szBuffer, -1, &rc, DT_LEFT, 0xffff0000);

        //场景渲染结束
        g_pd3dDevice->EndScene();
    }

    //在屏幕上显示场景
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


//-----------------------------------------------------------------------------
// Desc: 窗口过程, 处理消息
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    if (g_pCamera && g_pCamera->handleMessage(msg, wParam, lParam))
    {
        return 0;
    }

    switch( msg )
    {
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;

        case  WM_KEYUP:
            {
                switch(wParam)
                {
                case '1' :
                    Physics::DebugFlag::renderNode = !Physics::DebugFlag::renderNode;
                    break;
                case '2' :
                    Physics::DebugFlag::renderLeafBox = !Physics::DebugFlag::renderLeafBox;
                    break;
                case '3' :
                    Physics::DebugFlag::renderLeafTri = !Physics::DebugFlag::renderLeafTri;
                    break;

                case VK_ESCAPE:
                    DestroyWindow(g_hWnd);
                    break;
                }
            }
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}


//-----------------------------------------------------------------------------
// Desc: 入口函数
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    //注册窗口类
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpfnWndProc = MsgProc;
    wc.style = CS_CLASSDC;
    wc.hInstance = hInst;
    wc.lpszClassName = L"ClassName";
    RegisterClassEx( &wc );

    //创建窗口
    g_hWnd = CreateWindow( L"ClassName", L"碰撞检测", 
                              WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE, 
                              200, 100, 800, 600,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

    timeBeginPeriod(1);
	//初始化Direct3D
    if( SUCCEEDED( InitD3D( g_hWnd ) ) )
    { 
		//创建场景图形
        if( SUCCEEDED( InitGeometry() ) )
        {
            //显示窗口
            ShowWindow( g_hWnd, SW_SHOWDEFAULT );
            UpdateWindow( g_hWnd );

            //进入消息循环
            MSG msg; 
            ZeroMemory( &msg, sizeof(msg) );
            while( msg.message!=WM_QUIT )
            {
                if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
                {
                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                }
                else
				{
                    Render();  //渲染场景
				}
            }
        }
    }
    timeEndPeriod(1);

    Cleanup();
    UnregisterClass( L"ClassName", wc.hInstance );
    return 0;
}



