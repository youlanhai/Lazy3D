#include "stdafx.h"

#include "RenderDevice.h"
#include "Device.h"
#include "Effect.h"
#include "ResMgr.h"
#include "ResFactory.h"
#include "Texture.h"
#include "SkinMesh.h"
#include "StaticMesh.h"

namespace Lazy
{


    DWORD VertexXYZColorUV::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
    int VertexXYZColorUV::SIZE = sizeof(VertexXYZColorUV);


    DWORD VertexXYZColor::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
    int VertexXYZColor::SIZE = sizeof(VertexXYZColor);

    ///////////////////////////////////////////////////////////////////
    static RenderDevice * s_oneInstance = NULL;

    RenderDevice *rcDevice()
    {
        static RenderDevice s_rc;
        return &s_rc;
    }

    RenderDevice::RenderDevice()
        : m_device(NULL)
        , m_hWnd(NULL)
        , m_hInstance(NULL)
        , m_isLost(false)
        , m_windowWidth(640)
        , m_windowHeight(480)
        , m_fullScreen(false)
        , m_oldStyle(0)
    {
        assert(s_oneInstance == NULL && "initialize more tha once!");

        s_oneInstance = this;
        ZeroMemory(&m_d3dmm, sizeof(m_d3dmm));
        ZeroMemory(&m_d3dcaps, sizeof(m_d3dcaps));
    }


    RenderDevice::~RenderDevice()
    {
        destroy();
    }

    void RenderDevice::fillPresentParameter()
    {
        ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));

        //设置D3DPRESENT_PARAMETERS结构, 准备创建Direct3D设备对象
        m_d3dpp.Windowed = !m_fullScreen;
        m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        m_d3dpp.EnableAutoDepthStencil = TRUE;
        m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

        m_d3dpp.BackBufferFormat = m_d3dmm.Format;
        m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

        if (m_fullScreen)
        {
            m_d3dpp.BackBufferWidth = m_d3dmm.Width;
            m_d3dpp.BackBufferHeight = m_d3dmm.Height;
            m_d3dpp.FullScreen_RefreshRateInHz = m_d3dmm.RefreshRate;
        }
    }

    ///初始化
    bool RenderDevice::create(HWND hWnd, HINSTANCE hInstance, bool fullSrc)
    {
        assert(hWnd && "RenderDevice::create");
        
        if (m_device != nullptr)
            throw(std::runtime_error("Dx Device has been created!"));

        m_hWnd = hWnd;
        m_hInstance = hInstance;
        m_fullScreen = fullSrc;

        if (m_fullScreen)
        {
            m_windowWidth = ::GetSystemMetrics(SM_CXSCREEN);
            m_windowHeight = ::GetSystemMetrics(SM_CYSCREEN);
        }
        else
        {
            CRect rect;
            ::GetWindowRect(hWnd, &rect);

            m_windowWidth = rect.width();
            m_windowHeight = rect.height();
        }

        //创建Direct3D对象, 该对象用于创建Direct3D设备对象
        ComPtr<dx::D3D> pd3d = Direct3DCreate9(D3D_SDK_VERSION);
        if (!pd3d)
        {
            LOG_ERROR(L"Create d3d failed!");
            return false;
        }
        pd3d.delRef();

        if (FAILED(pd3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_d3dcaps)))
        {
            return false;
        }

        //获得显卡当前显示模式
        if (FAILED(pd3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &m_d3dmm)))
        {
            return false;
        }

        fillPresentParameter();

        DWORD vp = D3DCREATE_MIXED_VERTEXPROCESSING;
        if (m_d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
        {
            vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
        }

        vp |= D3DCREATE_MULTITHREADED;

        //创建Direct3D设备对象
        if (FAILED(pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
            vp, &m_d3dpp, &m_device)))
        {
            return false;
        }

        m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        m_device->SetRenderState(D3DRS_ZENABLE, TRUE);
        m_device->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(100, 100, 100));

        ResourceFactoryPtr factory = new ResFactory();
        TextureMgr::instance()->setResFacotry(factory);
        StaticMeshMgr::instance()->setResFacotry(factory);
        SkinMeshMgr::instance()->setResFacotry(factory);
        EffectMgr::instance()->setResFacotry(factory);

        return true;
    }

    void RenderDevice::destroy()
    {
        if (m_device) return;

        m_device->Release();
        m_device = NULL;
        m_hWnd = NULL;
        m_hInstance = NULL;
    }

    ///重置设备
    bool RenderDevice::resetDevice()
    {
        debugMessage(_T("INFO: reset device."));
        IDevice::lostDevice();

        fillPresentParameter();

        HRESULT hr = m_device->Reset(&m_d3dpp);
        if (FAILED(hr))
        {
            debugMessage(_T("ERROR: Reset Device failed! 0x%x"), hr);
            return false;
        }

        IDevice::resetDevice();
        return true;
    }

    ///改变窗口尺寸
    bool RenderDevice::changeSize(int width, int height)
    {
        if (m_fullScreen) return false;

        if (m_windowWidth == width && m_windowHeight == height) return false;

        m_windowWidth = width;
        m_windowHeight = height;

        RECT rect;
        ::GetWindowRect(m_hWnd, &rect);
        ::MoveWindow(m_hWnd, rect.left, rect.top, m_windowWidth, m_windowHeight, TRUE);

        return resetDevice();
    }

    bool RenderDevice::changeFullScreen(bool fullSrc)
    {
        if (fullSrc == m_fullScreen) return false;
        m_fullScreen = fullSrc;

        int x = 0;
        int y = 0;
        DWORD style = 0;
        if (fullSrc)
        {
            ::GetWindowRect(m_hWnd, &m_oldRect);
            m_oldStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);

            m_windowWidth = ::GetSystemMetrics(SM_CXSCREEN);
            m_windowHeight = ::GetSystemMetrics(SM_CYSCREEN);

            style = WS_VISIBLE | WS_POPUP;
        }
        else
        {
            m_windowWidth = m_oldRect.right - m_oldRect.left;
            m_windowHeight = m_oldRect.bottom - m_oldRect.top;

            x = m_oldRect.left;
            y = m_oldRect.top;
            style = m_oldStyle;
        }

        ::SetWindowLong(m_hWnd, GWL_STYLE, style);
        ::MoveWindow(m_hWnd, x, y, m_windowWidth, m_windowHeight, TRUE);

        debugMessage(_T("changeFullScreen full=%d (%d, %d, %d, %d)"), 
            fullSrc, x, y, m_windowWidth, m_windowHeight);

        return resetDevice();
    }

    void RenderDevice::clear(DWORD flag, D3DCOLOR cr, float z, DWORD stencil)
    {
        m_device->Clear(0, NULL, flag, cr, z, stencil);
    }

    bool RenderDevice::beginScene()
    { 
        while(m_isLost)
        {
            HRESULT hr = testCooperativeLevel();
            if (hr == D3DERR_DEVICENOTRESET)
            {
                if (resetDevice())
                {
                    m_isLost = false;
                    break;
                }
            }

            Sleep(1000);
            return false;
        }

        return SUCCEEDED(m_device->BeginScene()); 
    }

    void RenderDevice::present()
    { 
        HRESULT hr = m_device->Present(NULL, NULL, NULL, NULL);
        if (SUCCEEDED(hr))
        {
            m_isLost = false;
        }
        else if (hr == D3DERR_DEVICELOST)
        {
            debugMessage(_T("ERROR: Present Failed, Device Lost."));
            m_isLost = true;
        }
    }


    //////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////
    void RenderDevice::setRenderState(dx::RSType type, DWORD value)
    {
        m_device->SetRenderState(type, value); 
    }

    void RenderDevice::getRenderState(dx::RSType type, DWORD & value)
    {
        m_device->GetRenderState(type, &value); 
    }

    DWORD RenderDevice::getRenderState(dx::RSType type)
    {
        DWORD value;
        m_device->GetRenderState(type, &value);
        return value; 
    }
    
    void RenderDevice::getWorldViewProj(D3DXMATRIX & mat)
    {
        m_device->GetTransform(D3DTS_WORLD, &mat);

        D3DXMATRIX temp;
        m_device->GetTransform(D3DTS_VIEW, &temp);
        mat *= temp;

        m_device->GetTransform(D3DTS_PROJECTION, &temp);
        mat *= temp;
    }

   
    void RenderDevice::setTransform(dx::TSType type, D3DXMATRIX & matrix)
    {
        m_device->SetTransform(type, &matrix);
    }

    HRESULT RenderDevice::testCooperativeLevel()
    { 
        return m_device->TestCooperativeLevel(); 
    }
    
    HRESULT RenderDevice::setfvf(DWORD fvf)
    { 
        return m_device->SetFVF(fvf); 
    }

    HRESULT RenderDevice::setTexture(DWORD stage, dx::BaseTexture *pTexture)
    {
        return m_device->SetTexture(stage, pTexture);
    }

    HRESULT RenderDevice::drawPrimitiveUP(dx::PTType type, size_t num, const void *stream, size_t stride)
    {
        return m_device->DrawPrimitiveUP(type, num, stream, stride);
    }
}