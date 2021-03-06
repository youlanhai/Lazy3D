﻿#pragma once

#include "../Lzui/UIWidget.h"
#include "../Lzui/UIGuiMgr.h"

#include "LPyPoint.h"

namespace Lazy
{
#define LZPY_IMP_INIT_LUI(PY_CLASS, CXX_CLASS)\
    LZPY_IMP_INIT(PY_CLASS)\
    {\
        PyErr_SetString(PyExc_TypeError, #PY_CLASS " can't create an instance derectly!"); \
        return false; \
    }\
    object CXX_CLASS::createScriptSelf()\
    {\
        PY_CLASS *pSelf = new_instance_ex<PY_CLASS>(); \
        pSelf->m_object = this;\
        return new_reference(pSelf);\
    }


    object build_object(Widget * v);
    bool parse_object(Widget *& v, object o);

    //此类及其派生类，不得出现虚函数。
    class PyWidget : public PyBase
    {
        LZPY_DEF(PyWidget);

    public:
        PyWidget();
        ~PyWidget();

        LZPY_DEF_GET(parent, m_object->getParent);
        LZPY_DEF_GET(type, m_object->getType);

        LZPY_DEF_GET(size, m_object->getSize);
        LZPY_DEF_SET(size, m_object->setSize, CPoint);

        LZPY_DEF_GET(position, m_object->getPosition);
        LZPY_DEF_SET(position, m_object->setPosition, CPoint);

        LZPY_DEF_GET(absPosition, m_object->getAbsPosition);
        LZPY_DEF_SET(absPosition, m_object->setAbsPosition, CPoint);

        LZPY_DEF_GET(globalPosition, m_object->getGlobalPosition);
        LZPY_DEF_SET(globalPosition, m_object->setGlobalPosition, CPoint);

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

        LZPY_DEF_GET(globalRect, m_object->getGlobalRect);
        LZPY_DEF_SET(globalRect, m_object->setGlobalRect, CRect);

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

        LZPY_DEF_METHOD_1(createWidget);
        LZPY_DEF_METHOD_1(deleteWidget);

        PyObject *      m_pyDict;
        PyObject *      m_pyWeakreflist;
        Widget *        m_object;
    };

    BUILD_AND_PARSE_SCRIPT_OBJECT(PyWidget, Widget);

}//end namespace Lazy

