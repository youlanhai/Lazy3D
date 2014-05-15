#include "stdafx.h"
#include "LzpyObject.h"
#include "LzpyTuple.h"
#include "LzpyStr.h"
#include "LzpyDict.h"
#include "LzpyList.h"

namespace Lzpy
{

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

    object build_object(const object & v)
    {
        return v;
    }

    object build_object(const tuple & v)
    {
        return borrow_reference(v.get());
    }

    object build_object(const dict & v)
    {
        return borrow_reference(v.get());
    }

    object build_object(const str & v)
    {
        return borrow_reference(v.get());
    }

    object build_object(const list & v)
    {
        return borrow_reference(v.get());
    }

    object build_object(PyObject *p)
    {
        return object(borrow_reference(p));
    }

    object build_object(const borrow_reference & v)
    {
        return object(v);
    }

    object build_object(const new_reference & v)
    {
        return object(v);
    }

    ////////////////////////////////////////////////////////////////////
    // class object
    ////////////////////////////////////////////////////////////////////

    python_error::python_error()
    {}

    python_error::python_error(const char * error)
        : exception(error)
    {}


    object object::call_python()
    {
        return call_python(tuple());
    }

    object object::call_python(tuple arg)
    {
        if (!callable())
            throw(python_error("This object doesn't callable!"));

        return new_reference(PyObject_Call(m_ptr, arg.get(), nullptr));
    }

    object object::call_python(tuple arg, dict kwd)
    {
        if (!callable())
            throw(python_error("This object doesn't callable!"));

        return new_reference(PyObject_Call(m_ptr, arg.get(), kwd.get()));
    }


    bool object::hasattr(const char *attr) const
    {
        assert(m_ptr);
        return 0 != PyObject_HasAttrString(m_ptr, attr);
    }

    object object::getattr(const char *attr) const
    {
        object ret = new_reference(PyObject_GetAttrString(m_ptr, attr));
        if (ret.is_null())
            throw(python_error("Attribute Error!"));

        return ret;
    }

    bool object::setattr(const char *attr, object value)
    {
        return -1 != PyObject_SetAttrString(m_ptr, attr, value.get());
    }

    bool object::delattr(const char *attr)
    {
        assert(m_ptr);
        return -1 != PyObject_DelAttrString(m_ptr, attr);
    }

    bool object::callable() const
    {
        if (nullptr == m_ptr) return false;
        return 1 == PyCallable_Check(m_ptr);
    }

    bool object::is_null() const
    {
        return nullptr == m_ptr;
    }

    bool object::is_none() const
    {
        return m_ptr == Py_None;
    }

    bool object::is_int() const
    {
        return m_ptr && PyLong_Check(m_ptr);
    }

    bool object::is_float() const
    {
        return m_ptr && PyFloat_Check(m_ptr);
    }

    bool object::is_bool() const
    {
        return m_ptr && PyBool_Check(m_ptr);
    }

    bool object::is_str() const
    {
        return m_ptr && PyUnicode_Check(m_ptr);
    }

    bool object::is_tuple() const
    {
        return m_ptr && PyTuple_Check(m_ptr);
    }

    bool object::is_list() const
    {
        return m_ptr && PyList_Check(m_ptr);
    }

    bool object::is_dict() const
    {
        return m_ptr && PyDict_Check(m_ptr);
    }

    bool object::is_module() const
    {
        return m_ptr && PyModule_Check(m_ptr);
    }

    Lazy::tstring object::repr()
    {
        if (is_null()) return _T("null");
        
        object v = new_reference(PyObject_Repr(m_ptr));
        return v.parse<Lazy::tstring>();
    }

    void object::print()
    {
        Lazy::tstring text = repr();
        LOG_INFO(_T("python object info: %s"), text.c_str());
    }
}