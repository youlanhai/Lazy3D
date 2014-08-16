
#include "stdafx.h"
#include "UIGuiMgr.h"
#include "UIFactory.h"

#ifdef ENABLE_SCRIPT
#include "../PyUI/LzpyLzd.h"
#endif

namespace Lazy
{

    struct CompareWidgetName
    {
        tstring m_name;

        CompareWidgetName()
        {}

        CompareWidgetName(const tstring & name)
            : m_name(name)
        {}

        bool operator()(const Widget * p) const
        {
            return p->getName() == m_name;
        }
    };

    bool sortCompare(Widget * x, Widget * y)
    {
        return x->getZOrder() < y->getZOrder();
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

    /*static*/ Widget* Widget::m_pFocus = nullptr;
    /*static*/ Widget* Widget::m_pSelected = nullptr;
    /*static*/ Widget* Widget::m_pActived = nullptr;

    /*static*/ bool Widget::isVKDown(uint32 vk)
    {
        return (::GetAsyncKeyState(vk) & 0x8000) != 0;
    }

    /*static*/ void Widget::setFocus(Widget* pFocus)
    {
        if (m_pFocus == pFocus)  return;

        if (m_pFocus) m_pFocus->sendUIEvent(GuiMsg::mouseLeave, 0, 0);
        m_pFocus = pFocus;
        if (m_pFocus) m_pFocus->sendUIEvent(GuiMsg::mouseEnter, 0, 0);
    }

    /*static*/ void Widget::setSeleced(Widget* pSelected)
    {
        if (m_pSelected == pSelected) return;

        if (m_pSelected) m_pSelected->sendUIEvent(GuiMsg::lostSelected, 0, 0);
        m_pSelected = pSelected;
        if (m_pSelected) m_pSelected->sendUIEvent(GuiMsg::getSelected, 0, 0);

    }

    /*static*/ void Widget::setActived(Widget* pActived)
    {
        if (m_pActived == pActived) return;

        if (m_pActived) m_pActived->sendUIEvent(GuiMsg::lostActived, 0, 0);
        m_pActived = pActived;
        if (m_pActived)  m_pActived->sendUIEvent(GuiMsg::getActived, 0, 0);
    }

    //////////////////////////////////////////////////////////////////////////
    Widget::Widget(void)
        : m_visible(true)
        , m_enable(true)
        , m_dragable(false)
        , m_messagable(true)
        , m_topable(false)
        , m_parent(nullptr)
        , m_align(RelativeAlign::center)
        , m_childOrderable(true)
        , m_drawable(true)
        , m_bOrderDirty(true)
        , m_zorder(0)
    {
    }

    Widget::~Widget(void)
    {
        if (this == m_pFocus) m_pFocus = nullptr;
        if (this == m_pSelected) m_pSelected = nullptr;
        if (this == m_pActived) m_pActived = nullptr;

        destroy();
    }

    bool Widget::checkCanHandelMsg()
    {
        return m_visible && m_enable;
    }

    ///向控件发送消息。发送成功，返回true，否则false。
    bool Widget::sendEvent(const SEvent & event)
    {
        if (!checkCanHandelMsg()) return false;

        if (sendEventToChildren(event)) return true;

        if (!m_messagable) return false;

        if (event.eventType == EET_MOUSE_EVENT)
        {
            if (!isPointIn(event.mouseEvent.x, event.mouseEvent.y))
                return false;
        }

        return onEvent(event);
    }

    bool Widget::sendUIEvent(uint32 msg, uint32 wparam, uint32 lparam)
    {
        SEvent event;
        fillGuiEvent(event, msg, wparam, lparam);
        return sendEvent(event);
    }

    bool Widget::sendEventToChildren(const SEvent & event)
    {
        if (event.eventType == EET_GUI_EVENT)
        {
            return false;//gui消息属于ui自己，不转发给子控件。
        }

        bool processed = false;

        m_children.lock();
        for (Widget *pChild : m_children)
        {
            processed = pChild->sendEvent(event);
            if (processed) break;
        }
        m_children.unlock();

        if (processed)
            return true;

        m_skinChildren.lock();
        m_children.lock();
        for (Widget *pChild : m_skinChildren)
        {
            processed = pChild->sendEvent(event);
            if (processed) break;
        }
        m_children.unlock();
        m_skinChildren.unlock();

        return processed;
    }

    bool Widget::onEvent(const SEvent & event)
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


    void Widget::setVisible(bool visible)
    {
        if (m_visible == visible)
            return;

        m_visible = visible;

        if (visible)
            active();

        onShow(visible);
    }

    void Widget::onShow(bool show)
    {
    }

    void Widget::active(void)
    {
        topmost();
        onActive();
    }

    void Widget::onActive(void)
    {

    }

    /** 将控件至于顶层*/
    void Widget::topmost()
    {
        if (getTopable() && m_parent)
        {
            m_parent->topmostChild(this);
            m_parent->topmost();
            m_parent->setChildOrderDirty();
        }
    }

    void Widget::removeFromParent()
    {
        if (m_parent == nullptr) return;

        m_parent->delChild(this);
    }


    /** 移动控件*/
    void Widget::onDrag(const CPoint & delta, const CPoint & point)
    {
        setPosition(m_position.x + delta.x, m_position.y + delta.y);
    }

    void Widget::onAlign(const CPoint & sizeDelta)
    {
        int x = m_position.x;
        int y = m_position.y;

        if (m_align & RelativeAlign::hcenter)
        {
            x += sizeDelta.x >> 1;
        }
        else if (m_align & RelativeAlign::right)
        {
            x += sizeDelta.x;
        }

        if (m_align & RelativeAlign::vcenter)
        {
            y += sizeDelta.y >> 1;
        }
        else if (m_align & RelativeAlign::bottom)
        {
            y += sizeDelta.y;
        }

        setPosition(x, y);
    }

    void Widget::onParentResize(const CPoint & newSize, const CPoint & oldSize)
    {
        onAlign(newSize - oldSize);
    }

    void Widget::onResize(const CPoint & newSize, const CPoint & oldSize)
    {
        onAlign(newSize - oldSize);

        for (Widget * child : m_children)
        {
            child->onParentResize(newSize, oldSize);
        }

        for (Widget * child : m_skinChildren)
        {
            child->onParentResize(newSize, oldSize);
        }

#ifdef ENABLE_SCRIPT
        m_self.call_method_quiet("onSizeChange");
#endif
    }

    CRect Widget::getControlRect(void) const
    {
        CPoint pt = getAbsPosition();
        return CRect(pt, pt + m_size);
    }

    CRect Widget::getClientRect(void) const
    {
        return CRect(0, 0, m_size.x, m_size.y);
    }

    CRect Widget::getGlobalRect() const
    {
        return CRect(m_globalPosition, m_globalPosition + m_size);
    }

    void Widget::setGlobalRect(const CRect & rc)
    {
        m_size.set(rc.width(), rc.height());
        m_globalPosition.set(rc.left, rc.top);
        onPositionChange();
    }

    bool Widget::isPointIn(int x, int y) const
    {
        return getGlobalRect().isIn(x, y);
    }

    //global position is the world position.
    //abs position is relative to parent.
    //relative position is relative to parent with align.

    CPoint Widget::abs2relativePosition(const CPoint & position) const
    {
        CPoint pt = position;
        if (m_parent)
        {
            //offset position by align
            if (m_align & RelativeAlign::hcenter)
                pt.x -= (m_parent->getWidth() - m_size.x) >> 1;
            else if (m_align & RelativeAlign::right)
                pt.x -= (m_parent->getWidth() - m_size.x);

            if (m_align & RelativeAlign::vcenter)
                pt.y -= (m_parent->getHeight() - m_size.y) >> 1;
            else if (m_align & RelativeAlign::bottom)
                pt.y -= (m_parent->getHeight() - m_size.y);
        }
        return pt;
    }

    CPoint Widget::relative2absPosition(const CPoint & position) const
    {
        CPoint pt = position;
        if (m_parent)
        {
            //offset position by align
            if (m_align & RelativeAlign::hcenter)
                pt.x += (m_parent->getWidth() - m_size.x) >> 1;
            else if (m_align & RelativeAlign::right)
                pt.x += (m_parent->getWidth() - m_size.x);

            if (m_align & RelativeAlign::vcenter)
                pt.y += (m_parent->getHeight() - m_size.y) >> 1;
            else if (m_align & RelativeAlign::bottom)
                pt.y += (m_parent->getHeight() - m_size.y);
        }
        return pt;
    }

    void Widget::setGlobalPosition(int x, int y)
    {
        CPoint pt(x, y);
        if (pt == m_globalPosition)
            return;

        m_globalPosition = pt;
        //convert to abs position
        if (m_parent)
            pt -= m_parent->getGlobalPosition();

        //convert to relative postion
        m_position = abs2relativePosition(pt);

        onPositionChange();
    }

    void Widget::setAbsPosition(int x, int y)
    {
        CPoint pt = abs2relativePosition(CPoint(x, y));
        if (pt == m_position)
            return;

        m_position = pt;
        onPositionChange();
    }

    void Widget::setPosition(int x, int y)
    {
        CPoint pt(x, y);
        if (m_position == pt)
            return;

        m_position = pt;

        //convert to abs position
        m_globalPosition = getAbsPosition();

        //convert to global position
        if(m_parent)
            m_globalPosition += m_parent->getGlobalPosition();

        onPositionChange();
    }

    void Widget::onPositionChange()
    {
        //position change will only effect the real position of children.

        for (Widget * p : m_children)
        {
            p->onParentPositionChange();
        }

        for (Widget * p : m_skinChildren)
        {
            p->onParentPositionChange();
        }
    }

    void Widget::onParentPositionChange()
    {
        //convert to abs position
        m_globalPosition = getAbsPosition();

        //convert to gloabl position
        if (m_parent)
            m_globalPosition += m_parent->getGlobalPosition();

        onPositionChange();
    }

    void Widget::onSizeChange()
    {
        //position change will only effect the real position of children.

        for (Widget * p : m_children)
        {
            p->onParentPositionChange();
        }

        for (Widget * p : m_skinChildren)
        {
            p->onParentPositionChange();
        }
    }

    void Widget::setZOrder(int z)
    {
        if (m_zorder == z) return;

        m_zorder = z;
        if (m_parent) m_parent->setChildOrderDirty();
    }

    void Widget::setWidth(int width)
    {
        setSize(width, m_size.y);
    }

    void Widget::setHeight(int height)
    {
        setSize(m_size.x, height);
    }

    void Widget::setAlign(uint32 align)
    {
        m_align = align;
    }

    void Widget::setSkin(const tstring & skin)
    {
        if (skin == m_skin) return;

        clearSkin();

        m_skin = skin;
        createSkin();
    }

    bool Widget::loadFromFile(const tstring & file)
    {
        LZDataPtr root = openSection(file);
        if (!root || root->countChildren() == 0)
        {
            LOG_ERROR(L"Load layout file '%s' failed!", file.c_str());
            return false;
        }

        loadFromStream(root->getChild(0));
        return true;
    }

    void Widget::loadFromStream(LZDataPtr config)
    {
        clearChildren();

        loadProperty(config);

        //加载子控件
        LZDataPtr childrenPtr = config->read(L"children");
        if (childrenPtr)
        {
            for (LZDataPtr childPtr : (*childrenPtr))
            {
                tstring type = childPtr->readString(L"type");
                Widget* child = createWidget(type);
                if (!child)
                {
                    LOG_ERROR(L"createChildUI '%s' failed!", type.c_str());
                    continue;
                }
                child->loadFromStream(childPtr);
            }
        }

#ifdef ENABLE_SCRIPT
        setScript(config->readString(L"script"));
        if(m_self) m_self.call_method_quiet("onLoadLayout", Lzpy::make_object(config));
#endif
    }

    bool Widget::saveToFile(const tstring & file)
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

    void Widget::saveToStream(LZDataPtr config)
    {
        saveProperty(config);

        if (!m_children.empty())
        {
            LZDataPtr childrenPtr = config->write(L"children");
            childrenPtr->clearChildren();

            //保存子控件的简略数据
            m_children.lock();
            for (Widget* child : m_children)
            {
                LZDataPtr ptr = childrenPtr->newChild(child->getName());
                child->saveToStream(ptr);
            }
            m_children.unlock();
        }

#ifdef ENABLE_SCRIPT
        if (!m_script.empty()) config->writeString(L"script", m_script);
        if (m_self) m_self.call_method_quiet("onSaveLayout", Lzpy::make_object(config));
#endif
    }


    void Widget::loadProperty(LZDataPtr config)
    {
        m_name = config->tag();
        setSkin(config->readString(L"skin"));

        misc::readPosition(m_position, config, L"position");
        misc::readPosition(m_size, config, L"size");

        setVisible(config->readBool(L"visible", true));
        setEnable(config->readBool(L"enable", true));
        setMessagable(config->readBool(L"messagable", true));
        setDragable(config->readBool(L"dragable", false));
        setTopable(config->readBool(L"topable", false));
        setChildOrderable(config->readBool(L"childOrderable", true));
        setDrawable(config->readBool(L"drawable", true));
    }

    void Widget::saveProperty(LZDataPtr config)
    {
        config->writeString(L"type", getType());

        misc::writePosition(m_position, config, L"position");
        misc::writePosition(m_size, config, L"size");

        if (!getSkin().empty())
            config->writeString(L"skin", getSkin());

        if (!m_visible)
            config->writeBool(L"visible", m_visible);

        if (!m_enable)
            config->writeBool(L"enable", m_enable);

        if (!m_messagable)
            config->writeBool(L"messagable", m_messagable);

        if (m_dragable)
            config->writeBool(L"dragable", m_dragable);

        if (m_topable)
            config->writeBool(L"topable", m_topable);

        if (!m_childOrderable)
            config->writeBool(L"childOrderable", m_childOrderable);

        if (!m_drawable)
            config->writeBool(L"drawable", m_drawable);

    }


    void Widget::setSize(int w, int h)
    {
        CPoint oldSize = m_size;

        m_size.set(w, h);

        onResize(m_size, oldSize);
    }


    //添加控件，获得控件
    void Widget::addChild(Widget* pChild)
    {
        assert(pChild && "Widget::addChild");

        if (pChild->m_parent != NULL)
        {
            assert(0 && "Widget::addChild, control has been added to a Panel list!");
            return;
        }

        pChild->m_parent = this;
        m_children.addFront(pChild);
        setChildOrderDirty();
        pChild->onParentPositionChange();
    }

    Widget* Widget::getChild(const tstring & name)
    {
        size_t pos = name.find('/');

        CompareWidgetName cmp;
        if (pos == name.npos)
            cmp.m_name = name;
        else
            cmp.m_name = name.substr(0, pos);

        Widget* child = nullptr;
        WidgetChildren::iterator it = m_children.find_if(cmp);
        if (it != m_children.end())
            child = *it;

        if (pos != name.npos && child)
            child = child->getChild(name.substr(pos + 1));

        return child;
    }

    Widget* Widget::getSkinChild(const tstring & name)
    {
        size_t pos = name.find('/');

        CompareWidgetName cmp;
        if (pos == name.npos)
            cmp.m_name = name;
        else
            cmp.m_name = name.substr(0, pos);

        Widget* child = nullptr;
        WidgetChildren::iterator it = m_skinChildren.find_if(cmp);
        if (it != m_skinChildren.end())
            child = *it;

        if (pos != name.npos && child)
            child = child->getChild(name.substr(pos + 1));

        return child;
    }

    void Widget::delChild(Widget* pCtrl)
    {
        assert(pCtrl && "CPanel::delChild");
        if (pCtrl->m_parent != this)
        {
            assert(0 && "CPanel::delChild, control is not child of this panel!");
            return;
        }

        pCtrl->m_parent = nullptr;
        m_children.remove(pCtrl);
        setChildOrderDirty();
    }

    ///将子空间至于最顶层。
    void Widget::topmostChild(Widget* ctrl)
    {
        if (!m_childOrderable) return;

        WidgetChildren::iterator it = m_children.find(ctrl);
        if (it == m_children.end()) return;

        m_children.remove(ctrl);
        m_children.addFront(ctrl);
    }

    ///清除子控件
    void Widget::clearChildren()
    {
        for (WidgetChildren::iterator it = m_children.begin();
                it != m_children.end(); ++it)
        {
            (*it)->m_parent = nullptr;
            (*it)->destroy();
        }
        m_children.destroy();
    }

    void Widget::destroy()
    {
#ifdef ENABLE_SCRIPT
        m_self.call_method_quiet("onDestroy");
#endif
        clearChildren();
        clearSkin();

        removeFromParent();
    }

    ///根据坐标查找控件。
    Widget* Widget::findChildByPos(const CPoint & pos, bool resculy)
    {
        for (Widget* ptr : m_children)
        {
            if (!ptr->getVisible()) continue;

            //如果子控件是panel，则先进入panel
            if (resculy)
            {
                Widget* temp = ptr->findChildByPos(pos, resculy);
                if (temp) return temp;
            }

            if (ptr->isPointIn(pos.x, pos.y))
            {
                return ptr;
            }
        }

        if (isPointIn(pos.x, pos.y)) return this;
        return NULL;
    }

    void Widget::update(float elapse)
    {
        if (m_bOrderDirty)
        {
            m_bOrderDirty = false;
            m_children.sort(sortCompare);
        }

        m_children.update(elapse);
        m_skinChildren.update(elapse);
    }

    void Widget::render(IUIRender * pDevice)
    {
        m_skinChildren.render(pDevice);
        m_children.render(pDevice);
    }

    void Widget::clearSkin()
    {
        m_skin.clear();

        for (WidgetChildren::iterator it = m_skinChildren.begin();
            it != m_skinChildren.end(); ++it)
        {
            (*it)->m_parent = nullptr;
            (*it)->destroy();
        }
        m_skinChildren.destroy();
    }

    void Widget::createSkin()
    {
        LZDataPtr root = openSection(m_skin);
        if (!root || root->countChildren() == 0)
        {
            LOG_ERROR(L"Load skin file '%s' failed!", m_skin.c_str());
            return;
        }

        LZDataPtr config = root->getChild(0);
        loadProperty(config);

        //加载子控件
        LZDataPtr childrenPtr = config->read(L"children");
        if (childrenPtr)
        {
            for (LZDataPtr childPtr : (*childrenPtr))
            {
                tstring type = childPtr->readString(L"type");
                Widget* child = uiFactory()->create(wcharToChar(type));
                if (!child)
                {
                    LOG_ERROR(L"createChildUI '%s' failed!", type.c_str());
                    continue;
                }
                child->loadFromStream(childPtr);
                m_skinChildren.addBack(child);
            }
        }
    }

    ///创建一个子widget
    Widget * Widget::createWidget(const tstring & type)
    {
        Widget * pChild = uiFactory()->create(wcharToChar(type));
        addChild(pChild);
        return pChild;
    }

    ///销毁一个子widget
    void Widget::deleteWidget(Widget *pWidget)
    {
        auto it = m_children.find(pWidget);
        if (it == m_children.end())
        {
            LOG_ERROR(L"Widget '%s' is not a child of '%s'",
                pWidget->getName().c_str(), m_name.c_str());
            return;
        }

        delete pWidget;
    }

    ////////////////////////////////////////////////////////////////////

    Widget* loadUIFromFile(const tstring & layoutFile)
    {
        if (layoutFile.empty()) return nullptr;

        LZDataPtr root = openSection(layoutFile);
        if (!root || root->countChildren() == 0)
        {
            LOG_ERROR(L"Load layout file '%s' failed!", layoutFile.c_str());
            return nullptr;
        }

        LZDataPtr config = root->getChild(0);
        const tstring & type = config->readString(L"type");
        if (type.empty())
            return nullptr;

        Widget* child = uiFactory()->create(wcharToChar(type));
        child->loadFromStream(config);
        return child;
    }


}//namespace Lazy