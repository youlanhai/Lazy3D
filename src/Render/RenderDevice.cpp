﻿#include "stdafx.h"

#include "RenderDevice.h"
#include "Device.h"
#include "Effect.h"
#include "ResMgr.h"
#include "ResFactory.h"
#include "Texture.h"
#include "Mesh.h"
#include "EffectConstantSetter.h"
#include "ShadowMap.h"

#include "../Core/App.h"

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
        const uint32 DirtyWorldView = 1 << 3;

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
        , m_isFullScreen(false)
        , m_isRendering(false)
        , m_backBuffWidth(0)
        , m_backBuffHeight(0)
        , m_backBuffFormat(D3DFMT_UNKNOWN)
        , m_matDirty(0)
    {
        assert(s_oneInstance == NULL && "initialize more tha once!");

        s_oneInstance = this;
        ZeroMemory(&m_d3dmm, sizeof(m_d3dmm));
        ZeroMemory(&m_d3dcaps, sizeof(m_d3dcaps));

        pushWorld(matIdentity);

        DeviceMgr::initInstance();
        ShadowMap::initInstance();
    }

    RenderDevice::~RenderDevice()
    {
        destroy();

        DeviceMgr::finiInstance();
        ShadowMap::finiInstance();
    }

    void RenderDevice::fillPresentParameter()
    {
        ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));

        //设置D3DPRESENT_PARAMETERS结构, 准备创建Direct3D设备对象
        m_d3dpp.Windowed = !m_isFullScreen;
        m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        m_d3dpp.EnableAutoDepthStencil = TRUE;
        m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

        m_d3dpp.BackBufferFormat = m_d3dmm.Format;
        m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

        if (m_isFullScreen)
        {
            m_d3dpp.BackBufferWidth = m_d3dmm.Width;
            m_d3dpp.BackBufferHeight = m_d3dmm.Height;
            m_d3dpp.FullScreen_RefreshRateInHz = m_d3dmm.RefreshRate;
        }
        else
        {
            CRect rc;
            GetClientRect(m_hWnd, &rc);
            m_d3dpp.BackBufferWidth = rc.width();
            m_d3dpp.BackBufferHeight = rc.height();
        }

        getApp()->onCreateDevice(&m_d3dpp);

        m_backBuffFormat = m_d3dpp.BackBufferFormat;
        m_backBuffWidth = m_d3dpp.BackBufferWidth;
        m_backBuffHeight = m_d3dpp.BackBufferHeight;
    }

    ///初始化
    bool RenderDevice::create(HWND hWnd, HINSTANCE hInstance, bool fullSrc)
    {
        assert(m_device == nullptr && "RenderDevice::create - the device has been created!");

        m_hWnd = hWnd;
        m_hInstance = hInstance;
        m_isFullScreen = fullSrc;

        //创建Direct3D对象, 该对象用于创建Direct3D设备对象
        ComPtr<dx::D3D> pd3d = Direct3DCreate9(D3D_SDK_VERSION);
        if (!pd3d)
        {
            LOG_ERROR(L"Create d3d failed!");
            return false;
        }
        pd3d.delRef();

        D3DADAPTER_IDENTIFIER9 adapterInfo;
        if (FAILED(pd3d->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &adapterInfo)))
        {
            LOG_ERROR(L"Can't get the infomation of default video adapter.");
            return false;
        }

        LOG_INFO(L"Driver: %S", adapterInfo.Driver);
        LOG_INFO(L"Description: %S", adapterInfo.Description);
        LOG_INFO(L"DeviceName: %S", adapterInfo.DeviceName);
        LOG_INFO(L"DriverVersion: %d %u", adapterInfo.DriverVersion.HighPart, adapterInfo.DriverVersion.LowPart);

        if (FAILED(pd3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_d3dcaps)))
        {
            LOG_ERROR(L"Doesn't support hardware device.");
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

        m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
        m_device->SetRenderState(D3DRS_ZENABLE, TRUE);
        m_device->SetRenderState(D3DRS_LIGHTING, FALSE);
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

        DeviceMgr::instance()->onCreateDevice();

        ResourceFactoryPtr factory = new ResFactory();
        TextureMgr::instance()->setResFacotry(factory);
        MeshMgr::instance()->setResFacotry(factory);
        EffectMgr::instance()->setResFacotry(factory);
        EffectConstantSetter::init();

        Mesh::initShaders();
        return true;
    }

    void RenderDevice::destroy()
    {
        if (!m_device)
            return;

        EffectConstantSetter::fini();
        DeviceMgr::instance()->onCloseDevice();

        m_device->Release();
        m_device = NULL;
        m_hWnd = NULL;
        m_hInstance = NULL;
    }

    ///重置设备
    bool RenderDevice::resetDevice()
    {
        fillPresentParameter();

        HRESULT hr = m_device->Reset(&m_d3dpp);
        if (FAILED(hr))
        {
            LOG_ERROR(_T("Reset Device failed! 0x%x"), hr);
            return false;
        }

        LOG_INFO(_T("reset device. width=%d, height=%d"), m_backBuffWidth, m_backBuffHeight);

        m_isLost = false;
        DeviceMgr::instance()->onResetDevice();
        return true;
    }

    void RenderDevice::clear(DWORD flag, D3DCOLOR cr, float z, DWORD stencil)
    {
        m_device->Clear(0, NULL, flag, cr, z, stencil);
    }

    bool RenderDevice::beginScene()
    {
        assert(!m_isRendering);

        while(m_isLost)
        {
            HRESULT hr = testCooperativeLevel();
            if (hr == D3DERR_DEVICENOTRESET)
            {
                if (resetDevice())
                {
                    break;
                }
            }

            Sleep(1000);
            return false;
        }

        if (FAILED(m_device->BeginScene()))
            return false;

        m_isRendering = true;
        return true;
    }

    void RenderDevice::endScene()
    { 
        assert(m_isRendering);

        m_device->EndScene();
        m_isRendering = false;
    }

    void RenderDevice::present()
    {
        HRESULT hr = m_device->Present(NULL, NULL, NULL, NULL);
        
        if (hr == D3DERR_DEVICELOST)
        {
            if (!m_isLost)
            {
                LOG_ERROR(_T("Present Failed, Device Lost."));
                m_isLost = true;
                DeviceMgr::instance()->onLostDevice();
            }
        }
    }

    bool RenderDevice::resetDeviceSafely()
    {
        if (m_isLost || m_isRendering) return false;

        m_isLost = true;
        DeviceMgr::instance()->onLostDevice();
        return resetDevice();
    }

    void RenderDevice::onWindowResize(int width, int height)
    {
        resetDeviceSafely();
    }

    bool RenderDevice::onChangeFullScreen(bool fullScreen)
    {
        m_isFullScreen = fullScreen;
        return resetDeviceSafely();
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
        m_matDirty |= DirtyWorldViewProj | DirtyWorldView;
    }

    void RenderDevice::popWorld()
    {
        assert(!m_matWorlds.empty());
        m_matWorlds.pop_back();
        m_matDirty |= DirtyWorldViewProj | DirtyWorldView;
    }

    void RenderDevice::setWorld(const Matrix & world)
    {
        assert(!m_matWorlds.empty());
        m_matWorlds.back() = world;
        m_matDirty |= DirtyWorldViewProj | DirtyWorldView;
    }

    const Matrix & RenderDevice::getWorld() const
    {
        assert(!m_matWorlds.empty());
        return m_matWorlds.back();
    }

    void RenderDevice::setView(const Matrix & matrix)
    {
        m_matView = matrix;
        m_matDirty |= (DirtyWorldViewProj | DirtyViewProj | DirtyInView | DirtyWorldView);
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

    const Matrix & RenderDevice::getWorldView() const
    {
        if (m_matDirty & DirtyWorldView)
        {
            m_matWorldView = m_matWorlds.back() * m_matView;
            m_matDirty &= ~DirtyWorldView;
        }
        return m_matWorldView;
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