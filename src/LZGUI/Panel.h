#pragma once

#include "Control.h"



//////////面板类///////////////////
/* 面板可以包含子控件，实现所有UI资源的统一管理，
** 每一个应用程序最好只包含一个面板对象。
*/
class LZGUI_API CPanel : public IControl
{
public:
    typedef std::list<PControl>             ControlList;
    typedef ControlList::iterator           ControlIter;
    typedef ControlList::reverse_iterator   ControlRIter;

public:
    /** 构造函数*/
    CPanel(void);

    /** 构造函数。该方法会调用create方法。*/
    CPanel(int id, PControl parent, int x, int y);

    /** 析构函数*/
    ~CPanel();

    /** 获得类名。*/
    virtual LPCSTR getClassName(void){ return "CUIPane"; }

    /** 是容器类。*/
    virtual bool isContainer(void){ return true; }

    /** 创建*/
    virtual void create(int id, PControl parent, int x, int y);

    virtual void update(float fElapse);
    virtual void render(IDirect3DDevice9 * pDevice);

    virtual UINT preProcessMsg(UINT msg, WPARAM wParam, LPARAM lParam);
    virtual UINT messageProc(UINT msg, WPARAM wParam, LPARAM lParam);

    //添加控件，获得控件
    virtual void addChild(PControl pCtrl);
    virtual PControl getChild( int id );
    virtual void removeChild(PControl pCtrl);

    /** 判断控件是否为激活状态。*/
    virtual bool isChildActive(PControl ctrl);

    //ControlList* getChildren(void){ return &m_children; }

    void enableHandleSelfMsg(bool e) { m_canHandleSelfMsg = e;   }

    bool canHandleSelfMsg(){return m_canHandleSelfMsg; }

    ///清除子控件
    void clearChildren();

protected:
    //ControlList		m_children;		//控件列表
    VisitControl        m_childrenEx; //绘制和更新用
    bool                m_canHandleSelfMsg;
};