#include "stdafx.h"
#include "LPyUIBase.h"
#include "LPyLzd.h"


namespace Lzpy
{
    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_EXPORT(LzpyControl)
    {
        LZPY_GET(parent);
        LZPY_GET(type);
        LZPY_GET(__dict__);
        LZPY_GETSET(size);
        LZPY_GETSET(position);
        LZPY_GETSET(absPosition);
        LZPY_GETSET(globalPosition);
        LZPY_GETSET(zorder);
        LZPY_GETSET(name);
        LZPY_GETSET(skin);
        LZPY_GETSET(align);
        LZPY_GETSET(visible);
        LZPY_GETSET(enable);
        LZPY_GETSET(dragable);
        LZPY_GETSET(messagable);
        LZPY_GETSET(topable);
        LZPY_GETSET(childOrderable);
        LZPY_GETSET(script);
        LZPY_GETSET(scriptName);

        LZPY_GETSET(globalRect);

        LZPY_METHOD(addChild);
        LZPY_METHOD(getChild);
        LZPY_METHOD(delChild);

        LZPY_METHOD(clearChildren);
        LZPY_METHOD(getChildren);
        LZPY_METHOD(findChildByPos);
        LZPY_METHOD(destroy);

        LZPY_METHOD(loadFromFile);
        LZPY_METHOD(saveToFile);

        LZPY_LINK_ATTR(tp_weaklistoffset, offsetof(LzpyControl, m_pyWeakreflist));
        LZPY_LINK_ATTR(tp_dictoffset, offsetof(LzpyControl, m_pyDict));
    }

    LZPY_IMP_INIT(LzpyControl)
    {
        PyErr_SetString(PyExc_TypeError, "PyWidget can't create an instance derectly!");
        return false;
    }

    LzpyControl::LzpyControl()
        : m_pyDict(nullptr)
        , m_pyWeakreflist(nullptr)
        , m_control(nullptr)
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
            m_control->setScript(null_object);
            m_control = nullptr;
        }

        if (m_pyWeakreflist != NULL)
            PyObject_ClearWeakRefs(this);

        Py_DECREF(m_pyDict);
    }

    void LzpyControl::clearChildren()
    {
        m_control->clearChildren();
    }

    bool LzpyControl::addChild(object_base child)
    {
        if (!CHECK_INSTANCE(LzpyControl, child.get())) return false;

        LzpyControl *p = child.cast<LzpyControl>();
        m_control->addChild(p->m_control);

        return true;
    }

    bool LzpyControl::delChild(object_base child)
    {
        if (!CHECK_INSTANCE(LzpyControl, child.get())) return false;

        LzpyControl *p = child.cast<LzpyControl>();
        m_control->delChild(p->m_control);

        return true;
    }

    bool LzpyControl::createUI(const tstring & type, PyObject *arg)
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
            m_control->setSelf(this);

            if (parent) parent->addChild(object(this));
        }

        return bool(m_control);
    }


    object_base LzpyControl::getParent()
    {
        if (m_control && m_control->getParent())
        {
            return m_control->getParent()->getSelf();
        }

        return null_object;
    }

    object LzpyControl::getSize()
    {
        const CPoint & pt = m_control->getSize();
        return build_tuple(pt.x, pt.y);
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

    object LzpyControl::getAbsPosition()
    {
        const CPoint & pos = m_control->getAbsPosition();
        return build_tuple(pos.x, pos.y);
    }

    void LzpyControl::setAbsPosition(object v)
    {
        tuple arg(v);
        if (!arg.check() || arg.size() != 2)
        {
            PyErr_SetString(PyExc_TypeError, "tuple(x, y) needed!");
            throw(python_error("TypeError"));
        }

        int x, y;
        arg.parse_tuple(&x, &y);
        m_control->setAbsPosition(x, y);
    }


    object LzpyControl::getGlobalPosition()
    {
        const CPoint & pos = m_control->getGlobalPosition();
        return build_tuple(pos.x, pos.y);
    }

    void LzpyControl::setGlobalPosition(object v)
    {
        tuple arg(v);
        if (!arg.check() || arg.size() != 2)
        {
            PyErr_SetString(PyExc_TypeError, "tuple(x, y) needed!");
            throw(python_error("TypeError"));
        }

        int x, y;
        arg.parse_tuple(&x, &y);
        m_control->setGlobalPosition(x, y);
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

        bool ret = false;// m_control->saveToFile(fname);
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
        tstring name;
        if (!arg.parse_tuple(&name)) return null_object;

        Widget * child = m_control->getChild(name);
        if (child && child->getSelf())
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

        const WidgetChildren & children = m_control->getChildren();
        for (const WidgetPtr & child : children)
        {
            if (child->getSelf())
            {
                lst.append(child->getSelf());
            }
        }

        return lst;
    }

    LZPY_IMP_METHOD(LzpyControl, findChildByPos)
    {
        CPoint pt;
        bool resculy;
        if (!arg.parse_tuple(&pt.x, &pt.y, &resculy)) return null_object;

        Widget * p = m_control->findChildByPos(pt, resculy);
        if (!p || !p->getSelf()) return none_object;

        return p->getSelf();
    }

    LZPY_IMP_METHOD(LzpyControl, destroy)
    {
        m_control->destroy();
        m_control = nullptr;

        PyDict_Clear(m_pyDict);
        return none_object;
    }


}//end namespace Lzpy

