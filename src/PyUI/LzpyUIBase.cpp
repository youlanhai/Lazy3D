#include "stdafx.h"
#include "LzpyUIBase.h"
#include "LzpyLzd.h"

#ifndef ENABLE_SCRIPT
#error the flag ENABLE_SCRIPT doen't defined
#endif

namespace Lzpy
{
    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(LzpyControl)
    LZPY_GET(parent);
    LZPY_GET(type);
    LZPY_GET(__dict__);
    LZPY_GETSET(managed);
    LZPY_GETSET(id);
    LZPY_GETSET(size);
    LZPY_GETSET(position);
    LZPY_GETSET(zorder);
    LZPY_GETSET(name);
    LZPY_GETSET(text);
    LZPY_GETSET(font);
    LZPY_GETSET(image);
    LZPY_GETSET(color);
    LZPY_GETSET(bgColor);
    LZPY_GETSET(relative);
    LZPY_GETSET(relativePos);
    LZPY_GETSET(relativeAlign);
    LZPY_GETSET(visible);
    LZPY_GETSET(enable);
    LZPY_GETSET(enableDrag);
    LZPY_GETSET(enableSelfMsg);
    LZPY_GETSET(enableLimitInRect);
    LZPY_GETSET(enableClickTop);
    LZPY_GETSET(enableChangeChildOrder);
    LZPY_GETSET(editable);
    LZPY_GETSET(script);

    LZPY_GETSET(globalRect);

    LZPY_METHOD(addChild);
    LZPY_METHOD(getChild);
    LZPY_METHOD(delChild);
    LZPY_METHOD(getChildByName);

    LZPY_METHOD(clearChildren);
    LZPY_METHOD(getChildren);
    LZPY_METHOD(findChildByPos);
    LZPY_METHOD(destroy);

    LZPY_METHOD(localToParent);
    LZPY_METHOD(parentToLocal);

    LZPY_METHOD(localToGlobal);
    LZPY_METHOD(globalToLocal);

    LZPY_METHOD(loadFromFile);
    LZPY_METHOD(saveToFile);
    LZPY_LINK_ATTR(tp_weaklistoffset, offsetof(LzpyControl, m_pyWeakreflist));
    LZPY_LINK_ATTR(tp_dictoffset, offsetof(LzpyControl, m_pyDict));

    LZPY_CLASS_END();
    LZPY_IMP_INIT_LUI(LzpyControl, Control);

    LzpyControl::LzpyControl()
        : m_pyDict(nullptr)
        , m_pyWeakreflist(nullptr)
        , m_managed(false)
    {
        m_pyDict = PyDict_New();
    }

    LzpyControl::~LzpyControl()
    {
#ifdef _DEBUG
        {
            PyObject * pStr = PyObject_Repr(this);

            wchar_t *p = PyUnicode_AsUnicodeAndSize(pStr, NULL);
            LOG_DEBUG(L"%s deleted.", p);
            Py_DECREF(pStr);
        }
#endif

        if (m_control)
        {
            clearChildren();

            m_control->removeFromParent();
            m_control->setSelf(null_object);
            m_control = nullptr;
        }

        if (m_pyWeakreflist != NULL)
            PyObject_ClearWeakRefs(this);

        Py_DECREF(m_pyDict);
    }

    void LzpyControl::setManaged(bool managed)
    {
        if (m_managed == managed) return;

        m_managed = managed;
        if (managed)
            Py_IncRef(this);
        else
            Py_DecRef(this);
    }

    void LzpyControl::clearChildren()
    {
        list children = py_getChildren(null_object);

        size_t n = children.size();
        for (size_t i = 0; i < n; ++i)
        {
            children[i].cast<LzpyControl>()->setManaged(false);
        }

        m_control->clearChildren();
    }

    bool LzpyControl::addChild(object_base child)
    {
        if (!CHECK_INSTANCE(LzpyControl, child.get())) return false;

        LzpyControl *p = child.cast<LzpyControl>();
        m_control->addChild(p->m_control.get());

        return true;
    }

    bool LzpyControl::delChild(object_base child)
    {
        if (!CHECK_INSTANCE(LzpyControl, child.get())) return false;

        LzpyControl *p = child.cast<LzpyControl>();
        m_control->delChild(p->m_control.get());

        return true;
    }

    bool LzpyControl::createUI(int type, PyObject *arg)
    {
        LzpyControl * parent = nullptr;
        if (!PyArg_ParseTuple(arg, "|O", &parent)) return false;

        if (parent == Py_None)
            parent = nullptr;

        if (parent && !CHECK_INSTANCE(LzpyControl, parent))
        {
            return false;
        }

        m_control = uiFactory()->create(type);
        if (m_control)
        {
            m_control->setSelf(object_base(this));

            if (parent) parent->addChild(object(this));
        }

        return bool(m_control);
    }


    object_base LzpyControl::getParent()
    {
        if (m_control && m_control->getParent())
        {
            ControlPtr parent = m_control->getParent();
            return parent->getSelf();
        }

        return object_base();
    }

    object LzpyControl::getSize()
    {
        const CSize & pt = m_control->getSize();
        return build_tuple(pt.cx, pt.cy);
    }

    void LzpyControl::setSize(object v)
    {
        tuple arg(v);
        if (!arg.check() || arg.size() != 2)
        {
            PyErr_SetString(PyExc_TypeError, "tuple(w, h) needed!");
            throw(python_error("TypeError"));
        }

        CSize size;
        arg.parse_tuple(&size.cx, &size.cy);
        m_control->setSize(size.cx, size.cy);
    }

    object LzpyControl::getPosition()
    {
        const CPoint & pt = m_control->getPosition();
        return build_tuple(pt.x, pt.y);
    }

    void LzpyControl::setPosition(object v)
    {
        tuple arg(v);
        if (!arg.check() || arg.size() != 2)
        {
            PyErr_SetString(PyExc_TypeError, "tuple(x, y) needed!");
            throw(python_error("TypeError"));
        }

        CPoint pt;
        arg.parse_tuple(&pt.x, &pt.y);
        m_control->setPosition(pt.x, pt.y);
    }

    object LzpyControl::getRelativePos()
    {
        const Vector2 & pos = m_control->getRelativePos();
        return build_tuple(pos.x, pos.y);
    }

    void LzpyControl::setRelativePos(object v)
    {
        tuple arg(v);
        if (!arg.check() || arg.size() != 2)
        {
            PyErr_SetString(PyExc_TypeError, "tuple(x, y) needed!");
            throw(python_error("TypeError"));
        }

        float x, y;
        arg.parse_tuple(&x, &y);
        m_control->setRelativePos(x, y);
    }

    LZPY_IMP_METHOD(LzpyControl, localToParent)
    {
        CPoint pt;
        if (!arg.parse_tuple(&pt.x, &pt.y)) return null_object;

        m_control->localToParent(pt);
        return build_tuple(pt.x, pt.y);
    }
    LZPY_IMP_METHOD(LzpyControl, parentToLocal)
    {
        CPoint pt;
        if (!arg.parse_tuple(&pt.x, &pt.y)) return null_object;

        m_control->parentToLocal(pt);
        return build_tuple(pt.x, pt.y);
    }

    LZPY_IMP_METHOD(LzpyControl, localToGlobal)
    {
        CPoint pt;
        if (!arg.parse_tuple(&pt.x, &pt.y)) return null_object;

        m_control->localToGlobal(pt);
        return build_tuple(pt.x, pt.y);
    }
    LZPY_IMP_METHOD(LzpyControl, globalToLocal)
    {
        CPoint pt;
        if (!arg.parse_tuple(&pt.x, &pt.y)) return null_object;

        m_control->globalToLocal(pt);
        return build_tuple(pt.x, pt.y);
    }

    tuple LzpyControl::getGlobalRect()
    {
        CRect rc = m_control->getGlobalRect();
        return build_tuple(rc.left, rc.top, rc.right, rc.bottom);
    }

    void LzpyControl::setGlobalRect(tuple rect)
    {
        CRect rc;
        if (!rect.parse_tuple(&rc.left, &rc.top, &rc.right, &rc.bottom)) return;

        m_control->setGlobalRect(rc);
    }

    LZPY_IMP_METHOD(LzpyControl, loadFromFile)
    {
        std::wstring fname;
        if (!arg.parse_tuple(&fname)) return null_object;

        bool ret = m_control->loadFromFile(fname);
        return build_object(ret);
    }

    LZPY_IMP_METHOD(LzpyControl, saveToFile)
    {
        std::wstring fname;
        if (!arg.parse_tuple(&fname)) return null_object;

        bool ret = m_control->saveToFile(fname);
        return build_object(ret);
    }


    LZPY_IMP_METHOD(LzpyControl, addChild)
    {
        object_base child;
        if (!arg.parse_tuple(&child))
            return null_object;

        if (!addChild(child))
            return null_object;

        return none_object;
    }

    LZPY_IMP_METHOD(LzpyControl, getChild)
    {
        int id;
        if (!arg.parse_tuple(&id)) return null_object;

        PControl child = m_control->getChild(id);
        if (child && child->getSelf())
            return child->getSelf();

        return none_object;
    }

    LZPY_IMP_METHOD(LzpyControl, getChildByName)
    {
        tstring name;
        if (!arg.parse_tuple(&name)) return null_object;

        PControl child = m_control->getChildDeep(name);
        if(child && child->getSelf())
            return child->getSelf();

        return none_object;
    }

    LZPY_IMP_METHOD(LzpyControl, delChild)
    {
        object_base child;
        if (!arg.parse_tuple(&child))
            return null_object;

        if (!delChild(child))
            return null_object;

        return none_object;
    }

    LZPY_IMP_METHOD(LzpyControl, clearChildren)
    {
        clearChildren();
        return none_object;
    }

    LZPY_IMP_METHOD(LzpyControl, getChildren)
    {
        list lst;

        for (VisitControl::iterator it = m_control->childBegin();
                it != m_control->childEnd(); ++it)
        {
            IControl *pctl = *it;
            if (pctl && pctl->getSelf())
            {
                lst.append(pctl->getSelf());
            }
        }

        return lst;
    }

    LZPY_IMP_METHOD(LzpyControl, findChildByPos)
    {
        CPoint pt;
        bool resculy;
        if (!arg.parse_tuple(&pt.x, &pt.y, &resculy)) return null_object;

        PControl p = m_control->finChildByPos(pt, resculy);
        if (!p || !p->getSelf()) return none_object;

        return object(p->getSelf());
    }

    LZPY_IMP_METHOD(LzpyControl, destroy)
    {
        object_base(this).call_method_quiet("onDestroy");

        list children = this->py_getChildren(null_object);
        size_t n = children.size();
        for (size_t i = 0; i < n; ++i)
        {
            object child = children[i];
            child.cast<LzpyControl>()->py_destroy(null_object);
        }

        m_control->clearChildren();
        m_control->removeFromParent();
        m_control->setSelf(null_object);
        m_control = nullptr;

        PyDict_Clear(m_pyDict);
        setManaged(false);
        return none_object;
    }


}//end namespace Lzpy

