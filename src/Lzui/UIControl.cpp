
#include "stdafx.h"
#include "UIGuiMgr.h"
#include "UIFactory.h"

#ifdef ENABLE_SCRIPT
#include "../PyUI/LzpyLzd.h"
#endif

namespace Lazy
{

    struct PredIfIDEqual
    {
        PredIfIDEqual(int id) : m_id(id) { }
        bool operator()(PControl pc)
        {
            return pc->getID() == m_id;
        }
    private:
        int m_id;
    };

    struct PredIfID2Equal
    {
        PredIfID2Equal(int id) : m_id(id) { }
        bool operator()(RefPtr<IControl> pc)
        {
            return pc->getID() == m_id;
        }
    private:
        int m_id;
    };

    bool sortCompare(IControl * x, IControl * y)
    {
        return x->getPositionZ() < y->getPositionZ();
    }

    static EditorUICreateFun g_pUICreateFun = nullptr;
    void setEditorUICreateFun(EditorUICreateFun fun)
    {
        g_pUICreateFun = fun;
    }

#ifdef ENABLE_SCRIPT
    class ScriptEvent
    {
    public:
        static bool onEvent(Lzpy::object pyScript, const SEvent & event)
        {
            if (!pyScript) return false;

            switch (event.eventType)
            {
            case EET_GUI_EVENT:
                return onGuiEvent(pyScript, event.guiEvent);
            case EET_KEY_EVENT:
                return onKeyEvent(pyScript, event.keyEvent);
            case EET_CHAR_EVENT:
                return onCharEvent(pyScript, event.charEvent);
            case EET_MOUSE_EVENT:
                return onMouseEvent(pyScript, event.mouseEvent);
            case EET_SYS_EVENT:
                return onSysEvent(pyScript, event.sysEvent);
            default:
                break;
            }

            return false;
        }

    private:

        static bool onGuiEvent(Lzpy::object pyScript, const SEvent::SGuiEvent & event)
        {
            if (!pyScript.hasattr("onGuiEvent")) return false;

            return pyScript.call_method("onGuiEvent", event.message, event.wparam, event.lparam);

        }

        static bool onKeyEvent(Lzpy::object pyScript, const SEvent::SKeyEvent & event)
        {
            if (!pyScript.hasattr("onKeyEvent")) return false;

            return pyScript.call_method("onKeyEvent", event.down, (int) event.key);
        }

        static bool onCharEvent(Lzpy::object pyScript, const SEvent::SCharEvent & event)
        {
            if (!pyScript.hasattr("onCharEvent")) return false;

            return pyScript.call_method("onCharEvent", (int) event.ch);
        }

        static bool onMouseEvent(Lzpy::object pyScript, const SEvent::SMouseEvent & event)
        {
            if (event.event == EME_MOUSE_WHEEL)
                return onMouseWheel(pyScript, event);

            if (!pyScript.hasattr("onMouseEvent")) return false;
            return pyScript.call_method("onMouseEvent", (int) event.event, event.x, event.y);
        }

        static bool onMouseWheel(Lzpy::object pyScript, const SEvent::SMouseEvent & event)
        {
            if (!pyScript.hasattr("onMouseWheel")) return false;
            return pyScript.call_method("onMouseWheel", event.x, event.y, event.wheel);
        }

        static bool onSysEvent(Lzpy::object pyScript, const SEvent::SSysEvent & event)
        {
            if (!pyScript.hasattr("onSysEvent")) return false;

            return pyScript.call_method("onSysEvent", event.message, event.wparam, event.lparam);
        }

    };

#endif

    //////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////

    /*static*/ PControl IControl::m_pFocus = nullptr;
    /*static*/ PControl IControl::m_pSelected = nullptr;
    /*static*/ PControl IControl::m_pActived = nullptr;

    /*static*/ bool IControl::isVKDown(DWORD vk)
    {
        return (::GetAsyncKeyState(vk) & 0x8000) != 0;
    }

    /*static*/ void IControl::setFocus(PControl pFocus)
    {
        if (m_pFocus == pFocus)  return;

        if (m_pFocus) m_pFocus->sendUIEvent(GuiMsg::mouseLeave, 0, 0);
        m_pFocus = pFocus;
        if (m_pFocus) m_pFocus->sendUIEvent(GuiMsg::mouseEnter, 0, 0);
    }

    /*static*/ void IControl::setSeleced(PControl pSelected)
    {
        if (m_pSelected == pSelected) return;

        if (m_pSelected) m_pSelected->sendUIEvent(GuiMsg::lostSelected, 0, 0);
        m_pSelected = pSelected;
        if (m_pSelected) m_pSelected->sendUIEvent(GuiMsg::getSelected, 0, 0);

    }

    /*static*/ void IControl::setActived(PControl pActived)
    {
        if (m_pActived == pActived) return;

        if (m_pActived) m_pActived->sendUIEvent(GuiMsg::lostActived, 0, 0);
        m_pActived = pActived;
        if (m_pActived)  m_pActived->sendUIEvent(GuiMsg::getActived, 0, 0);
    }

    //////////////////////////////////////////////////////////////////////////
    IControl::IControl(void)
        : m_id(0)
        , m_color(0xffffffff)
        , m_bgColor(0x7fffffff)
        , m_bVisible(true)
        , m_bEnable(true)
        , m_bDrag(false)
        , m_bEnableSelfMsg(true)
        , m_bClickTop(false)
        , m_parent(nullptr)
        , m_font(getDefaultFont())
        , m_bRelative(false)
        , m_relativePos(0, 0)
        , m_relativeAlign(RelativeAlign::center)
        , m_bChangeChildOrder(true)
        , m_bLimitInRect(false)
        , m_bDrawSelf(true)
        , m_name(L"noname")
        , m_bEditable(false)
        , m_bManaged(false)
        , m_bOrderDirty(true)
        , m_zorder(0)
    {
    }

    IControl::~IControl(void)
    {
        removeFromParent();

        if (this == m_pFocus) m_pFocus = nullptr;
        if (this == m_pSelected) m_pSelected = nullptr;
        if (this == m_pActived) m_pActived = nullptr;

        clearChildren();

    }

    bool IControl::checkCanHandelMsg()
    {
        return m_bVisible && m_bEnable;
    }

    ///向控件发送消息。发送成功，返回true，否则false。
    bool IControl::sendEvent(const SEvent & event)
    {
        if (!checkCanHandelMsg()) return false;
        
        if (sendEventToChildren(event)) return true;

        if (!m_bEnableSelfMsg) return false;

        if (event.eventType == EET_MOUSE_EVENT)
        {
            if (!isPointIn(event.mouseEvent.x, event.mouseEvent.y))
                return false;
        }
        return onEvent(event);
    }

    bool IControl::sendUIEvent(uint32 msg, uint32 wparam, uint32 lparam)
    {
        SEvent event;
        fillGuiEvent(event, msg, wparam, lparam);
        return sendEvent(event);
    }

    bool IControl::sendEventToChildren(const SEvent & event)
    {
        if (event.eventType == EET_GUI_EVENT)
        {
            return false;//gui消息属于ui自己，不转发给子控件。
        }
        else if (event.eventType == EET_MOUSE_EVENT)
        {
            if (m_bLimitInRect && !isPointIn(event.mouseEvent.x, event.mouseEvent.y))
                return false;
        }

        bool processed = false;
        m_children.lock();

        for (VisitControl::iterator it = m_children.begin();
            it != m_children.end(); ++it)
        {
            IControl *pChild = *it;

            if (event.isMouseEvent())
            {
                //坐标系转换
                SEvent subEvent = event;
                subEvent.mouseEvent.x -= pChild->getPosition().x;
                subEvent.mouseEvent.y -= pChild->getPosition().y;
                processed = pChild->sendEvent(subEvent);
            }
            else
            {
                processed = pChild->sendEvent(event);
            }

            if (processed) break;
        }
        m_children.unlock();

        return processed;
    }

    bool IControl::onEvent(const SEvent & event)
    {
        bool processed = false;
        if (event.eventType == EET_MOUSE_EVENT)
        {
            processed = true; //鼠标消息将不在继续向后传递。故返回true。

            if (event.mouseEvent.event == EME_MOUSE_MOVE)
            {
                setFocus(this);
            }
            else if (event.mouseEvent.event == EME_LMOUSE_DOWN)
            {
                setSeleced(this);
                topmost();
            }
            else if (event.mouseEvent.event == EME_LMOUSE_UP)
            {
                setActived(this);
                setSeleced(NULL);
            }
            else if (event.mouseEvent.event == EME_MOUSE_WHEEL)
            {
                processed = false;
            }
        }
        else if (event.eventType == EET_GUI_EVENT)
        {
            if (event.guiEvent.message == GuiMsg::mouseLeftDrag)
            {
                onDrag(CPoint(event.guiEvent.wparam), CPoint(event.guiEvent.lparam));
                processed = true;
            }
        }

#ifdef ENABLE_SCRIPT
        if (ScriptEvent::onEvent(m_self, event))
            processed = true;
#endif
        return processed;
    }


    void IControl::show(bool bShow)
    {
        m_bVisible = bShow;

        if (bShow) active();
        onShow(bShow);
    }

    void IControl::toggle(void)
    {
        show(!isVisible());
    }

    void IControl::onShow(bool show)
    {
    }

    void IControl::active(void)
    {
        topmost();
        onActive();
    }

    void IControl::onActive(void)
    {

    }

    /** 将控件至于顶层*/
    void IControl::topmost()
    {
        if (canClickTop() && m_parent)
        {
            m_parent->topmostChild(this);
            m_parent->topmost();
            m_parent->setChildOrderDirty();
        }
    }

    void IControl::removeFromParent()
    {
        if (m_parent == nullptr) return;

        m_parent->delChild(this);
    }


    /** 移动控件*/
    void IControl::onDrag(const CPoint & delta, const CPoint & point)
    {
        setPosition(m_position.x + delta.x, m_position.y + delta.y);
    }

    CRect IControl::getControlRect(void) const
    {
        return CRect(m_position, m_size);
    }

    CRect IControl::getClientRect(void) const
    {
        return CRect(0, 0, m_size.cx, m_size.cy);
    }

    //坐标系转换
    void IControl::localToParent(CPoint & pt) const
    {
        pt += m_position;
    }

    void IControl::localToParent(CRect & rc) const
    {
        rc.offset(m_position.x, m_position.y);
    }

    void IControl::localToGlobal(CPoint & pt) const
    {
        localToParent(pt);
        if (m_parent) m_parent->localToGlobal(pt);
    }

    void IControl::localToGlobal(CRect & rc) const
    {
        localToParent(rc);
        if (m_parent) m_parent->localToGlobal(rc);
    }


    void IControl::parentToLocal(CPoint & pt) const
    {
        pt -= m_position;
    }

    void IControl::parentToLocal(CRect & rc) const
    {
        rc.offset(-m_position.x, -m_position.y);
    }

    void IControl::globalToLocal(CPoint & pt) const
    {
        if (m_parent)  m_parent->globalToLocal(pt);

        parentToLocal(pt);
    }

    void IControl::globalToLocal(CRect & rc) const
    {
        if (m_parent) m_parent->globalToLocal(rc);

        parentToLocal(rc);
    }

    CRect IControl::getGlobalRect() const
    {
        CRect rc = getClientRect();
        localToGlobal(rc);
        return rc;
    }

    void IControl::setGlobalRect(const CRect & r)
    {
        CRect rc = r;
        if (m_parent) m_parent->globalToLocal(rc);
        
        setPosition(rc.left, rc.top);
        setSize(rc.width(), rc.height());
    }

    bool IControl::isPointIn(int x, int y) const
    {
        if (x < 0 || y < 0) return false;
        if (x > m_size.cx || y > m_size.cy) return false;

        return true;
    }

    bool IControl::isPointInRefParent(int x, int y) const
    {
        if (x < m_position.x || y < m_position.y) return false;
        if (x > m_position.x + m_size.cx) return false;
        if (y > m_position.y + m_size.cy) return false;

        return true;
    }

    void IControl::setPosition(int x, int y)
    {
        m_position.set(x, y);

        if (m_bRelative) applyReal2Relative();
    }

    void IControl::setPositionX(int x)
    {
        setPosition(x, m_position.y);
    }

    void IControl::setPositionY(int y)
    {
        setPosition(m_position.x, y);
    }

    void IControl::setPositionZ(int z)
    {
        m_zorder = z;
        if (m_parent) m_parent->setChildOrderDirty();
    }

    void IControl::setWidth(int width)
    {
        setSize(width, m_size.cy);
    }

    void IControl::setHeight(int height)
    {
        setSize(m_size.cx, height);
    }

    ///相对坐标系
    void IControl::setRelative(bool relative)
    {
        m_bRelative = relative;

        if (m_bRelative) applyRelative2Real();
    }

    ///设置相对坐标。只有m_bRelative为true，此调用才有效。
    void IControl::setRelativePos(float x, float y)
    {
        m_relativePos.set(x, y);

        if (m_bRelative) applyRelative2Real();
    }

    void IControl::setRelativeAlign(DWORD align)
    {
        m_relativeAlign = align;

        if (m_bRelative) applyRelative2Real();
    }

    void IControl::applyRelative2Real()
    {
        if (nullptr == m_parent) return;

        const CSize & size = m_parent->getSize();
        m_position.x = LONG(size.cx * m_relativePos.x);
        m_position.y = LONG(size.cy * m_relativePos.y);

        //水平方向
        if (m_relativeAlign & RelativeAlign::hcenter)
        {
            m_position.x -= m_size.cx >> 1;
        }
        else if (m_relativeAlign & RelativeAlign::right)
        {
            m_position.x -= m_size.cx;
        }

        //垂直方向
        if (m_relativeAlign & RelativeAlign::vcenter)
        {
            m_position.y -= m_size.cy >> 1;
        }
        else if (m_relativeAlign & RelativeAlign::bottom)
        {
            m_position.y -= m_size.cy;
        }
    }

    void IControl::applyReal2Relative()
    {
        if (nullptr == m_parent) return;

        CPoint pt = m_position;

        //反向矫正坐标

        //水平方向
        if (m_relativeAlign & RelativeAlign::hcenter)
        {
            pt.x += m_size.cx >> 1;
        }
        else if (m_relativeAlign & RelativeAlign::right)
        {
            pt.x += m_size.cx;
        }

        //垂直方向
        if (m_relativeAlign & RelativeAlign::vcenter)
        {
            pt.y += m_size.cy >> 1;
        }
        else if (m_relativeAlign & RelativeAlign::bottom)
        {
            pt.y += m_size.cy;
        }

        const CSize & size = m_parent->getSize();

        if (size.cx > 0)
        {
            m_relativePos.x = (float) pt.x / (float) size.cx;
        }
        else
        {
            m_relativePos.x = 0.0f;
        }

        if (size.cy > 0)
        {
            m_relativePos.y = (float) pt.y / (float) size.cy;
        }
        else
        {
            m_relativePos.y = 0.0f;
        }


    }

    bool IControl::loadFromFile(const tstring & file)
    {
        setEditable(false);

        LZDataPtr root = openSection(file);
        if (!root || root->countChildren() == 0)
        {
            LOG_ERROR(L"Load layout file '%s' failed!", file.c_str());
            return false;
        }

        loadFromStream(root->getChild(0));
        return true;
    }

    void IControl::loadFromStream(LZDataPtr config)
    {
        assert(config);

        m_name = config->tag();
        setID(config->readInt(L"id"));
        setText(config->readString(L"text"));
        setImage(config->readString(L"image"));
        setFont(config->readString(L"font", getDefaultFont()));
        setColor(config->readHex(L"color", 0xffff0000));
        setBgColor(config->readHex(L"bgcolor", 0xffffffff));
        misc::readPosition(m_position, config, L"position");
        misc::readSize(m_size, config, L"size");

        show(config->readBool(L"visible", true));
        enable(config->readBool(L"enable", true));
        enableSelfMsg(config->readBool(L"enableSelfMsg", true));
        enableDrag(config->readBool(L"enalbeDrag", false));
        enableClickTop(config->readBool(L"clickTop", false));
        enableChangeChildOrder(config->readBool(L"limitInRect", true));
        enableChangeChildOrder(config->readBool(L"changeChildOrder", true));
        enableDrawSelf(config->readBool(L"drawSelf", true));

        m_bRelative = config->readBool(L"relative", false);
        if (m_bRelative)
        {
            misc::readVector2(m_relativePos, config, L"relativePos");
            setRelativeAlign(config->readInt(L"relativeAlign"));
        }

        clearChildren();

        //加载子控件
        LZDataPtr childrenPtr = config->read(L"children");
        if (childrenPtr)
        {
            for (LZDataPtr childPtr : (*childrenPtr))
            {
                int type = childPtr->readInt(L"type");

                PControl child = createEditorUI(childPtr);
                if (!child)
                {
                    LOG_ERROR(L"createChildUI '%d' failed!", type);
                    continue;
                }

                child->loadFromStream(childPtr);

                child->setName(childPtr->tag());
                child->setEditable(true);
                addChild(child);
            }
        }

#ifdef ENABLE_SCRIPT
        setScript(config->readString(L"script"));
        if(m_self) m_self.call_method_quiet("onLoadLayout", Lzpy::make_object(config));
#endif
    }


    bool IControl::saveToFile(const tstring & file)
    {
        LZDataPtr root = openSection(file, true);
        if (!root)
        {
            LOG_ERROR(L"Open layout file '%s' failed!", file.c_str());
            return false;
        }

        root->clearChildren();

        LZDataPtr config = root->newOne(m_name, EmptyStr);
        root->addChild(config);

        saveToStream(config);
        return saveSection(root, file);
    }

    void IControl::saveToStream(LZDataPtr config)
    {
        assert(config);

        config->writeInt(L"type", getType());
        config->writeInt(L"id", getID());

        if (!getText().empty())
            config->writeString(L"text", getText());

        if (!getImage().empty())
            config->writeString(L"image", getImage());

        if (!getFont().empty())
            config->writeString(L"font", getFont());

        config->writeHex(L"color", m_color);
        config->writeHex(L"bgcolor", m_bgColor);
        config->writeBool(L"visible", m_bVisible);
        config->writeBool(L"enable", m_bEnable);
        config->writeBool(L"enableSelfMsg", m_bEnableSelfMsg);
        config->writeBool(L"enalbeDrag", m_bDrag);
        config->writeBool(L"clickTop", m_bClickTop);
        config->writeBool(L"limitInRect", m_bLimitInRect);
        config->writeBool(L"changeChildOrder", m_bChangeChildOrder);
        config->writeBool(L"drawSelf", m_bDrawSelf);

        misc::writePosition(m_position, config, L"position");
        misc::writeSize(m_size, config, L"size");

        if (m_bRelative)
        {
            config->writeBool(L"relative", m_bRelative);
            config->writeInt(L"relativeAlign", m_relativeAlign);
            misc::writeVector2(m_relativePos, config, L"relativePos");
        }


        LZDataPtr childrenPtr = config->write(L"children");
        childrenPtr->clearChildren();

        //保存子控件的简略数据
        lockChildren();
        for (PControl child : m_children)
        {
            if (!child->getEditable()) continue;

            LZDataPtr ptr = childrenPtr->newOne(child->getName(), EmptyStr);
            childrenPtr->addChild(ptr);
            child->saveToStream(ptr);
        }
        unlockChildren();

#ifdef ENABLE_SCRIPT
        if (!m_script.empty()) config->writeString(L"script", m_script);
        if (m_self) m_self.call_method_quiet("onSaveLayout", Lzpy::make_object(config));
#endif
    }

    void IControl::setSize(int w, int h)
    {
        m_size.set(w, h);

        if (!m_children.empty())
        {
            for (IControl * child : m_children)
            {
                if (child->getRelative()) child->applyRelative2Real();
            }
        }

#ifdef ENABLE_SCRIPT
        m_self.call_method_quiet("onSizeChange");
#endif
    }


    //添加控件，获得控件
    void IControl::addChild(PControl pCtrl)
    {
        assert(pCtrl && "IControl::addChild");

        if (pCtrl->m_parent != NULL)
            throw(std::logic_error("IControl::addChild, control has been added to a Panel list!"));

        pCtrl->m_parent = this;
        m_children.addFront(pCtrl);
        setChildOrderDirty();
    }

    PControl IControl::getChild(int id)
    {
        VisitControl::iterator it = m_children.find_if(PredIfIDEqual(id));
        if (it != m_children.end())
        {
            return *it;
        }
        return NULL;
    }

    PControl IControl::getChild(const tstring & name)
    {
        auto fun = [name](PControl p){ return p->getName() == name; };
        VisitControl::iterator it = m_children.find_if(fun);
        if (it != m_children.end()) return *it;
        
        return NULL;
    }

    PControl IControl::getChildDeep(const tstring & name)
    {
        size_t pos = name.find('/');
        if (pos == name.npos) return getChild(name);

        PControl child = getChild(name.substr(0, pos));
        if (!child) return nullptr;

        return child->getChildDeep(name.substr(pos + 1));
    }

    void IControl::delChild(PControl pCtrl)
    {
        assert(pCtrl && "CPanel::delChild");
        if (pCtrl->m_parent != this)
            throw(std::logic_error("CPanel::delChild, control is not child of this panel!"));

        pCtrl->m_parent = nullptr;
        m_children.remove(pCtrl);
        setChildOrderDirty();
    }

    ///将子空间至于最顶层。
    void IControl::topmostChild(PControl ctrl)
    {
        if (!m_bChangeChildOrder) return;

        assert(ctrl);

        VisitControl::iterator it = m_children.find(ctrl);
        if (it == m_children.end()) return;

        m_children.remove(ctrl);
        m_children.addFront(ctrl);
    }

    ///清除子控件
    void IControl::clearChildren()
    {
        //加锁，防止子控件析构时删除自己。
        for (VisitControl::iterator it = m_children.begin();
            it != m_children.end(); ++it)
        {
            (*it)->m_parent = nullptr;
            (*it)->setManaged(false);
        }
        m_children.destroy();
    }

    void IControl::destroy()
    {
        for (VisitControl::iterator it = m_children.begin();
            it != m_children.end(); ++it)
        {
            (*it)->m_parent = nullptr;
            (*it)->destroy();
        }

#ifdef ENABLE_SCRIPT
        m_self.call_method_quiet("onDestroy");
#endif

        clearChildren();
        removeFromParent();
    }

    ///根据坐标查找控件。
    PControl IControl::finChildByPos(const CPoint & pos, bool resculy)
    {
        for (VisitControl::iterator it = m_children.begin();
            it != m_children.end();  ++it)
        {
            PControl ptr = *it;
            if (!ptr || !ptr->isVisible()) continue;

            CPoint pt = pos;
            ptr->parentToLocal(pt);

            //如果子控件是panel，则先进入panel
            if (resculy)
            {
                PControl temp = ptr->finChildByPos(pt, resculy);
                if (temp) return temp;
            }

            if (ptr->isPointIn(pt.x, pt.y))
            {
                return ptr;
            }
        }

        if (isPointIn(pos.x, pos.y)) return this;
        return NULL;
    }

    IControl* IControl::createEditorUI(LZDataPtr config)
    {
        if (g_pUICreateFun)
        {
            return g_pUICreateFun(this, config);
        }

        int type = config->readInt(L"type", -1);
        if (type < 0) return nullptr;

        IControl *p = uiFactory()->create(type);
        if (p) setManaged(true);

        return p;
    }

    void IControl::update(float elapse)
    {
        if (m_bOrderDirty)
        {
            m_bOrderDirty = false;
            m_children.sort(sortCompare);
        }

        m_children.update(elapse);
    }

    void IControl::render(IUIRender * pDevice)
    {
        m_children.render(pDevice);
    }

    void IControl::setManaged(bool managed)
    {
        if (m_bManaged == managed) return;

        if (managed)
            this->addRef();
        else
            this->delRef();

        m_bManaged = managed;
    }

    ////////////////////////////////////////////////////////////////////

    ControlPtr loadUIFromFile(const tstring & layoutFile)
    {
        if (layoutFile.empty()) return nullptr;

        LZDataPtr root = openSection(layoutFile);
        if (!root || root->countChildren() == 0)
        {
            LOG_ERROR(L"Load layout file '%s' failed!", layoutFile.c_str());
            return nullptr;
        }

        LZDataPtr config = root->getChild(0);
        int type = config->readInt(L"type");

        ControlPtr child = uiFactory()->create(type);
        child->loadFromStream(config);
        return child;
    }


}//namespace Lazy