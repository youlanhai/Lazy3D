
#include "stdafx.h"
#include "UIGuiMgr.h"
#include "UIFactory.h"
#include "TypeParser.h"

#include "../Lzpy/Lzpy.h"
#include "../PyScript/LPyUIBase.h"

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

        bool operator()(const WidgetPtr & p) const
        {
            return p->getName() == m_name;
        }
    };

    bool sortCompare(const WidgetPtr & x, const WidgetPtr & y)
    {
        return x->getZOrder() < y->getZOrder();
    }


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
        , m_layer(nullptr)
        , m_parent(nullptr)
        , m_align(0)
        , m_childOrderable(true)
        , m_drawable(true)
        , m_bOrderDirty(true)
        , m_zorder(0)
        , m_children(this)
        , m_skinChildren(this)
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
        for (WidgetPtr & pChild : m_children)
        {
            processed = pChild->sendEvent(event);
            if (processed) break;
        }
        m_children.unlock();

        if (processed)
            return true;

        m_skinChildren.lock();
        m_children.lock();
        for (WidgetPtr & pChild : m_skinChildren)
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

        processed |= delegate.onEvent(event);
        processed |= ScriptEvent::onEvent(m_script, event);

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

        for (WidgetPtr & child : m_children)
        {
            child->onParentResize(newSize, oldSize);
        }

        for (WidgetPtr & child : m_skinChildren)
        {
            child->onParentResize(newSize, oldSize);
        }

        m_script.call_method_quiet("onSizeChange");
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
        m_globalPosition.set(x, y);
        m_position = m_globalPosition;
        //convert to abs position
        if (m_parent)
            m_position -= m_parent->getGlobalPosition();

        //convert to relative postion
        m_position = abs2relativePosition(m_position);

        onPositionChange();
    }

    void Widget::setAbsPosition(int x, int y)
    {
        m_position = abs2relativePosition(CPoint(x, y));
        onPositionChange();
    }

    void Widget::setPosition(int x, int y)
    {
        m_position.set(x, y);

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

        for (WidgetPtr & p : m_children)
        {
            p->onParentPositionChange();
        }

        for (WidgetPtr & p : m_skinChildren)
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

        for (WidgetPtr & p : m_children)
        {
            p->onParentPositionChange();
        }

        for (WidgetPtr & p : m_skinChildren)
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

        setGlobalPosition(m_globalPosition.x, m_globalPosition.y);
    }

    void Widget::setSkin(const tstring & skin)
    {
        if (skin == m_skin) return;

        clearSkin();

        m_skin = skin;
        createSkin();
    }

    void Widget::setLayer(const tstring & layer)
    {
        if (m_layer)
            m_layer->delChild(this);

        m_layer = getGUIMgr()->getChild(layer);

        if (m_layer)
            m_layer->addChild(this);
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
        setName(config->tag());

        clearChildren();

        for (LZDataPtr val : (*config))
        {
            if (!setProperty(config, val->tag(), val))
            {
                if (val->tag() == L"children")
                {
                    for (LZDataPtr childPtr : (*val))
                    {
                        const tstring & type = childPtr->asString();
                        Widget* child = createWidget(type);
                        if (!child)
                        {
                            LOG_ERROR(L"createChildUI '%s' failed!", type.c_str());
                            continue;
                        }
                        child->loadFromStream(childPtr);
                    }
                }
                else
                {
                    LOG_WARNING(L"Property '%s' = '%s' was not used.",
                        val->ctag(), val->cvalue());
                }
            }
        }
    }

    bool Widget::setProperty(LZDataPtr config, const tstring & key, LZDataPtr val)
    {
        if (key == L"skin")
            setSkin(val->asString());
        else if (key == L"layer")
            setLayer(val->asString());
        else if (key == L"position")
        {
            setAlign(config->readHex(L"align"));

            CPoint pt = TypeParser::parsePoint(val->asString());
            setPosition(pt.x, pt.y);
        }
        else if (key == L"size")
        {
            CPoint pt = TypeParser::parsePoint(val->asString());
            setSize(pt.x, pt.y);
        }
        else if (key == L"visible")
            setVisible(val->asBool());
        else if (key == L"enable")
            setEnable(val->asBool());
        else if (key == L"messagable")
            setMessagable(val->asBool());
        else if (key == L"dragable")
            setDragable(val->asBool());
        else if (key == L"topable")
            setTopable(val->asBool());
        else if (key == L"childOrderable")
            setChildOrderable(val->asBool());
        else if (key == L"drawable")
            setDrawable(val->asBool());
        else if (key == L"align")
        {
            //do nothing
        }
        else
            return false;

        return true;
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
        if (name.empty())
            return nullptr;

        size_t pos = name.find('/');

        CompareWidgetName cmp;
        if (pos == name.npos)
            cmp.m_name = name;
        else
            cmp.m_name = name.substr(0, pos);

        Widget* child = nullptr;
        WidgetChildren::iterator it = m_children.find_if(cmp);
        if (it != m_children.end())
            child = (*it).get();

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
            child = (*it).get();

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
        m_script.call_method_quiet("onDestroy");
        clearChildren();
        clearSkin();

        setLayer(L"");
        removeFromParent();
    }

    ///根据坐标查找控件。
    Widget* Widget::findChildByPos(const CPoint & pos, bool resculy)
    {
        for (WidgetPtr & ptr : m_children)
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
                return ptr.get();
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
        for (LZDataPtr val : (*config))
        {
            if (!setProperty(config, val->tag(), val))
            {
                if (val->tag() == L"children")
                {
                    for (LZDataPtr childPtr : (*val))
                    {
                        const tstring & type = childPtr->tag();
                        Widget* child = uiFactory()->create(type);
                        if (!child)
                        {
                            LOG_ERROR(L"createChildUI '%s' failed!", type.c_str());
                            continue;
                        }
                        child->loadFromStream(childPtr);
                        m_skinChildren.addBack(child);
                    }
                }
                else
                {
                    LOG_WARNING(L"Property '%s' = '%s' was not used.",
                        val->tag(), val->asString().c_str());
                }
            }
        }
    }

    ///创建一个子widget
    Widget * Widget::createWidget(const tstring & type)
    {
        Widget * pChild = uiFactory()->create(type);
        if(pChild) addChild(pChild);
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

    Lzpy::object Widget::createScriptSelf() const
    {
        using namespace Lzpy;
        LzpyControl *pSelf = new_instance_ex<LzpyControl>();
        pSelf->m_control = const_cast<Widget*>(this);
        return new_reference(pSelf);
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
        const tstring & type = config->asString();
        Widget* child = uiFactory()->create(type);
        if(child) child->loadFromStream(config);
        return child;
    }


}//namespace Lazy
