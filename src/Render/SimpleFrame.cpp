#include "StdAfx.h"

#include "RenderDevice.h"
#include "SimpleFrame.h"


namespace Lazy
{

    /*static*/ DWORD CUSTOMVERTEX::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
    /*static*/ DWORD CUSTOMVERTEX::SIZE = sizeof(CUSTOMVERTEX);

    SimpleFrame *g_pFrame = NULL;

    SimpleFrame * getFrame()
    {
        return g_pFrame;
    }

    SimpleFrame::SimpleFrame(void)
        : m_hWnd(0)
        , m_hInstance(0)
    {
        g_pFrame = this;

        MemoryChecker::initInstance();
        MemoryPool::init();
    }


    SimpleFrame::~SimpleFrame(void)
    {
        MemoryPool::fini();
        MemoryChecker::finiInstance();
    }

    void SimpleFrame::clear()
    {
        rcDevice()->destroy();
        clearSectionCache();
    }

    bool SimpleFrame::init(HWND hWnd, HINSTANCE hInstance)
    {
        m_hWnd = hWnd;
        m_hInstance = hInstance;

        //创建Direct3D对象, 该对象用于创建Direct3D设备对象
        if (!Lazy::rcDevice()->create(hWnd, hInstance))
            return false;

        //启用深度测试
        //     g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, true );
        //     g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESS );
        //     g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

        //设置Alpha测试
        //     g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,true);
        //     g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF,0x00000081);
        //     g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);

        Lazy::rcDevice()->setRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        Lazy::rcDevice()->setRenderState(D3DRS_ZENABLE, TRUE);

        //设置部分环境光
        Lazy::rcDevice()->setRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(100, 100, 100));


        return onInit();
    }

    void SimpleFrame::updateAndRender()
    {
        onUpdate();

        //清空后台缓冲区
        Lazy::rcDevice()->clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                                D3DCOLOR_XRGB(45, 50, 170), 1.0f, 0);

        //开始在后台缓冲区绘制图形
        if (Lazy::rcDevice()->beginScene())
        {
            onRender();
            //结束在后台缓冲区绘制图形
            Lazy::rcDevice()->endScene();
        }

        //将在后台缓冲区绘制的图形提交到前台缓冲区显示
        Lazy::rcDevice()->present();
    }

    bool SimpleFrame::onInit()
    {
        return true;
    }

    void SimpleFrame::onUpdate()
    {
        m_fps.update();

    }

    void SimpleFrame::onRender()
    {
        //顶点数据
        CUSTOMVERTEX vertices [] =
        {
            { 100.0f, 400.0f, 0.5f, 1.0f, 0xffff0000, },
            { 300.0f, 50.0f, 0.5f, 1.0f, 0xff00ff00, },
            { 500.0f, 400.0f, 0.5f, 1.0f, 0xff0000ff, },
        };

        //在后台缓冲区绘制图形
        Lazy::rcDevice()->setfvf(CUSTOMVERTEX::FVF);
        Lazy::rcDevice()->drawPrimitiveUP(D3DPT_TRIANGLELIST, 1, vertices, CUSTOMVERTEX::SIZE);
    }

    bool SimpleFrame::onEvent(const Lazy::SEvent & event)
    {
        if (event.isSysEvent())
        {
            if (event.sysEvent.message == WM_SIZE && event.sysEvent.message != SIZE_MINIMIZED)
            {
                onSize(LOWORD(event.sysEvent.lparam), HIWORD(event.sysEvent.lparam));
            }
            else if (event.sysEvent.message == WM_SIZING)
            {
                RECT * rc = (RECT*) event.sysEvent.lparam;
                onSize(rc->right - rc->left, rc->bottom - rc->top);
            }
        }

        return false;
    }

    void SimpleFrame::onSize(int w, int h)
    {
        Lazy::rcDevice()->resetDevice();
    }

    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    SimpleWindow::SimpleWindow()
        : m_hWnd(0)
    {

    }

    SimpleWindow::~SimpleWindow()
    {

    }

    void SimpleWindow::create(HWND hWnd)
    {
        m_hWnd = hWnd;
    }

    bool SimpleWindow::create(int w, int h)
    {
        assert(m_hWnd == 0);

        return false;
    }

    LRESULT CALLBACK SimpleWindow::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        SimpleFrame * pFrame = getFrame();
        if (pFrame && processWindowMsg(pFrame, hwnd, msg, wparam, lparam))
            return 0;

        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

}//end namespace Lazy