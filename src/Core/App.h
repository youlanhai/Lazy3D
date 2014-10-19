//App.h
#pragma once

#include "../utility/UtilConfig.h"

#include "Keyboard.h"
#include "RenderTask.h"

#include "../Render/RenderDevice.h"

namespace Lazy
{
    namespace LoadingType
    {
        const int None = 0;
        const int Initialize = 1;
        const int SpaceChange = 2;
    }


    /**应用程序类*/
    class LZDLL_API CApp : public IBase, public IEventReceiver
    {
    public:
        CApp(void);

        virtual ~CApp(void);

    public:

        //不同的两种创建方式
        //方法1.会创建一个默认的windows窗口，使用默认的消息处理函数。

        /** 游戏类初始化。
         *  @param hInstance  应用程序实例对象
         *  @param caption    窗口标题
         *  @param nWidth     窗口宽度
         *  @param nHeight    窗口高度
         *  @param bFullScreen 是否全屏
         */
        bool create(HINSTANCE hInstance,
                    const std::wstring & caption,
                    int nWidth,
                    int nHeight,
                    bool bFullScreen);

        ///消息处理
        virtual bool onEvent(const SEvent & event) override;

        ///退出游戏
        void quitGame(void);

        ///处理消息。返回false，表示程序退出。
        bool processMessage();

        ///消息循环
        virtual void mainLoop();

        //方法2.不创建窗口，利用外部创建好的窗口。
        //外部需要在游戏循环的地方调用run，在游戏结束后调用clear；
        //调用onEvent处理windows消息。

        ///创建app。
        bool createEx(HINSTANCE hInstance,
                      HWND hWnd,
                      const std::wstring & strCaption,
                      int nWidth,
                      int nHeight,
                      bool bFullSrcreen);

        ///运行
        virtual void run(void) ;

        ///释放资源
        virtual void clear(void) ;

    protected:

        /**游戏初始化*/
        virtual bool init(void) ;

        /**更新*/
        virtual void update(void);

        /**渲染*/
        virtual void render(void) ;

        /**注册窗口类消息。请实现该消息来修改窗口风格。*/
        virtual void onRegisterClass(WNDCLASSEX *pwc) ;

        /**创建设备消息。实现该方法来修改设备。*/
        virtual void onCreateDevice(D3DPRESENT_PARAMETERS * pParam) ;

        /** 释放单例资源。*/
        virtual void releaseSingleton();

        /** 创建单例对象*/
        virtual void createSingleton();

    private:

        /**注册窗口类*/
        void registerClass();

        /**创建窗口*/
        bool createWindow();

        /**初始化渲染设备*/
        bool createDevice();

    public://属性

        ///获得绘图设备
        dx::Device * getDevice(void) { return m_pd3dDevice; }

        ///获得窗口宽度
        int getWidth(void) const { return m_nWidth; }

        ///获得窗口高度
        int getHeight(void) const { return m_nHeight; }

        bool isFullScreen(void) const { return m_bFullScreen; }

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

        void addDrawTask(IRenderObj* rend);
        void delDrawTask(IRenderObj* rend);

        void addTickTask(IRenderObj* up);
        void delTickTask(IRenderObj* up);

        void addDrawTickTask(IRenderObj *pObj);
        void delDrawTickTask(IRenderObj* pObj);

        bool isMsgHooked(void) { return m_bMsgHooked; }

        CRect   getClientRect(void);
        void    screenToClient(POINT* pt);
        void    clientToScreen(POINT* pt);

        CPoint  getCursorPos(void);
        void    setCursorPos(CPoint pt);

    public:

        //多线程Loading

        bool isGameLoading() const { return m_isLoading; }
        virtual void startGameLoading(int type);
        virtual void stopGameLoading() { m_isLoading = false; }

        bool isGameLoadingOK() const { return m_isLoadingOK; }
        bool setGameLoadingStatus(bool ok) { m_isLoadingOK = ok; }

    protected:
        dx::Device *        m_pd3dDevice;      //Direct3D设备对象
        HINSTANCE			m_hInstance;		//应用程序实例对象
        HWND				m_hWnd;				//窗口句柄
        int		            m_nWidth;			//窗口宽度
        int		            m_nHeight;			//窗口高度
        tstring             m_caption;	        //窗口标题
        float               m_fElapse;
        float               m_timeScale;

        bool	            m_bFullScreen;		//是否全屏
        bool                m_bMsgHooked;

        bool                m_isLoading;
        bool                m_isLoadingOK;
        int                 m_loadingType;
        float               m_loadingElapse;

        //公共资源
        RenderTaskPtr       m_pRenderTaskMgr;   //< 渲染队列
        RenderTaskPtr       m_pUpdateTaskMgr;   //< 更新队列
        KeyboardPtr	        m_pKeyboard;
    };

}// end namespace Lazy
