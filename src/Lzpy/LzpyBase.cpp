#include "stdafx.h"
#include "LzpyBase.h"

namespace Lzpy
{
    LZPY_CLASS_EXPORT(PyBase)
    {
        LZPY_MEMEBER(refcount, T_INT, ob_refcnt);
    }

    PyBase::PyBase()
    {
    }

    PyBase::~PyBase()
    {
    }

    LZPY_IMP_INIT(PyBase)
    {
        return true;
    }

    //////////////////////////////////////////////
    ///
    //////////////////////////////////////////////
    LZPY_CLASS_EXPORT(PyScriptProxy)
    {
        LZPY_GET(object);
        LZPY_GETSET(script);

        LZPY_LINK_ATTR(tp_getattro, PyGetAttr);
        LZPY_LINK_ATTR(tp_setattro, PySetAttr);
    }

    LZPY_IMP_INIT(PyScriptProxy)
    {
        PyErr_SetString(PyExc_TypeError, "PyScriptProxy can't create derectly!");
        return false;
    }

    PyScriptProxy::PyScriptProxy()
        : m_object(nullptr)
    {

    }

    PyScriptProxy::~PyScriptProxy()
    {
        if (m_object != nullptr)
            m_object->decRef();
    }

    /*static*/ PyScriptProxy * PyScriptProxy::New(Lazy::ScriptObject *pObject)
    {
        if (pObject == nullptr)
            return nullptr;

        PyScriptProxy * p = new_instance_ex<PyScriptProxy>();
        p->m_object = pObject;
        p->m_object->incRef();

        return p;
    }


    /*static*/ PyObject * PyScriptProxy::PyGetAttr(PyObject * o, PyObject *attr_name)
    {
        if (!CHECK_INSTANCE(PyScriptProxy, o))
            return NULL;

        PyScriptProxy * p = (PyScriptProxy*) o;
        return PyObject_GetAttr(p->script(), attr_name);
    }

    /*static*/ int PyScriptProxy::PySetAttr(PyObject * o, PyObject *attr_name, PyObject * value)
    {
        if (!CHECK_INSTANCE(PyScriptProxy, o))
            return -1;

        PyScriptProxy * p = (PyScriptProxy*) o;
        return PyObject_SetAttr(p->script(), attr_name, value);
    }
}
