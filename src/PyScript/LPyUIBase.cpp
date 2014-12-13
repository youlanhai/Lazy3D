#include "stdafx.h"
#include "LPyUIBase.h"
#include "LPyLzd.h"


namespace Lzpy
{
    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_EXPORT(PyWidget)
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

        LZPY_LINK_ATTR(tp_weaklistoffset, offsetof(PyWidget, m_pyWeakreflist));
        LZPY_LINK_ATTR(tp_dictoffset, offsetof(PyWidget, m_pyDict));
    }

    LZPY_IMP_INIT_LUI(PyWidget);

    PyWidget::PyWidget()
        : m_pyDict(nullptr)
        , m_pyWeakreflist(nullptr)
        , m_object(nullptr)
    {
        m_pyDict = PyDict_New();
    }

    PyWidget::~PyWidget()
    {
#ifdef _DEBUG
        {
            PyObject * pStr = PyObject_Repr(this);

            wchar_t *p = PyUnicode_AsUnicodeAndSize(pStr, NULL);
            LOG_DEBUG(L"%s deleted.", p);
            Py_DECREF(pStr);
        }
#endif

        if (m_object)
        {
            clearChildren();

            m_object->removeFromParent();
            m_object->setScript(null_object);
            m_object = nullptr;
        }

        if (m_pyWeakreflist != NULL)
            PyObject_ClearWeakRefs(this);

        Py_DECREF(m_pyDict);
    }

    void PyWidget::clearChildren()
    {
        m_object->clearChildren();
    }

    bool PyWidget::addChild(object_base child)
    {
        if (!CHECK_INSTANCE(PyWidget, child.get())) return false;

        PyWidget *p = child.cast<PyWidget>();
        m_object->addChild(p->m_object);

        return true;
    }

    bool PyWidget::delChild(object_base child)
    {
        if (!CHECK_INSTANCE(PyWidget, child.get())) return false;

        PyWidget *p = child.cast<PyWidget>();
        m_object->delChild(p->m_object);

        return true;
    }

    object PyWidget::getSize()
    {
        const CPoint & pt = m_object->getSize();
        return build_tuple(pt.x, pt.y);
    }

    void PyWidget::setSize(object v)
    {
        tuple arg(v);
        if (!arg.check() || arg.size() != 2)
        {
            PyErr_SetString(PyExc_TypeError, "tuple(w, h) needed!");
            throw(python_error("TypeError"));
        }

        CSize size;
        arg.parse_tuple(&size.cx, &size.cy);
        m_object->setSize(size.cx, size.cy);
    }

    object PyWidget::getPosition()
    {
        const CPoint & pt = m_object->getPosition();
        return build_tuple(pt.x, pt.y);
    }

    void PyWidget::setPosition(object v)
    {
        tuple arg(v);
        if (!arg.check() || arg.size() != 2)
        {
            PyErr_SetString(PyExc_TypeError, "tuple(x, y) needed!");
            throw(python_error("TypeError"));
        }

        CPoint pt;
        arg.parse_tuple(&pt.x, &pt.y);
        m_object->setPosition(pt.x, pt.y);
    }

    object PyWidget::getAbsPosition()
    {
        const CPoint & pos = m_object->getAbsPosition();
        return build_tuple(pos.x, pos.y);
    }

    void PyWidget::setAbsPosition(object v)
    {
        tuple arg(v);
        if (!arg.check() || arg.size() != 2)
        {
            PyErr_SetString(PyExc_TypeError, "tuple(x, y) needed!");
            throw(python_error("TypeError"));
        }

        int x, y;
        arg.parse_tuple(&x, &y);
        m_object->setAbsPosition(x, y);
    }


    object PyWidget::getGlobalPosition()
    {
        const CPoint & pos = m_object->getGlobalPosition();
        return build_tuple(pos.x, pos.y);
    }

    void PyWidget::setGlobalPosition(object v)
    {
        tuple arg(v);
        if (!arg.check() || arg.size() != 2)
        {
            PyErr_SetString(PyExc_TypeError, "tuple(x, y) needed!");
            throw(python_error("TypeError"));
        }

        int x, y;
        arg.parse_tuple(&x, &y);
        m_object->setGlobalPosition(x, y);
    }

    tuple PyWidget::getGlobalRect()
    {
        CRect rc = m_object->getGlobalRect();
        return build_tuple(rc.left, rc.top, rc.right, rc.bottom);
    }

    void PyWidget::setGlobalRect(tuple rect)
    {
        CRect rc;
        if (!rect.parse_tuple(&rc.left, &rc.top, &rc.right, &rc.bottom)) return;

        m_object->setGlobalRect(rc);
    }

    LZPY_IMP_METHOD(PyWidget, loadFromFile)
    {
        std::wstring fname;
        if (!arg.parse_tuple(&fname)) return null_object;

        bool ret = m_object->loadFromFile(fname);
        return build_object(ret);
    }

    LZPY_IMP_METHOD(PyWidget, saveToFile)
    {
        std::wstring fname;
        if (!arg.parse_tuple(&fname)) return null_object;

        bool ret = false;// m_object->saveToFile(fname);
        return build_object(ret);
    }


    LZPY_IMP_METHOD(PyWidget, addChild)
    {
        object_base child;
        if (!arg.parse_tuple(&child))
            return null_object;

        if (!addChild(child))
            return null_object;

        return none_object;
    }

    LZPY_IMP_METHOD(PyWidget, getChild)
    {
        tstring name;
        if (!arg.parse_tuple(&name)) return null_object;

        Widget * child = m_object->getChild(name);
        if (child && child->getSelf())
            return child->getSelf();

        return none_object;
    }

    LZPY_IMP_METHOD(PyWidget, delChild)
    {
        object_base child;
        if (!arg.parse_tuple(&child))
            return null_object;

        if (!delChild(child))
            return null_object;

        return none_object;
    }

    LZPY_IMP_METHOD(PyWidget, clearChildren)
    {
        clearChildren();
        return none_object;
    }

    LZPY_IMP_METHOD(PyWidget, getChildren)
    {
        list lst;

        const WidgetChildren & children = m_object->getChildren();
        for (const WidgetPtr & child : children)
        {
            if (child->getSelf())
            {
                lst.append(child->getSelf());
            }
        }

        return lst;
    }

    LZPY_IMP_METHOD(PyWidget, findChildByPos)
    {
        CPoint pt;
        bool resculy;
        if (!arg.parse_tuple(&pt.x, &pt.y, &resculy)) return null_object;

        Widget * p = m_object->findChildByPos(pt, resculy);
        if (!p || !p->getSelf()) return none_object;

        return p->getSelf();
    }

    LZPY_IMP_METHOD(PyWidget, destroy)
    {
        m_object->destroy();
        m_object = nullptr;

        PyDict_Clear(m_pyDict);
        return none_object;
    }


}//end namespace Lzpy

