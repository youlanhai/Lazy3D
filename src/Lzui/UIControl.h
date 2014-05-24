#pragma once

namespace Lazy
{
    /** UI控件接口
        IControl为所有控件类的基类，实现所有控件统一管理。
        所有派生类必须实现update，render,以完成绘制和更新；
        GUIMgr，管理了整个UI系统，将所有的UI关联起来，形成一颗UI树。实现了UI消息传递，
        以及统一更新和渲染。详见CGUIManager。
    */
    class LZUI_API IControl : public IBase
    {
    public:
        MAKE_UI_HEADER(IControl, uitype::Control);

        IControl(void);
        virtual ~IControl(void);

        ///释放资源。打破循环引用，释放资源。不要在构造函数中调用。
        virtual void destroy();

        ///更新
        virtual void update(float fElapse);

        ///绘制
        virtual void render(IUIRender * pDevice);

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

    public: //串行化

        ///打开配置文件
        virtual bool loadFromFile(const tstring & file);

        ///保存布局到文件
        virtual bool saveToFile(const tstring & file);

        virtual void loadFromStream(LZDataPtr root);
        virtual void saveToStream(LZDataPtr root);

        void setName(const tstring & key){ m_name = key; }
        const tstring & getName() const { return m_name; }

    public: //坐标系转换

        /** 局部坐标转换到父控件坐标系。*/
        void localToParent(CPoint & pt) const;
        void localToParent(CRect & rc) const;

        void localToGlobal(CPoint & pt) const;
        void localToGlobal(CRect & rc) const;

        void parentToLocal(CPoint & pt) const;
        void parentToLocal(CRect & rc) const;

        void globalToLocal(CPoint & pt) const;
        void globalToLocal(CRect & rc) const;

        CRect getGlobalRect() const;
        void setGlobalRect(const CRect & rc);

        ///点是否在当前区域
        bool isPointIn(int x, int y)  const;

        ///点是否在当前区域。点为父坐标系中的点。
        bool isPointInRefParent(int x, int y) const;

    public://属性

        ///设置ID。
        void setID(int id){ m_id = id; }
        int  getID(void) const { return m_id; }

        ///获取父
        PControl getParent(void) { return m_parent; }

        ///获取父
        PControl getParent(void) const { return m_parent; }

        ///设置文本
        virtual void setText(const tstring & text){ m_text = text; }
        virtual const tstring & getText(void) const { return m_text; }

        ///设置字体
        virtual void setFont(const tstring & name){ m_font = name; }
        virtual const tstring & getFont(void) const { return m_font; }

        ///设置背景图像名称
        virtual void setImage(const tstring & image){ m_image = image; }
        virtual const tstring & getImage(void) const { return m_image; }

        ///设置颜色。
        virtual void setColor(uint32 cr){ m_color = cr; }
        uint32 getColor(void) const { return m_color; }

        uint32 getBgColor() const { return m_bgColor; }
        void setBgColor(uint32 cr){ m_bgColor = cr; }

        ///设置位置
        virtual void setPosition(int x, int y);
        void setPositionX(int x);
        void setPositionY(int y);
        void setPositionZ(int z);

        const CPoint & getPosition(void) const { return m_position; }
        int getPositionX() const { return m_position.x; }
        int getPositionY() const { return m_position.y; }
        int getPositionZ() const { return m_zorder; }

        ///设置尺寸
        virtual void setSize(int w, int h);
        void setWidth(int width);
        void setHeight(int height);

        const CSize & getSize(void) const { return m_size; }
        int getWidth() const { return m_size.cx; }
        int getHeight() const { return m_size.cy; }

        //设置控件区域
        CRect getControlRect(void)  const;
        CRect getClientRect(void) const;

        ///相对坐标系
        void setRelative(bool relative);
        bool getRelative() const { return m_bRelative; }

        ///设置相对坐标。只有m_bRelative为true，此调用才有效。
        void setRelativePos(float x, float y);
        const Math::Vector2 & getRelativePos() const { return m_relativePos; }

        ///设置对齐方式
        void setRelativeAlign(DWORD align);
        DWORD getRelativeAlign() const { return m_relativeAlign; }

        ///更新相对坐标
        void applyRelative2Real();
        void applyReal2Relative();

#ifdef ENABLE_SCRIPT

        /** borrw reference */
        Lzpy::object_base getSelf(){ return m_self; }
        void setSelf(Lzpy::object_base self){ m_self = self; }

        const tstring & getScript() const { return m_script; }
        void setScript(const tstring & script){ m_script = script; }

    protected:
        /** python端脚本，ui类不持有python引用计数。*/
        Lzpy::object_base   m_self;
        tstring             m_script;

#endif

    public://开关设置

        ///设置可见
        virtual void show(bool bShow);
        virtual void toggle(void);
        bool isVisible(void) const { return m_bVisible; }

        ///设置是否可用
        void enable(bool e){ m_bEnable = e; }
        bool isEnable(void) const { return m_bEnable; }

        ///设置是否可拖拽窗体
        void enableDrag(bool e){ m_bDrag = e; }
        bool canDrag(void) const { return m_bDrag; }

        ///启用/禁用处理自身消息。如果一个ui不处理自己的消息，他就相当于是个消息转发器，将消息转发到子控件
        void enableSelfMsg(bool e){ m_bEnableSelfMsg = e; }
        bool isSelfMsgEnable(void) const{ return m_bEnableSelfMsg; }

        ///点击是否可移动到上层。
        void enableClickTop(bool e) { m_bClickTop = e; }
        bool canClickTop(void) const { return m_bClickTop; }

        ///允许改变子控件的顺序。如果启用，点击子控件，控件的顺序会被提前。
        void enableChangeChildOrder(bool can){ m_bChangeChildOrder = can; }
        bool canChangeChildOrder() const { return m_bChangeChildOrder; }

        void enableLimitInRect(bool enalbe){ m_bLimitInRect = enalbe; }
        bool canLimitInRect() const { return m_bLimitInRect; }

        void enableDrawSelf(bool enable){ m_bDrawSelf = enable; }
        bool canDrawSelf() const { return m_bDrawSelf; }

        void setEditable(bool enable){ m_bEditable = enable; }
        bool getEditable() const { return m_bEditable; }

    public: //对子控件操作

        ///将本控件从父控件表中删除
        void removeFromParent();

        ///添加子控件
        virtual void addChild(PControl pCtrl);

        ///根据id查找子控件
        virtual PControl getChild(int id);

        PControl getChild(const tstring & name);

        PControl getChildDeep(const tstring & name);

        const VisitControl & getChildren() const { return m_children; }

        ///删除子控件
        virtual void delChild(PControl pCtrl);

        ///将子空间至于最顶层。
        virtual void topmostChild(PControl ctrl);

        ///清除子控件
        void clearChildren();

        ///托管池
        void setManaged(bool managed);
        bool getManaged() const { return m_bManaged; }

        void setChildOrderDirty(){ m_bOrderDirty = true; }

        ///根据坐标查找活动的控件。pos的参考系为当前控件。
        PControl finChildByPos(const CPoint & pos, bool resculy = false);

        //遍历children

        VisitControl::iterator childBegin(){ return m_children.begin(); }
        VisitControl::iterator childEnd(){ return m_children.end(); }
        void lockChildren(){ m_children.lock(); }
        void unlockChildren(){ m_children.unlock(); }

    public://静态方法

        static bool isVKDown(DWORD vk);
        static void setFocus(PControl focus);
        static void setSeleced(PControl pSelected);
        static void setActived(PControl pActived);

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

        virtual IControl* createEditorUI(LZDataPtr config);

    protected:
        int				m_id;			///< 编号。
        tstring         m_name;         ///< 名称，跟id是相似的作用。
        PControl        m_parent;       ///< 父控件
        uint32		    m_color;	    ///< 前景颜色
        uint32          m_bgColor;      ///< 背景色
        CPoint          m_position;     ///< 位置
        CSize           m_size;         ///< 尺寸
        int             m_zorder;       ///< z深度值
        tstring         m_text;         ///<文本
        tstring		    m_font;		    ///<字体名称
        tstring         m_image;        ///<图片名称
        VisitControl    m_children;     ///<子控件列表
        Math::Vector2   m_relativePos;  ///< 相对坐标系
        uint32          m_relativeAlign;///< 相对坐标系下的排版方式
        
        bool            m_bEditable;    ///是否可用于ui编辑器。只有可编辑的控件，才可以保存到ui配置文件中。
        bool            m_bManaged;     ///< 从配置文件中加载出来的ui，需要托管
        bool			m_bVisible;		///< 窗体是否可见
        bool			m_bEnable;		///< 是否可用，即可处理消息
        bool            m_bEnableSelfMsg;   ///< 是否可以处理自己的消息。如果不能，就相当于是个消息转发器
        bool            m_bDrag;        ///< 是否可拖拽
        bool            m_bClickTop;    ///< 是否点击之后排到顶层
        bool            m_bDrawSelf;    ///< 是否可绘制自己
        bool            m_bChangeChildOrder;///< 是否可改变子控件顺序
        bool            m_bLimitInRect;     ///< 消息是否限制在窗体区域内
        bool            m_bRelative;    ///< 相对坐标系
        bool            m_bOrderDirty;

    protected:
        static PControl        m_pFocus;       ///< 当前鼠标所在位置的控件
        static PControl        m_pSelected;    ///< 当前鼠标按下被选中的控件。
        static PControl        m_pActived;     ///< 当前鼠标抬起后激活的控件。
    };

    ControlPtr loadUIFromFile(const tstring & layoutFile);

}//namespace Lazy
