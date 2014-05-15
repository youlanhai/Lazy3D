﻿#pragma once

namespace Lazy
{

    ///渲染设备
    class RenderDevice
    {
        RenderDevice(const RenderDevice &);
        const RenderDevice & operator=(const RenderDevice &);

    public:
        RenderDevice();
        ~RenderDevice();

        ///创建dx设备
        bool create(HWND hWnd, HINSTANCE hInstance, bool fullSrc=false);
        void destroy();

        ///设备是否丢失
        bool isLost() const { return m_isLost; }

        void clear(DWORD flag, D3DCOLOR cr, float z, DWORD stencil);
        bool beginScene();
        void endScene(){ m_device->EndScene(); }
        void present();

        ///改变窗口尺寸
        bool changeSize(int width, int height);
        int windowWidth() const { return m_windowWidth; }
        int windowHeight() const { return m_windowHeight; }

        ///改变全屏
        bool changeFullScreen(bool fullSrc);

        dx::Device *getDevice() { return m_device; }
        const D3DCAPS9 *getCaps() const { return &m_d3dcaps; }

        ///设置渲染参数
        void setRenderState(dx::RSType type, DWORD value);
        void getRenderState(dx::RSType type, DWORD & value);
        DWORD getRenderState(dx::RSType type);

        //设置变换矩阵
        void setTransform(dx::TSType type, D3DXMATRIX & matrix);
        void getWorldViewProj(D3DXMATRIX & mat);

        HRESULT testCooperativeLevel();
        HRESULT setTexture(DWORD stage, dx::BaseTexture *pTexture);
        HRESULT setfvf(DWORD fvf);
        HRESULT drawPrimitiveUP(dx::PTType type, size_t num, const void *stream, size_t stride);


        ///重置设备
        bool resetDevice();


        void setFontShader(const tstring & shaderFile){ m_fontShader = shaderFile; }
        const tstring & getFontShader(){ return m_fontShader; }

    protected:

        void fillPresentParameter();

    private:
        HWND        m_hWnd;
        HINSTANCE   m_hInstance;
        dx::Device  *m_device;
        bool        m_isLost;
        bool        m_fullScreen;
        int         m_windowWidth;
        int         m_windowHeight;

        DWORD       m_oldStyle;
        RECT        m_oldRect;
        D3DPRESENT_PARAMETERS m_d3dpp;
        D3DDISPLAYMODE m_d3dmm;
        D3DCAPS9    m_d3dcaps;

        tstring     m_fontShader;
    };

    ///获取渲染设备
    RenderDevice *rcDevice();



    /** 带纹理颜色顶点*/
    struct UIVertex
    {
        float x, y, z;
        D3DCOLOR color;
        float tu, tv;

        static DWORD FVF;
        static int SIZE;
    };


    /** 带颜色顶点*/
    struct UIVertex_UNTEX
    {
        float x, y, z;
        D3DCOLOR color;

        static DWORD FVF;
        static int SIZE;
    };
}