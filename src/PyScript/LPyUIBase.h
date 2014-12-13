#pragma once

#include "../Lzui/UIWidget.h"
#include "../Lzui/UIGuiMgr.h"

namespace Lzpy
{
    using namespace Lazy;

    //此类及其派生类，不得出现虚函数。
    class PyWidget : public PyBase
    {
        LZPY_DEF(PyWidget);

    public:
        PyWidget();
        ~PyWidget();

        object getSize();
        void setSize(object v);

        object getPosition();
        void setPosition(object v);

        object getAbsPosition();
        void setAbsPosition(object v);

        object getGlobalPosition();
        void setGlobalPosition(object v);

        LZPY_DEF_GET(parent, getParent);
        LZPY_DEF_GET(type, m_control->getType);

        LZPY_DEF_GET(size, getSize);
        LZPY_DEF_SET(size, setSize, object);

        LZPY_DEF_GET(position, getPosition);
        LZPY_DEF_SET(position, setPosition, object);

        LZPY_DEF_GET(absPosition, getAbsPosition);
        LZPY_DEF_SET(absPosition, setAbsPosition, object);

        LZPY_DEF_GET(globalPosition, getGlobalPosition);
        LZPY_DEF_SET(globalPosition, setGlobalPosition, object);

        LZPY_DEF_GET(name, m_control->getName);
        LZPY_DEF_SET(name, m_control->setName, tstring);

        LZPY_DEF_GET(skin, m_control->getSkin);
        LZPY_DEF_SET(skin, m_control->setSkin, tstring);

        LZPY_DEF_GET(zorder, m_control->getZOrder);
        LZPY_DEF_SET(zorder, m_control->setZOrder, int);

        LZPY_DEF_GET(align, m_control->getAlign);
        LZPY_DEF_SET(align, m_control->setAlign, uint32);

        LZPY_DEF_GET(visible, m_control->getVisible);
        LZPY_DEF_SET(visible, m_control->setVisible, bool);

        LZPY_DEF_GET(enable, m_control->getEnable);
        LZPY_DEF_SET(enable, m_control->setEnable, bool);

        LZPY_DEF_GET(dragable, m_control->getDragable);
        LZPY_DEF_SET(dragable, m_control->setDragable, bool);

        LZPY_DEF_GET(messagable, m_control->getMessagable);
        LZPY_DEF_SET(messagable, m_control->setMessagable, bool);

        LZPY_DEF_GET(topable, m_control->getTopable);
        LZPY_DEF_SET(topable, m_control->setTopable, bool);

        LZPY_DEF_GET(childOrderable, m_control->getChildOrderable);
        LZPY_DEF_SET(childOrderable, m_control->setChildOrderable, bool);

        LZPY_DEF_GET(drawable, m_control->getDrawable);
        LZPY_DEF_SET(drawable, m_control->setDrawable, bool);

        LZPY_DEF_GET(script, m_control->getScript);
        LZPY_DEF_SET(script, m_control->setScript, object);

        LZPY_DEF_GET(scriptName, m_control->getScriptName);
        LZPY_DEF_SET(scriptName, m_control->setScriptName, tstring);

        LZPY_DEF_GET_MEMBER(__dict__, m_pyDict);

        tuple getGlobalRect();
        void setGlobalRect(tuple rect);
        LZPY_DEF_GET(globalRect, getGlobalRect);
        LZPY_DEF_SET(globalRect, setGlobalRect, tuple);

        //坐标转换
        LZPY_DEF_METHOD(loadFromFile);
        LZPY_DEF_METHOD(saveToFile);


        LZPY_DEF_METHOD(addChild);
        LZPY_DEF_METHOD(delChild);
        LZPY_DEF_METHOD(getChild);

        LZPY_DEF_METHOD(clearChildren);
        LZPY_DEF_METHOD(getChildren);
        LZPY_DEF_METHOD(findChildByPos);
        LZPY_DEF_METHOD(destroy);

        bool createUI(const tstring & type, PyObject *arg = nullptr);

        object_base getParent();
        bool addChild(object_base child);
        bool delChild(object_base child);
        void clearChildren();

        PyObject *      m_pyDict;
        PyObject *      m_pyWeakreflist;
        Widget *        m_control;
    };

}//end namespace Lzpy

