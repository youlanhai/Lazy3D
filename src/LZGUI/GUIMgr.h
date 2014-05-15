#pragma once

#include "Panel.h"

/** UI管理器基类。*/
class LZGUI_API GUIManagerBase : public CPanel
{
public:
    ///构造函数。使用默认参数。noArgs无意义。
    GUIManagerBase(bool noArgs);

    /** 构造函数。*/
    GUIManagerBase(IDirect3DDevice9 * pDevice, HWND hWnd, HINSTANCE hInst);

    virtual ~GUIManagerBase(void);

    virtual LPCSTR getClassName(void){ return "GUIManagerBase"; }

    virtual void render(IDirect3DDevice9 * pDevice);

    void initLayotPanel(CPanel* top, CPanel* normal, CPanel* bottom);

protected:

    bool init(IDirect3DDevice9 * pDevice, HWND hWnd, HINSTANCE hInst);

public: //消息处理
    virtual UINT messageProc(UINT msg, WPARAM wParam, LPARAM lParam);

    /** 消息过滤器。位于m_msgFilter中的消息会被执行，其余消息会跳过*/
    virtual bool messageFilter(UINT msg);

    ///添加一个消息
    void addFilterMsg(UINT msg);

    ///移除一个消息
    void removeFilterMsg(UINT msg);
    
    ///得到焦点控件
    virtual PControl getFocusCtl(void);

    ///设置焦点控件
    virtual void setFocusCtl(PControl ptr);

    ///获得当前鼠标位置
    CPoint getCursorPos(void){ return m_cursorPos; }

    ///获得上次鼠标位置
    CPoint getLastCursorPos(void){ return m_lastCursorPos; }

    /** 添加一个被跟踪者。用于标记当前正活动的控件，主要用于发出mouseLeave消息。*/
    void addMouseTracker(PControl ctrl);
    void updateMouseTracker(void);

    /** 添加一个拖拽跟踪者。*/
    void addDragTracker(PControl ctrl);
    void updateDragTracker(void);
    void cancelAllDrag(void);

    ///添加到管理池
    void addToPool(PControl pctrl);

    ///从管理池移除。仅从链表移除，不释放资源。
    void removeFromPool(PControl pctrl);

    ///释放pool里的资源。
    void releasePool(void);

public ://属性
    ///获得绘图设备
    IDirect3DDevice9* getRenderDevice(void){ return m_pRenderDevice;}

    ///获得窗口句柄
    HWND getHWnd(void){ return m_hWnd;}

    ///获得应用程序实例
    HINSTANCE getInstance(void){ return m_hInstance; }

    virtual void addChild(PControl pCtrl);
    virtual PControl getChild( int id );

    /** 三个内置panel，用于排列子控件顺序。
        子控件父亲指定为UIMgr的控件，其父亲会被设为normalPanel。
        */
    ///最顶层Panel
    CPanel* topMostPanel(void);
    
    ///常规层panel
    CPanel* normalPanel(void);
    
    ///最低层panel
    CPanel* bottomMostPanel(void);

    
protected:
    IDirect3DDevice9*   m_pRenderDevice;
    HWND                m_hWnd;
    HINSTANCE           m_hInstance;

    CPanel*         m_topMost; //< 最顶部控件
    CPanel*         m_normal;   //< 正常控件
    CPanel*         m_bottomMost;//< 最底层控件
    PControl        m_ptrFocusCtl;//< 焦点控件
    CPoint          m_cursorPos;    //< 当前鼠标位置
    CPoint          m_lastCursorPos;    //< 上一次鼠标位置
    std::vector<UINT> m_msgFilter;  //< 消息过滤
    ControlList     m_mouseTracker;//< 鼠标跟踪。记录了活动控件。
    ControlList     m_dragTracker; //< 拖拽跟踪。
    ControlList     m_managePool; //< 管理池
};

//////////////////////////////////////////////////////////////////////////

/** UI管理器。*/
class LZGUI_API CGUIManager : public GUIManagerBase
{
public:
    /** 构造函数。*/
    CGUIManager(IDirect3DDevice9 * pDevice, HWND hWnd, HINSTANCE hInst);

    virtual ~CGUIManager(void);

    virtual LPCSTR getClassName(void){ return "CGUIManager"; }

protected:
    RefPtr<CPanel>  m_topPanel; //< 最顶部控件
    RefPtr<CPanel>  m_normalPanel;   //< 正常控件
    RefPtr<CPanel>  m_bottomPanel;//< 最底层控件
};

