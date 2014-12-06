#include "stdafx.h"
#include "LzpyObject.h"
#include "LzpyTuple.h"
#include "LzpyStr.h"
#include "LzpyDict.h"
#include "LzpyList.h"

namespace Lzpy
{
    PyObject * xincref(const object & v)
    {
        return xincref(v.get());
    }

    PyObject * xdecref(const object & v)
    {
        return xdecref(v.get());
    }
    ////////////////////////////////////////////////////////////////////
    // parse_object
    ////////////////////////////////////////////////////////////////////
    bool parse_object(bool &v, const object & o)
    {
        v = bool(o);
        return true;
    }

    bool parse_object(int & v, const object & o)
    {
        if (o.is_int())
        {
            v = PyLong_AsLong(o.get());
        }
        else if (o.is_float())
        {
            v = (int) PyFloat_AsDouble(o.get());
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "int type needed!");
            return false;
        }

        return true;
    }

    bool parse_object(unsigned int & v, const object & o)
    {
        if (o.is_int())
        {
            v = PyLong_AsUnsignedLong(o.get());
        }
        else if (o.is_float())
        {
            v = (unsigned int) PyFloat_AsDouble(o.get());
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "int type needed!");
            return false;
        }

        return true;
    }

    bool parse_object(long & v, const object & o)
    {
        if (o.is_int())
        {
            v = PyLong_AsLong(o.get());
        }
        else if (o.is_float())
        {
            v = (long) PyFloat_AsDouble(o.get());
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "int type needed!");
            return false;
        }

        return true;
    }

    bool parse_object(unsigned long & v, const object & o)
    {
        if (o.is_int())
        {
            v = PyLong_AsUnsignedLong(o.get());
        }
        else if (o.is_float())
        {
            v = (unsigned long) PyFloat_AsDouble(o.get());
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "int type needed!");
            return false;
        }

        return true;
    }

    bool parse_object(float & v, const object & o)
    {
        if (o.is_float())
        {
            v = (float) PyFloat_AsDouble(o.get());
        }
        else if (o.is_int())
        {
            v = (float) PyLong_AsDouble(o.get());
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "float type needed!");
            return false;
        }

        return true;
    }

    bool parse_object(double & v, const object & o)
    {
        if (o.is_float())
        {
            v = PyFloat_AsDouble(o.get());
        }
        else if (o.is_int())
        {
            v = PyLong_AsDouble(o.get());
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "float type needed!");
            return false;
        }

        return true;
    }

    bool parse_object(std::string & v, const object & o)
    {
        if (!o.is_str())
        {
            PyErr_SetString(PyExc_TypeError, "utf-8 string type needed!");
            return false;
        }

        Py_ssize_t n;
        char *p = PyUnicode_AsUTF8AndSize(o.get(), &n);
        v.assign(p, p + n);
        return true;
    }

    bool parse_object(std::wstring & v, const object & o)
    {
        if (!o.is_str())
        {
            PyErr_SetString(PyExc_TypeError, "unicode string type needed!");
            return false;
        }

        Py_ssize_t n;
        wchar_t *p = PyUnicode_AsUnicodeAndSize(o.get(), &n);
        v.assign(p, p + n);
        return true;
    }

    bool parse_object(char *& v, const object & o)
    {
        if (!o.is_str())
        {
            PyErr_SetString(PyExc_TypeError, "utf-8 str type needed!");
            return false;
        }

        v = PyUnicode_AsUTF8(o.get());
        return true;
    }

    bool parse_object(wchar_t *& v, const object & o)
    {
        if (!o.is_str())
        {
            PyErr_SetString(PyExc_TypeError, "unicode str type needed!");
            return false;
        }

        v = PyUnicode_AsUnicode(o.get());
        return true;
    }

    bool parse_object(object_base & v, const object & o)
    {
        v = o;
        return true;
    }

    bool parse_object(object & v, const object & o)
    {
        v = o;
        return true;
    }

    bool parse_object(tuple & v, const object & o)
    {
        if (!o.is_tuple())
        {
            PyErr_SetString(PyExc_TypeError, "tuple type needed!");
            return false;
        }

        v = o;
        return true;
    }

    bool parse_object(dict & v, const object & o)
    {
        if (!o.is_dict())
        {
            PyErr_SetString(PyExc_TypeError, "dict type needed!");
            return false;
        }

        v = o;
        return true;
    }

    bool parse_object(str & v, const object & o)
    {
        if (!o.is_str())
        {
            PyErr_SetString(PyExc_TypeError, "str type needed!");
            return false;
        }

        v = o;
        return true;
    }

    bool parse_object(list & v, const object & o)
    {
        if (!o.is_list())
        {
            PyErr_SetString(PyExc_TypeError, "list type needed!");
            return false;
        }

        v = o;
        return true;
    }

    ////////////////////////////////////////////////////////////////////
    // build_object
    ////////////////////////////////////////////////////////////////////

    object build_object(bool v)
    {
        return new_reference(PyBool_FromLong(v));
    }

    object build_object(int v)
    {
        return new_reference(PyLong_FromLong(v));
    }

    object build_object(unsigned int v)
    {
        return new_reference(PyLong_FromUnsignedLong(v));
    }

    object build_object(long v)
    {
        return new_reference(PyLong_FromLong(v));
    }

    object build_object(unsigned long v)
    {
        return new_reference(PyLong_FromUnsignedLong(v));
    }

    object build_object(float v)
    {
        return new_reference(PyFloat_FromDouble(v));
    }

    object build_object(double v)
    {
        return new_reference(PyFloat_FromDouble(v));
    }

    object build_object(const std::string & v)
    {
        return new_reference(PyUnicode_FromStringAndSize(v.c_str(), v.size()));
    }

    object build_object(const std::wstring & v)
    {
        return new_reference(PyUnicode_FromUnicode(v.c_str(), v.size()));
    }

    object build_object(const char * v)
    {
        return new_reference(PyUnicode_FromString(v));
    }

    object build_object(const wchar_t * v)
    {
        return new_reference(PyUnicode_FromUnicode(v, wcslen(v)));
    }

    object build_object(const object_base & v)
    {
        return v;
    }

    object build_object(const object & v)
    {
        return v;
    }

    object build_object(const tuple & v)
    {
        return object(v);
    }

    object build_object(const dict & v)
    {
        return object(v);
    }

    object build_object(const str & v)
    {
        return object(v);
    }

    object build_object(const list & v)
    {
        return object(v);
    }

    object build_object(PyObject *p)
    {
        return object(p);
    }

    object build_object(const new_reference & v)
    {
        return object(v);
    }

    ////////////////////////////////////////////////////////////////////
    // python exception
    ////////////////////////////////////////////////////////////////////

    python_error::python_error()
    {}

    python_error::python_error(const char * error)
        : exception(error)
    {}

    ////////////////////////////////////////////////////////////////////
    // class object_base
    ////////////////////////////////////////////////////////////////////

    object_base::object_base()
        : m_ptr(nullptr)
    {}

    object_base::object_base(PyObject *p)
        : m_ptr(p)
    {}

    object_base::object_base(const object_base & t)
        : m_ptr(t.m_ptr)
    {}

    object_base::~object_base()
    {}

    object_base::operator bool() const
    {
        return m_ptr && PyObject_IsTrue(m_ptr);
    }

    const object_base & object_base::operator = (const object_base & o)
    {
        m_ptr = o.m_ptr;
        return *this;
    }

    bool object_base::operator == (const object_base & o) const
    {
        return m_ptr == o.m_ptr;
    }

    bool object_base::operator < (const object_base & o) const
    {
        return m_ptr < o.m_ptr;
    }

    bool object_base::operator > (const object_base & o) const
    {
        return m_ptr > o.m_ptr;
    }

    object object_base::call_python()
    {
        return call_python(tuple(), null_object);
    }

    object object_base::call_python(tuple arg)
    {
        return call_python(arg, null_object);
    }

    object object_base::call_python(tuple arg, dict kwd)
    {
        if (!callable())
            throw(python_error("This object doesn't callable!"));

        PyObject * ret = PyObject_Call(m_ptr, arg.get(), kwd.get());
        if (ret == nullptr)
        {
            PyErr_Print();
        }

        return new_reference(ret);
    }

    bool object_base::hasattr(const char *attr) const
    {
        assert(m_ptr);
        return 0 != PyObject_HasAttrString(m_ptr, attr);
    }

    object object_base::getattr(const char *attr) const
    {
        object ret = new_reference(PyObject_GetAttrString(m_ptr, attr));
        if (ret.is_null())
            throw(python_error("Attribute Error!"));

        return ret;
    }

    bool object_base::setattr(const char *attr, object value)
    {
        return -1 != PyObject_SetAttrString(m_ptr, attr, value.get());
    }

    bool object_base::delattr(const char *attr)
    {
        assert(m_ptr);
        return -1 != PyObject_DelAttrString(m_ptr, attr);
    }

    bool object_base::callable() const
    {
        if (nullptr == m_ptr) return false;
        return 1 == PyCallable_Check(m_ptr);
    }

    bool object_base::is_null() const
    {
        return nullptr == m_ptr;
    }

    bool object_base::is_none() const
    {
        return m_ptr == Py_None;
    }

    bool object_base::is_int() const
    {
        return m_ptr && PyLong_Check(m_ptr);
    }

    bool object_base::is_float() const
    {
        return m_ptr && PyFloat_Check(m_ptr);
    }

    bool object_base::is_bool() const
    {
        return m_ptr && PyBool_Check(m_ptr);
    }

    bool object_base::is_str() const
    {
        return m_ptr && PyUnicode_Check(m_ptr);
    }

    bool object_base::is_tuple() const
    {
        return m_ptr && PyTuple_Check(m_ptr);
    }

    bool object_base::is_list() const
    {
        return m_ptr && PyList_Check(m_ptr);
    }

    bool object_base::is_dict() const
    {
        return m_ptr && PyDict_Check(m_ptr);
    }

    bool object_base::is_module() const
    {
        return m_ptr && PyModule_Check(m_ptr);
    }

    Lazy::tstring object_base::repr()
    {
        if (is_null()) return _T("null");

        object pyStr = new_reference(PyObject_Repr(m_ptr));
        Lazy::tstring text;
        parse_object(text, pyStr);
        return text;
    }

    std::string object_base::repr_a()
    {
        if (is_null()) return "null";

        object pyStr = new_reference(PyObject_Repr(m_ptr));
        std::string text;
        parse_object(text, pyStr);
        return text;
    }

    std::wstring object_base::repr_u()
    {
        if (is_null()) return _T("null");

        object pyStr = new_reference(PyObject_Repr(m_ptr));
        std::wstring text;
        parse_object(text, pyStr);
        return text;
    }

    void object_base::print()
    {
        Lazy::tstring text = repr();
        LOG_INFO(_T("python object info: %s"), text.c_str());
    }

    ////////////////////////////////////////////////////////////////////
    // class object
    ////////////////////////////////////////////////////////////////////

    object::object()
    {}

    object::~object()
    {
        delRef();
    }

#if 0
    object::object(PyObject *v)
    {
        m_ptr = xincref(v);
    }

    object::object(const object & v)
    {
        m_ptr = xincref(v.get());
    }

    object::object(const object_base & v)
    {
        m_ptr = xincref(v.get());
    }

    object::object(const new_reference & v)
    {
        m_ptr = v.get();
    }

    const object & object::operator = (const object & v)
    {
        set_borrow(v.get());
        return *this;
    }

    const object & object::operator = (const object_base & v)
    {
        set_borrow(v.get());
        return *this;
    }

    const object & object::operator = (const new_reference & v)
    {
        set_new(v.get());
        return *this;
    }
#endif

    //borrowed refrence
    void object::set_borrow(PyObject *p)
    {
        if (p == m_ptr) return;

        Py_XINCREF(p);
        Py_XDECREF(m_ptr);
        m_ptr = p;
    }

    //new refrence
    void object::set_new(PyObject *p)
    {
        if (p == m_ptr) return;

        Py_XDECREF(m_ptr);
        m_ptr = p;
    }
}
