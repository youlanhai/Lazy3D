#include "stdafx.h"
#include "LPyUIBase.h"
#include "LPyLzd.h"


namespace Lazy
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
        if (m_pyWeakreflist != NULL)
            PyObject_ClearWeakRefs(this);

        Py_DECREF(m_pyDict);
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
        Widget * child;
        if (!arg.parse_tuple(&child)) return null_object;

        m_object->addChild(child);
        return none_object;
    }

    LZPY_IMP_METHOD(PyWidget, getChild)
    {
        tstring name;
        if (!arg.parse_tuple(&name)) return null_object;

        Widget * child = m_object->getChild(name);
        return build_object(child);
    }

    LZPY_IMP_METHOD(PyWidget, delChild)
    {
        Widget * child;
        if (!arg.parse_tuple(&child)) return null_object;

        m_object->delChild(child);
        return none_object;
    }

    LZPY_IMP_METHOD(PyWidget, clearChildren)
    {
        m_object->clearChildren();
        return none_object;
    }

    LZPY_IMP_METHOD(PyWidget, getChildren)
    {
        list lst;

        const WidgetChildren & children = m_object->getChildren();
        for (const WidgetPtr & child : children)
        {
            lst.append(child.get());
        }

        return lst;
    }

    LZPY_IMP_METHOD(PyWidget, findChildByPos)
    {
        CPoint pt;
        bool resculy;
        if (!arg.parse_tuple(&pt.x, &pt.y, &resculy)) return null_object;

        Widget * p = m_object->findChildByPos(pt, resculy);
        return build_object(p);
    }

    LZPY_IMP_METHOD(PyWidget, destroy)
    {
        m_object->destroy();

        PyDict_Clear(m_pyDict);
        return none_object;
    }


}//end namespace Lazy

