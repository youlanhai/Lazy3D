#pragma once

#include "../Lzui/UIWidget.h"
#include "../Lzui/UIGuiMgr.h"

namespace Lzpy
{
    using namespace Lazy;

#define LZPY_IMP_INIT_LUI(PY_CLASS)\
    LZPY_IMP_INIT(PY_CLASS)\
    {\
        PyErr_SetString(PyExc_TypeError, #PY_CLASS " can't create an instance derectly!"); \
        return false; \
    }

    object build_object(Widget * v);
    bool parse_object(Widget *& v, object o);

    object build_object(WidgetPtr v);
    bool parse_object(WidgetPtr & v, object o);


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

        LZPY_DEF_GET(parent, m_object->getParent);
        LZPY_DEF_GET(type, m_object->getType);

        LZPY_DEF_GET(size, getSize);
        LZPY_DEF_SET(size, setSize, object);

        LZPY_DEF_GET(position, getPosition);
        LZPY_DEF_SET(position, setPosition, object);

        LZPY_DEF_GET(absPosition, getAbsPosition);
        LZPY_DEF_SET(absPosition, setAbsPosition, object);

        LZPY_DEF_GET(globalPosition, getGlobalPosition);
        LZPY_DEF_SET(globalPosition, setGlobalPosition, object);

        LZPY_DEF_GET(name, m_object->getName);
        LZPY_DEF_SET(name, m_object->setName, tstring);

        LZPY_DEF_GET(skin, m_object->getSkin);
        LZPY_DEF_SET(skin, m_object->setSkin, tstring);

        LZPY_DEF_GET(zorder, m_object->getZOrder);
        LZPY_DEF_SET(zorder, m_object->setZOrder, int);

        LZPY_DEF_GET(align, m_object->getAlign);
        LZPY_DEF_SET(align, m_object->setAlign, uint32);

        LZPY_DEF_GET(visible, m_object->getVisible);
        LZPY_DEF_SET(visible, m_object->setVisible, bool);

        LZPY_DEF_GET(enable, m_object->getEnable);
        LZPY_DEF_SET(enable, m_object->setEnable, bool);

        LZPY_DEF_GET(dragable, m_object->getDragable);
        LZPY_DEF_SET(dragable, m_object->setDragable, bool);

        LZPY_DEF_GET(messagable, m_object->getMessagable);
        LZPY_DEF_SET(messagable, m_object->setMessagable, bool);

        LZPY_DEF_GET(topable, m_object->getTopable);
        LZPY_DEF_SET(topable, m_object->setTopable, bool);

        LZPY_DEF_GET(childOrderable, m_object->getChildOrderable);
        LZPY_DEF_SET(childOrderable, m_object->setChildOrderable, bool);

        LZPY_DEF_GET(drawable, m_object->getDrawable);
        LZPY_DEF_SET(drawable, m_object->setDrawable, bool);

        LZPY_DEF_GET(script, m_object->getScript);
        LZPY_DEF_SET(script, m_object->setScript, object);

        LZPY_DEF_GET(scriptName, m_object->getScriptName);
        LZPY_DEF_SET(scriptName, m_object->setScriptName, tstring);

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

        bool addChild(object_base child);
        bool delChild(object_base child);
        void clearChildren();

        PyObject *      m_pyDict;
        PyObject *      m_pyWeakreflist;
        Widget *        m_object;
    };

    BUILD_AND_PARSE_SCRIPT_OBJECT(PyWidget, Widget);

}//end namespace Lzpy

