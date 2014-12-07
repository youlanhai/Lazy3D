#include "stdafx.h"
#include "LPyPhysics.h"

#include <sstream>

namespace Lzpy
{

    //////////////////////////////////////////////////////////////////////
    /// PyVector2
    //////////////////////////////////////////////////////////////////////

    LZPY_CLASS_EXPORT(PyVector2)
    {
        LZPY_LINK_ATTR(tp_str, PyVector2::reprfunc);

        LZPY_MEMEBER(x, T_FLOAT, m_vector.x);
        LZPY_MEMEBER(y, T_FLOAT, m_vector.y);
        LZPY_METHOD(set);
    }

    PyVector2::PyVector2()
    {}

    /*static*/ PyObject * PyVector2::reprfunc(PyObject * self)
    {
        PyVector2 * pThis = (PyVector2*) self;

        std::wostringstream os;
        os << pThis->m_vector.x << ", " << pThis->m_vector.y;
        std::wstring str = os.str();
        return PyUnicode_FromWideChar(str.c_str(), str.size());
    }

    LZPY_IMP_METHOD(PyVector2, set)
    {
        if (!arg.parse_tuple(&m_vector.x, &m_vector.y))
            return null_object;

        return none_object;
    }

    //////////////////////////////////////////////////////////////////////
    /// PyVector3
    //////////////////////////////////////////////////////////////////////
    PyVector3 * PyVector3_add(PyVector3 * a, PyVector3 * b)
    {
        if (!CHECK_INSTANCE(PyVector3, a)) return nullptr;
        if (!CHECK_INSTANCE(PyVector3, b)) return nullptr;

        PyVector3 * c = new_instance_ex<PyVector3>();
        c->m_vector = a->m_vector + b->m_vector;
        return c;
    }

    PyVector3 * PyVector3_sub(PyVector3 * a, PyVector3 * b)
    {
        if (!CHECK_INSTANCE(PyVector3, a)) return nullptr;
        if (!CHECK_INSTANCE(PyVector3, b)) return nullptr;

        PyVector3 * c = new_instance_ex<PyVector3>();
        c->m_vector = a->m_vector - b->m_vector;
        return c;
    }

    PyVector3 * PyVector3_mutiply(PyVector3 * a, PyObject * b)
    {
        if (!CHECK_INSTANCE(PyVector3, a)) return nullptr;

        float v = (float)PyFloat_AsDouble(b);

        PyVector3 * c = new_instance_ex<PyVector3>();
        c->m_vector = a->m_vector * v;
        return c;
    }

    PyVector3 * PyVector3_div(PyVector3 * a, PyObject * b)
    {
        if (!CHECK_INSTANCE(PyVector3, a)) return nullptr;

        float v = (float) PyFloat_AsDouble(b);
        if (v == 0.0f)
        {
            PyErr_SetString(PyExc_ZeroDivisionError, "float div");
            return NULL;
        }

        PyVector3 * c = new_instance_ex<PyVector3>();
        c->m_vector = a->m_vector / v;
        return c;
    }

    PyVector3 * PyVector3_negative(PyVector3 * a)
    {
        if (!CHECK_INSTANCE(PyVector3, a)) return nullptr;

        PyVector3 * c = new_instance_ex<PyVector3>();
        c->m_vector = a->m_vector;
        c->m_vector.negative();
        return c;
    }

    static PyNumberMethods PyVector3_numberMethods =
    {
        (binaryfunc) PyVector3_add,
        (binaryfunc) PyVector3_sub,
        (binaryfunc) PyVector3_mutiply,
        (binaryfunc) PyVector3_div,
    };


    LZPY_CLASS_EXPORT(PyVector3)
    {
        LZPY_LINK_ATTR(tp_str, PyVector3::reprfunc);

        PyVector3_numberMethods.nb_negative = (unaryfunc) PyVector3_negative;
        LZPY_LINK_ATTR(tp_as_number, &PyVector3_numberMethods);


        LZPY_MEMEBER(x, T_FLOAT, m_vector.x);
        LZPY_MEMEBER(y, T_FLOAT, m_vector.y);
        LZPY_MEMEBER(z, T_FLOAT, m_vector.z);
        LZPY_METHOD(set);
    }

    PyVector3::PyVector3()
    {
    }

    LZPY_IMP_INIT(PyVector3)
    {
        return parse_object(m_vector, object(arg));
    }

    LZPY_IMP_METHOD(PyVector3, set)
    {
        if (!arg.parse_tuple(&m_vector.x, &m_vector.y, &m_vector.z))
            return null_object;

        return none_object;
    }

    PyObject * PyVector3::reprfunc(PyObject * self)
    {
        PyVector3 * pThis = (PyVector3*)self;

        std::wostringstream os;
        os << pThis->m_vector.x << ", " << pThis->m_vector.y << ", " << pThis->m_vector.z;
        std::wstring str = os.str();
        return PyUnicode_FromWideChar(str.c_str(), str.size());
    }

    //////////////////////////////////////////////////////////////////////
    /// export entry
    //////////////////////////////////////////////////////////////////////
    void exportPyPhsicis(const char * module)
    {
        LZPY_REGISTER_CLASS(Vector3, PyVector3);
    }


    object build_object(const Vector2 & v)
    {
        PyVector2 * p = new_instance_ex<PyVector2>();
        p->m_vector = v;
        return new_reference(p);
    }

    bool parse_object(Vector2 & v, object o)
    {
        if (has_instance<PyVector2>(o.get()))
        {
            v = (o.cast<PyVector2>())->m_vector;
            return true;
        }
        else if (o.is_tuple())
        {
            tuple arg(o);
            if (arg.parse_tuple(&v.x, &v.y))
                return true;
        }

        PyErr_SetString(PyExc_TypeError, "Must be a Vector2 or tuple2!");
        return false;
    }

    object build_object(const Vector3 & v)
    {
        PyVector3 * p = new_instance_ex<PyVector3>();
        p->m_vector = v;
        return new_reference(p);
    }

    bool parse_object(Vector3 & v, object o)
    {
        if (has_instance<PyVector3>(o.get()))
        {
            v = (o.cast<PyVector3>())->m_vector;
            return true;
        }
        else if (o.is_tuple())
        {
            tuple arg(o);
            if (arg.parse_tuple(&v.x, &v.y, &v.z))
                return true;
        }

        PyErr_SetString(PyExc_TypeError, "Must be a Vector3 or tuple3!");
        return false;
    }

}