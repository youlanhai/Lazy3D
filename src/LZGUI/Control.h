#pragma once

//#define LZ3DENGINE_STATIC_LINK

#include "../Core/Core.h"


#ifndef LZ3DENGINE_DYNAMIC_LINK
#   define LZGUI_API
#else
#   ifdef LZGUI_EXPORTS
#       define LZGUI_API __declspec(dllexport)
#   else
#       define LZGUI_API __declspec(dllimport)
#   endif
#endif


#include <vector>
#include <list>
#include <string>
#include <algorithm>

typedef std::string stdstring;

#define WM_MOUSEENTERS   WM_USER+1
#define WM_MOUSELEAVES  WM_USER+2

class IControl;
typedef IControl*   PControl;

class GUIManagerBase;
LZGUI_API GUIManagerBase* getGUIMgr(void); ///获得gui管理器


/** 预处理消息。*/
enum PRE_PRO_MSG
{
    PRE_MSG_OK = 0, //可以处理
    PRE_MSG_NO = 1, //不可以处理
    PRE_MSG_BREAK = 2, //中断消息
};

/** 按钮状态*/
enum BUTTON_STATE
{
    BS_NORMAL = 0,  //< 整常
    BS_ACTIVE,      //< 活动
    BS_DOWN,        //< 按下
    BS_DISABLE,     //< 禁用
    BS_HOT,         //< 热点
};

//////////////////////////////////////////////////////////////////////////

/** 带纹理颜色顶点*/
struct UIVertex
{
    float x, y, z, h;
    D3DCOLOR color;
    float tu, tv;

    void set(float x_, float y_, float z_, float h_, D3DCOLOR cr_, float tu_, float tv_);
    static DWORD FVF;
    static int SIZE;
};

/** 带颜色顶点*/
struct UIVertex_UNTEX
{
    float x, y, z, h;
    D3DCOLOR color;

    void set(float x_, float y_, float z_, float h_, D3DCOLOR cr_);
    static DWORD FVF;
    static int SIZE;
};

//////////////////////////////////////////////////////////////////////////
/** UI控件接口
    IControl为所有控件类的基类，实现所有控件统一管理。
    所有派生类必须实现IRenderObj接口（update，render）,以完成绘制和更新；
    所有控件的parent，必须指定为GUIMgr或者其它三个程序预置的Panel，详见CGUIManager。
    GUIMgr，管理了整个UI系统，将所有的UI关联起来，形成一颗UI树。实现了UI消息传递，
    以及统一更新和渲染。详见CGUIManager。
    */
class LZGUI_API IControl : public IRenderObj, public IBase
{
public:
    /// 构造函数
    IControl(void);

    /// 析构函数
    virtual ~IControl(void);

    ///初始化窗体
    void init(int id, PControl parent, stdstring caption, int x, int y);

    ///获得类名称
    virtual LPCSTR getClassName(void){ return "not set"; }

    ///是否是容器
    virtual bool isContainer(void){ return false; }

    ///更新
    virtual void update(float fElapse){}

    ///绘制
    virtual void render(IDirect3DDevice9 * pDevice){}

public://消息处理。
    /** 预处理消息。如果消息不属于当前控件，则可以提前结束。*/
    virtual UINT preProcessMsg(UINT msg, WPARAM wParam, LPARAM lParam);

    /** 消息处理过程。*/
    virtual UINT messageProc(UINT msg, WPARAM wParam, LPARAM lParam);

    /** 消息过程。仅供UI库内部使用，外部处理响应消息请使用onMessageProc。*/
    virtual void msgProc(UINT msg, WPARAM wParam, LPARAM lParam);

    /** 消息通知*/
    virtual void onMessageProc(UINT msg, WPARAM wParam, LPARAM lParam);

    /** 键盘按键消息处理。*/
    virtual void onKey(bool isDown, UINT key, UINT param);

    /** 鼠标移动消息处理。*/
    virtual void onMouseMove(CPoint pt, CPoint old);

    /** 鼠标进入消息处理。*/
    virtual void onMouseEnter(CPoint pt);

    /** 鼠标离开消息处理。*/
    virtual void onMouseLeave(CPoint pt);

    /** 鼠标左键消息处理。*/
    virtual void onMouseLButton(bool isDown, CPoint pt);

    /** 鼠标右键消息处理。*/
    virtual void onMouseRButton(bool isDown, CPoint pt);

    /** 鼠标双击消息处理。*/
    virtual void onMouseDBClick(bool left, CPoint pt);

    /** 鼠标滚轮消息处理。*/
    virtual void onMouseWheel(int zDelta, CPoint pt);

    /** 得到焦点。*/
    virtual void onFocus();

    /** 失去焦点。*/
    virtual void onLostFocus();

    /** 显示消息处理。*/
    virtual void onShow(bool show);

    /** 关闭消息处理。*/
    virtual void onClose(void);

    /** 鼠标拖拽。*/
    virtual void drag(CPoint cur, CPoint old);

    /** 拖拽完毕。*/
    virtual void onDragEnd(void);

    /** 激活窗体。*/
    virtual void active(void);

    /** 激活消息处理。*/
    virtual void onActive(void);

    /** 鼠标离开当前控件*/
    virtual void mouseLeave(void){ m_bMouseIn = false;}

    void trackMouse(CPoint pt);
public://子控件消息
    
    /** button消息。*/
    virtual void onButton(int id, bool isDown){}

    /** 编辑框消息。*/
    virtual void onTextChange(int id, stdstring oldText){}

    /** 滚动条消息。*/
    virtual void onScroll(int id, float pos){}

public: //坐标系转换

    /** 局部坐标转换到父控件坐标系。*/
    CPoint localToParent(CPoint pt);
    CRect localToParent(CRect rc);

    CPoint localToGlobal(CPoint pt);
    CRect localToGlobal(CRect rc);

    CPoint parentToLocal(CPoint pt);
    CRect parentToLocal(CRect rc);

    CPoint globalToLocal(CPoint pt);
    CRect globalToLocal(CRect rc);

public://属性

    ///设置ID。
    void setID(int id){ m_id = id; }
    int  getID( void ){ return m_id; }


    IControl* getParent(void){ return m_parent; }
    void setParent(IControl* p){ m_parent = p; }

    ///设置标题
    virtual void setCaption(stdstring caption){ m_caption = caption; }
    stdstring getCaption(void){ return m_caption; }

    ///设置字体
    void setFont(stdstring name){ m_fontName = name;}
    stdstring getFont(void){ return m_fontName; }

    ///设置前景颜色
    virtual void setColor(COLORREF cr){ m_crColor=cr; }	
    COLORREF getColor(void){ return m_crColor; }

    ///设置背景色
    virtual void setBackColor(COLORREF cr){ m_crBackColor = cr; }
    virtual COLORREF getBackColor(void){ return m_crBackColor; }

    ///设置位置
    virtual void setPosition(int x, int y){ m_position.set(x,y);}
    virtual CPoint getPosition(void){ return m_position; }

    ///设置尺寸
    virtual void setSize(int w, int h){ m_size.set(w, h); }
    virtual CSize getSize(void){ return m_size ; }

    //设置控件区域
    CRect getControlRect(void);
    CRect getClientRect(void);

    ///设置背景图像名称
    virtual void setImage(stdstring image){ m_bkImage = image;}
    virtual stdstring getImage(void) { return m_bkImage;}

public://开关设置

    ///设置可见
    virtual void show(bool bShow);
    virtual void toggle(void);
    bool isVisible(void){ return m_bVisible; }

    ///设置是否可用
    void enable(bool e){ m_bEnable=e; }
    bool isEnable(void){ return m_bEnable; }

    ///设置是否可拖拽窗体
    void enableDrag(bool e){ m_bDrag = e; }
    bool canDrag(void){ return m_bDrag; }

    ///是否启用鼠标跟踪。用于发出mouseEnter和leave消息。
    void enableTrack(bool e) { m_bTrackMouse = e; }
    bool canTrack(void){ return m_bTrackMouse; }

    ///是否可处理消息。
    void enableHandleMsg(bool e){ m_bHandleMsg = e; }
    bool canHandleMsg(void){ return m_bHandleMsg; }

    ///鼠标左键是否在控件按下。用于拖拽等操作。
    bool isLButtonDown(void){ return m_bLButtonDown ; }
    void setLButtonDown(bool bDown){ m_bLButtonDown = bDown; }

    ///点击是否移动到上层。
    void enableClickTop(bool e){ m_bClickTop = e; }
    bool canClickTop(void){ return m_bClickTop;}

public: //对子控件操作
    ///添加子控件
    virtual void addChild(PControl pCtrl) {}

    ///获得控件
    virtual PControl getChild( int id ) { return NULL; }

    ///移除一个子控件
    virtual void removeChild(PControl pCtrl){}

    ///子控件是否活动。用于响应clickTop优化判断。
    virtual bool isChildActive(PControl ctrl){ return true; }

    ///清除子控件
    virtual void clearChildren();

protected:
    int				m_id;			//< 编号。
    PControl        m_parent;       //< 父控件
    stdstring		m_caption;		//< 标题
    stdstring		m_fontName;		//< 字体名称
    stdstring       m_bkImage;      //< 背景图
    COLORREF		m_crBackColor;	//< 前景色
    BYTE            m_alpha;
    COLORREF		m_crColor;	    //< 背景色
    CPoint          m_position;     //< 位置
    CSize           m_size;         //< 尺寸
    bool			m_bVisible;		//< 窗体是否可见
    bool			m_bEnable;		//< 是否可用
    bool            m_bDrag;        //< 是否可拖拽
    bool            m_bTrackMouse;  //< 是否跟踪鼠标
    bool            m_bMouseIn;     //< 鼠标是否在区域中
    bool            m_bLButtonDown; //< 鼠标左键是否按下
    bool            m_bHandleMsg;   //< 是否可处理消息
    bool            m_bClickTop;    //< 是否点击之后排到顶层
};

//////////////////////////////////////////////////////////////////////////

struct VisitControl : public ::VisitPool<IControl>
{
    void update(float elapse)
    {
        lock(true);
        for (VisitRIterator it=rbegin(); it!=rend(); ++it)
        {
            if (it->canOperate())
            {
                (it->obj())->update(elapse);
            }
        }
        lock(false);
        removeDead();
    }
    void render(IDirect3DDevice9* pDevice)
    {
        lock(true);
        for (VisitIterator it=begin(); it!=end(); ++it)
        {
            if (it->canOperate())
            {
                (it->obj())->render(pDevice);
            }
        }
        lock(false);
    }
};

//////////////////////////////////////////////////////////////////////////

/** 颜色渐变。
    给定当前目标颜色、渐变时间，程序会在每次update时重新计算当前颜色，通过getCurColor方法，
    取得当前颜色。
    渐变速度：vi = (di-si)/t。每次渐变增量为dc = vi*elapse。
    */
class LZGUI_API CColorFade : IUpdate
{
public:
    /** 构造函数*/
    CColorFade(void);

    /** 重置参数*/
    void reset(COLORREF crCur, COLORREF crDest, float time=-1.0f);
    void setSource(PControl ps){ m_source = ps; }

    /** 渐变。*/
    void update(float fElapse);

    /** 设置目标颜色。每次调用此函数的时候，会重新计算渐变速度，重置参数。*/
    void setDestColor(COLORREF cr);

    /** 获得当前颜色。该颜色为所需的渐变色。*/
    COLORREF getCurColor(void){ return m_crCur; }
private:
    PControl    m_source;
    COLORREF    m_crDest;   //< 目标颜色
    COLORREF    m_crCur;    //< 当前颜色
    float       m_time;     //< 渐变所用时间
    float       m_elapse;   //< 已用时间
    float       m_v[3];     //< rgb分量渐变速度。
};
