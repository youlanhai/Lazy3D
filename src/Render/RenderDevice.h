#pragma once

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
        bool create(HWND hWnd, HINSTANCE hInstance, bool fullSrc = false);
        void destroy();

        ///设备是否丢失
        BOOL isLost() const { return m_isLost; }

        void clear(DWORD flag, D3DCOLOR cr, float z, DWORD stencil);
        bool beginScene();
        void endScene();
        void present();

        bool resetDeviceSafely();
        bool changeFullScreen(bool fullScreen);

        int         getBackBufferWidth() const { return m_backBuffWidth; }
        int         getBackBufferHeight() const { return m_backBuffHeight; }
        D3DFORMAT   getBackBufferFormat() const { return m_backBuffFormat; }

        dx::Device *getDevice() { return m_device; }
        const D3DCAPS9 *getCaps() const { return &m_d3dcaps; }

        ///设置渲染参数
        void  setRenderState(dx::RSType type, DWORD value);
        void  getRenderState(dx::RSType type, DWORD & value);
        DWORD getRenderState(dx::RSType type);

        void pushRS(dx::RSType type, DWORD value);
        void popRS(dx::RSType type);

        //设置变换矩阵
        void pushWorld(const Matrix & matrix);
        void popWorld();
        const Matrix & getWorld() const;

        void setView(const Matrix & matrix);
        const Matrix & getView() const;

        void setProj(const Matrix & matrix);
        const Matrix & getProj() const;

        const Matrix & getInvView() const;
        const Matrix & getViewProj() const;
        const Matrix & getWorldView() const;
        const Matrix & getWorldViewProj() const;

        void applyWorld();
        void applyView();
        void applyProj();


        HRESULT testCooperativeLevel();
        HRESULT setTexture(DWORD stage, dx::BaseTexture *pTexture);
        HRESULT setfvf(DWORD fvf);
        HRESULT drawPrimitiveUP(dx::PTType type, size_t num, const void *stream, size_t stride);


        void setMaterial(const D3DMATERIAL9 & material){ m_material = material; }
        const D3DMATERIAL9 & getMaterial() const{ return m_material; }

    private:

        bool resetDevice();

        void fillPresentParameter();

    private:
        HWND        m_hWnd;
        HINSTANCE   m_hInstance;
        dx::Device* m_device;

        BOOL        m_isLost;
        BOOL        m_isFullScreen;
        BOOL        m_isRendering;

        int         m_backBuffWidth;
        int         m_backBuffHeight;
        D3DFORMAT   m_backBuffFormat;

        D3DPRESENT_PARAMETERS   m_d3dpp;
        D3DDISPLAYMODE          m_d3dmm;
        D3DCAPS9                m_d3dcaps;

        D3DMATERIAL9  m_material;

        std::vector<std::pair<DWORD, DWORD>>  m_rsStack;

        std::vector<Matrix> m_matWorlds;
        Matrix              m_matView;
        Matrix              m_matProj;
        mutable Matrix      m_matInvView;
        mutable Matrix      m_matViewProj;
        mutable Matrix      m_matWorldView;
        mutable Matrix      m_matWorldViewProj;
        mutable uint32      m_matDirty;
    };

    ///获取渲染设备
    RenderDevice *rcDevice();



    /** 带纹理颜色顶点*/
    struct VertexXYZColorUV
    {
        float x, y, z;
        D3DCOLOR color;
        float tu, tv;

        static DWORD FVF;
        static int SIZE;
    };


    /** 带颜色顶点*/
    struct VertexXYZColor
    {
        float x, y, z;
        D3DCOLOR color;

        static DWORD FVF;
        static int SIZE;
    };
}