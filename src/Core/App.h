//App.h
#pragma once

#include "../utility/UtilConfig.h"
#include "../utility/FPS.h"

#include "Keyboard.h"
#include "RenderTask.h"

#include "../Render/RenderDevice.h"

namespace Lazy
{
    /** 应用程序类。可以从此类派生，定制自己的游戏。*/
    class LZDLL_API CApp : public IBase, public IEventReceiver
    {
    public:
        CApp(void);
        virtual ~CApp(void);

        /** 应用程序初始化。创建窗口和渲染设备。
         *  @param hInstance  应用程序实例对象
         *  @param caption    窗口标题
         *  @param nWidth     窗口宽度
         *  @param nHeight    窗口高度
         *  @param bFullScreen 是否全屏
         *  也可以使用已经创建的窗口构造游戏类，详见@createEx。
         */
        bool create(HINSTANCE hInstance,
                    const std::wstring & caption,
                    int nWidth,
                    int nHeight,
                    bool bFullScreen);

        /** 应用程序初始化。不创建窗口，可以利用外部创建好的窗口来初始化游戏。
        *   注意：外部需要在游戏循环的地方调用run，在游戏结束后调用clear；
        *   调用onEvent处理windows消息。
        */
        bool createEx(HINSTANCE hInstance,
                      HWND hWnd,
                      const std::wstring & strCaption,
                      int nWidth,
                      int nHeight,
                      bool bFullSrcreen);

        /** 消息处理。*/
        virtual bool onEvent(const SEvent & event) override;

        /** windows消息循环*/
        virtual void mainLoop();

        /** 游戏运行一帧。*/
        virtual void run(void) ;

        /** 释放游戏资源。*/
        virtual void clear(void) ;

        /** 退出游戏*/
        void quitGame(void);

        /** 循环处理windows窗口消息，直到消息队列为空。
        *   返回false，表示程序退出。
        */
        bool processMessage();

        /** 改变窗口尺寸*/
        bool changeWindowSize(int width, int height);

        /** 改变窗口客户区大小*/
        bool changeClientSize(int width, int height);

        /** 改变全屏*/
        bool changeFullScreen(bool fullScreen);

    protected:

        /** 游戏初始化*/
        virtual bool init(void) ;

        /** 更新*/
        virtual void update(void);

        /** 渲染*/
        virtual void render(void) ;

        /** 注册窗口类。请实现该消息来修改窗口风格。*/
        virtual void onRegisterClass(WNDCLASSEX * /*pwc*/){}

        /** 创建窗口。实现该方法来定制窗口风格。*/
        virtual void onCreateWindow(CREATESTRUCT * /*pwin*/){}

        /** 创建渲染设备。实现该方法来修改设备。*/
        virtual void onCreateDevice(D3DPRESENT_PARAMETERS * /*pParam*/){}

        /** 释放单例资源。*/
        virtual void releaseSingleton();

        /** 创建单例对象*/
        virtual void createSingleton();

    private:

        /** 注册窗口类*/
        void registerClass();

        /** 创建窗口*/
        bool createWindow();

        /** 初始化渲染设备*/
        bool createDevice();

    public://属性

        ///获得绘图设备
        dx::Device * getDevice(void) { return m_pd3dDevice; }

        ///获得窗口宽度
        int getWidth(void) const { return m_nWidth; }

        ///获得窗口高度
        int getHeight(void) const { return m_nHeight; }

        BOOL isFullScreen(void) const { return m_bFullScreen; }

        HWND getHWnd(void) const { return m_hWnd; }

        HINSTANCE getInstance(void) const { return m_hInstance; }

        float elapse() const { return m_fElapse; }

        ///设置窗口标题
        void setCaption(tstring const & caption);
        const tstring & getCaption(void) const { return m_caption; }

        float   getTimeScale() const { return m_timeScale; }
        void    setTimeScale(float scale) { m_timeScale = scale; }

    public:
        KeyboardPtr getKeyboard(void) { return m_pKeyboard; }

        void addDrawTask(IRenderable* rend);
        void delDrawTask(IRenderable* rend);

        void addTickTask(IRenderable* up);
        void delTickTask(IRenderable* up);

        void addDrawTickTask(IRenderable *pObj);
        void delDrawTickTask(IRenderable* pObj);

        bool isMsgHooked(void) { return m_bMsgHooked; }

        CRect   getClientRect(void);
        void    screenToClient(POINT* pt);
        void    clientToScreen(POINT* pt);

        CPoint  getCursorPos(void);
        void    setCursorPos(CPoint pt);

    protected:
        dx::Device *        m_pd3dDevice;      //Direct3D设备对象
        HINSTANCE			m_hInstance;		//应用程序实例对象
        HWND				m_hWnd;				//窗口句柄
        int		            m_nWidth;			//窗口宽度
        int		            m_nHeight;			//窗口高度
        tstring             m_caption;	        //窗口标题
        float               m_fElapse;
        float               m_timeScale;
        Fps                 m_fps;

        BOOL	            m_bFullScreen;		//是否全屏
        BOOL                m_bWindowMinimized;
        BOOL                m_bWindowResized;
        DWORD               m_oldWindowStyle;
        CRect               m_oldWindowRect;

        bool                m_bMsgHooked;

        //公共资源
        RenderTaskPtr       m_pRenderTaskMgr;   //< 渲染队列
        RenderTaskPtr       m_pUpdateTaskMgr;   //< 更新队列
        KeyboardPtr	        m_pKeyboard;

        friend class RenderDevice;
    };

}// end namespace Lazy
