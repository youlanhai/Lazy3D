#pragma once

#include "UIDelegate.h"
#include "UIDefine.h"

namespace Lazy
{
    /** UI控件接口
     *  Widget为所有控件类的基类，实现所有控件统一管理.
     *  GUIMgr，管理了整个UI系统，将所有的UI关联起来，形成一颗UI树。实现了UI消息传递，
     *  以及统一更新和渲染。详见GUIMgr。
     */
    class LZUI_API Widget
    {
    public:
        MAKE_UI_HEADER(Widget);

        Widget(void);
        virtual ~Widget(void);

        ///释放资源。打破循环引用，释放资源。不要在构造函数中调用。
        virtual void destroy();

        ///更新
        virtual void update(float fElapse);

        ///绘制
        virtual void render(IUIRender * pDevice);

        ///创建一个子widget
        Widget * createWidget(const tstring & type);

        ///创建一个子widget
        template<typename T>
        T * createWidgetT();
        
        ///销毁一个子widget
        void deleteWidget(Widget *pWidget);

    public://消息处理。

        /** 向控件发送消息。
            sendEvent要负责过滤消息，将合法的消息传递给messageProc.
            发送成功，返回true，否则false。
        */
        virtual bool sendEvent(const SEvent& event);

        ///发送ui消息
        virtual bool sendUIEvent(uint32 msg, uint32 wparam, uint32 lparam);

        ///检查是否能处理消息
        virtual bool checkCanHandelMsg();

        ///激活控件。控件将位于最顶层。
        virtual void active(void);

        ///将控件至于顶层
        virtual void topmost();

        ///消息委托
        DelegateGroup delegate;

    public: //串行化

        ///从文件中加载布局
        virtual bool loadFromFile(const tstring & file);

        ///保存布局到文件
        virtual bool saveToFile(const tstring & file);

        virtual void loadFromStream(LZDataPtr config);
        virtual void saveToStream(LZDataPtr config);

        virtual void loadProperty(LZDataPtr config);
        virtual void saveProperty(LZDataPtr config);

    public://属性

        ///获取父
        Widget* getParent(void) { return m_parent; }

        ///获取父
        Widget* getParent(void) const { return m_parent; }

        void setName(const tstring & key) { m_name = key; }
        const tstring & getName() const { return m_name; }

        const tstring & getSkin() const { return m_skin; }
        void setSkin(const tstring & skin);

        void setGlobalPosition(int x, int y);
        void setAbsPosition(int x, int y);
        void setPosition(int x, int y);
        void setZOrder(int z);

        const CPoint & getGlobalPosition() const{ return m_globalPosition; }
        CPoint getAbsPosition() const{ return relative2absPosition(m_position); }
        const CPoint & getPosition(void) const { return m_position; }
        int getZOrder() const { return m_zorder; }

        ///设置尺寸
        virtual void setSize(int w, int h);
        void setWidth(int width);
        void setHeight(int height);

        const CPoint & getSize(void) const { return m_size; }
        int getWidth() const { return m_size.x; }
        int getHeight() const { return m_size.y; }

        CRect getGlobalRect() const;
        void setGlobalRect(const CRect & rc);

        ///点是否在当前区域
        bool isPointIn(int x, int y)  const;

        //设置控件区域
        CRect getControlRect(void)  const;
        CRect getClientRect(void) const;

        ///设置对齐方式
        void setAlign(uint32 align);
        uint32 getAlign() const { return m_align; }

        ///设置可见
        virtual void setVisible(bool bShow);
        bool getVisible(void) const { return m_visible; }

        ///设置是否可用
        void setEnable(bool enable) { m_enable = enable; }
        bool getEnable(void) const { return m_enable; }

        ///设置是否可拖拽窗体
        void setDragable(bool enable) { m_dragable = enable; }
        bool getDragable(void) const { return m_dragable; }

        ///启用/禁用处理自身消息。如果一个ui不处理自己的消息，他就相当于是个消息转发器，将消息转发到子控件
        void setMessagable(bool enable) { m_messagable = enable; }
        bool getMessagable(void) const { return m_messagable; }

        ///点击是否可移动到上层。
        void setTopable(bool enable) { m_topable = enable; }
        bool getTopable(void) const { return m_topable; }

        ///允许改变子控件的顺序。如果启用，点击子控件，控件的顺序会被提前。
        void setChildOrderable(bool can) { m_childOrderable = can; }
        bool getChildOrdeable() const { return m_childOrderable; }

        void setDrawable(bool enable) { m_drawable = enable; }
        bool getDrawable() const { return m_drawable; }

#ifdef ENABLE_SCRIPT

        /** borrw reference */
        Lzpy::object_base getSelf() { return m_self; }
        void setSelf(Lzpy::object_base self) { m_self = self; }

        const tstring & getScript() const { return m_script; }
        void setScript(const tstring & script) { m_script = script; }

    protected:
        /** python端脚本，ui类不持有python引用计数。*/
        Lzpy::object_base   m_self;
        tstring             m_script;

#endif

    public: //对子控件操作

        ///将本控件从父控件表中删除
        void removeFromParent();

        ///添加子控件
        virtual void addChild(Widget* pCtrl);

        ///删除子控件
        virtual void delChild(Widget* pCtrl);

        ///将子空间至于最顶层。
        virtual void topmostChild(Widget* ctrl);

        ///清除子控件
        void clearChildren();

        const WidgetChildren & getChildren() const { return m_children; }

        Widget* getChild(const tstring & name);

        template<typename T>
        T * getChildT(const tstring & name);

        void setChildOrderDirty() { m_bOrderDirty = true; }

        ///根据坐标查找活动的控件。pos的参考系为当前控件。
        Widget* findChildByPos(const CPoint & pos, bool resculy = false);

        Widget* getSkinChild(const tstring & name);

    public://静态方法

        static bool isVKDown(uint32 vk);
        static void setFocus(Widget* focus);
        static void setSeleced(Widget* pSelected);
        static void setActived(Widget* pActived);

    protected:

        virtual bool sendEventToChildren(const SEvent & event);

        ///处理事件
        virtual bool onEvent(const SEvent& event);

        /** 激活消息处理。*/
        virtual void onActive(void);

        /** 显示消息处理。*/
        virtual void onShow(bool show);

        /** 拖拽消息*/
        virtual void onDrag(const CPoint & delta, const CPoint & point);

        void onAlign(const CPoint & sizeDelta);
        virtual void onParentResize(const CPoint & newSize, const CPoint & oldSize);
        virtual void onResize(const CPoint & newSize, const CPoint & oldSize);

        CPoint abs2relativePosition(const CPoint & pt) const;
        CPoint relative2absPosition(const CPoint & pt) const;

        virtual void onPositionChange();
        virtual void onSizeChange();
        virtual void onParentPositionChange();

        void clearSkin();
        void createSkin();

    protected:
        tstring         m_name;         ///< 名称
        tstring         m_skin;
        Widget*         m_parent;       ///< 父控件
        CPoint          m_globalPosition;
        CPoint          m_position;     ///< 位置
        CPoint          m_size;         ///< 尺寸
        int             m_zorder;       ///< z深度值

        WidgetChildren  m_children; ///< 子定义子控件列表
        WidgetChildren  m_skinChildren;   ///< 皮肤子控件

        uint32          m_align;///< 相对坐标系下的排版方式

        //继承属性。会影响到子控件相应的属性。
        bool			m_visible;		///< 窗体是否可见
        bool			m_enable;		///< 是否可用，即可处理消息

        //非继承属性。不影响子控件相应的属性。
        bool            m_messagable;   ///< 是否可以处理自己的消息。如果不能，就相当于是个消息转发器
        bool            m_dragable;        ///< 是否可拖拽
        bool            m_topable;    ///< 是否点击之后排到顶层
        bool            m_drawable;    ///< 是否可绘制自己
        bool            m_childOrderable;///< 是否可改变子控件顺序
        bool            m_bOrderDirty;

    protected:
        static Widget*        m_pFocus;       ///< 当前鼠标所在位置的控件
        static Widget*        m_pSelected;    ///< 当前鼠标按下被选中的控件。
        static Widget*        m_pActived;     ///< 当前鼠标抬起后激活的控件。
    };

    Widget* loadUIFromFile(const tstring & layoutFile);


    template<typename T>
    T * Widget::createWidgetT()
    {
        return dynamic_cast<T*>(this->createWidget(T::getTypeS()));
    }

    template<typename T>
    T * Widget::getChildT(const tstring & name)
    {
        return dynamic_cast<T*>(this->getChild(name));
    }

}//namespace Lazy
