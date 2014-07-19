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

    namespace
    {
        const uint32 DirtyWorldViewProj = 1 << 0;
        const uint32 DirtyInView = 1 << 1;
        const uint32 DirtyViewProj = 1 << 2;

        const uint32 DirtyAll = 0xff;
    }

    RSHolder::RSHolder(dx::Device *pDevice, dx::RSType rsType, DWORD rsValue)
        : m_pDevice(pDevice)
        , m_rsType(rsType)
    {
        m_pDevice->GetRenderState(m_rsType, &m_rsOriginValue);
        
        if (m_rsOriginValue != rsValue)
            m_pDevice->SetRenderState(m_rsType, rsValue);
        else
            m_pDevice = nullptr;
    }

    RSHolder::~RSHolder()
    {
        if (m_pDevice)
            m_pDevice->SetRenderState(m_rsType, m_rsOriginValue);
    }

    ///////////////////////////////////////////////////////////////////
    ///
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
        , m_matDirty(0)
    {
        assert(s_oneInstance == NULL && "initialize more tha once!");

        s_oneInstance = this;
        ZeroMemory(&m_d3dmm, sizeof(m_d3dmm));
        ZeroMemory(&m_d3dcaps, sizeof(m_d3dcaps));

        pushWorld(matIdentity);
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
        m_device->SetRenderState(D3DRS_LIGHTING, TRUE);
        m_device->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(100, 100, 100));

        m_device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        m_device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
        m_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
        m_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        m_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

        m_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
        m_device->SetRenderState(D3DRS_ALPHAREF, 10);
        m_device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

        m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

        m_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        m_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        m_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

        m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        m_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

        ResourceFactoryPtr factory = new ResFactory();
        TextureMgr::instance()->setResFacotry(factory);
        StaticMeshMgr::instance()->setResFacotry(factory);
        SkinMeshMgr::instance()->setResFacotry(factory);
        EffectMgr::instance()->setResFacotry(factory);

        return true;
    }

    void RenderDevice::destroy()
    {
        if (!m_device)
            return;

        IDevice::closeAllDevice();

        m_device->Release();
        m_device = NULL;
        m_hWnd = NULL;
        m_hInstance = NULL;
    }

    ///重置设备
    bool RenderDevice::resetDevice()
    {
        debugMessage(_T("INFO: reset device."));
        IDevice::lostAllDevice();

        fillPresentParameter();

        HRESULT hr = m_device->Reset(&m_d3dpp);
        if (FAILED(hr))
        {
            debugMessage(_T("ERROR: Reset Device failed! 0x%x"), hr);
            return false;
        }

        IDevice::resetAllDevice();
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

    void RenderDevice::pushRS(dx::RSType type, DWORD value)
    {
        assert(m_rsStack.size() < 0xffff);

        DWORD oldV;
        m_device->GetRenderState(type, &oldV);
        m_device->SetRenderState(type, value);

        m_rsStack.push_back(std::pair<DWORD, DWORD>(type, oldV));
    }

    void RenderDevice::popRS(dx::RSType type)
    {
        for (int i = int(m_rsStack.size()) - 1; i > 0; --i)
        {
            if (m_rsStack[i].first == type)
            {
                m_device->SetRenderState(type, m_rsStack[i].second);
                m_rsStack.erase(m_rsStack.begin() + i);
                break;
            }
        }
    }

    void RenderDevice::pushWorld(const Matrix & matrix)
    {
        assert(m_matWorlds.size() < 0xffff);

        m_matWorlds.push_back(matrix);
        m_matDirty |= DirtyWorldViewProj;
    }

    void RenderDevice::popWorld()
    {
        assert(!m_matWorlds.empty());
        m_matWorlds.pop_back();
        m_matDirty |= DirtyWorldViewProj;
    }

    void RenderDevice::setView(const Matrix & matrix)
    {
        m_matView = matrix;
        m_matDirty |= (DirtyWorldViewProj | DirtyViewProj | DirtyInView);
    }

    const Matrix & RenderDevice::getView() const
    {
        return m_matView;
    }

    void RenderDevice::setProj(const Matrix & matrix)
    {
        m_matProj = matrix;
        m_matDirty |= (DirtyWorldViewProj | DirtyViewProj);
    }

    const Matrix & RenderDevice::getProj() const
    {
        return m_matProj;
    }

    const Matrix & RenderDevice::getInvView() const
    {
        if (m_matDirty & DirtyInView)
        {
            m_matView.getInvert(m_matInvView);
            m_matDirty &= ~DirtyInView;
        }
        return m_matInvView;
    }

    const Matrix & RenderDevice::getViewProj() const
    {
        if (m_matDirty & DirtyViewProj)
        {
            m_matViewProj = m_matView * m_matProj;
            m_matDirty &= ~DirtyViewProj;
        }
        return m_matViewProj;
    }

    const Matrix & RenderDevice::getWorldViewProj() const
    {
        if (m_matDirty & DirtyWorldViewProj)
        {
            m_matWorldViewProj = m_matWorlds.back() * m_matView * m_matProj;
            m_matDirty &= ~DirtyWorldViewProj;
        }
        return m_matWorldViewProj;
    }

    void RenderDevice::applyWorld()
    {
        m_device->SetTransform(D3DTS_WORLD, &m_matWorlds.back());
    }
    
    void RenderDevice::applyView()
    {
        m_device->SetTransform(D3DTS_VIEW, &m_matView);
    }

    void RenderDevice::applyProj()
    {
        m_device->SetTransform(D3DTS_PROJECTION, &m_matProj);
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