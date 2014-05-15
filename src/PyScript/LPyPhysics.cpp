#include "stdafx.h"
#include "LPyPhysics.h"

namespace LazyPy
{

    PyVector3 * PyVector3_add(PyVector3 * a, PyVector3 * b)
    {
        if (!helper::has_instance<PyVector3>(a, true)) return nullptr;
        if (!helper::has_instance<PyVector3>(b, true)) return nullptr;

        PyVector3 * c = helper::new_instance_ex<PyVector3>();
        c->m_vector = a->m_vector + b->m_vector;
        return c;
    }

    PyVector3 * PyVector3_sub(PyVector3 * a, PyVector3 * b)
    {
        if (!helper::has_instance<PyVector3>(a, true)) return nullptr;
        if (!helper::has_instance<PyVector3>(b, true)) return nullptr;

        PyVector3 * c = helper::new_instance_ex<PyVector3>();
        c->m_vector = a->m_vector - b->m_vector;
        return c;
    }

    PyVector3 * PyVector3_mutiply(PyVector3 * a, PyObject * b)
    {
        if (!helper::has_instance<PyVector3>(a, true)) return nullptr;

        float v = (float)PyFloat_AsDouble(b);

        PyVector3 * c = helper::new_instance_ex<PyVector3>();
        c->m_vector = a->m_vector * v;
        return c;
    }

    PyVector3 * PyVector3_div(PyVector3 * a, PyObject * b)
    {
        if (!helper::has_instance<PyVector3>(a, true)) return nullptr;

        float v = (float) PyFloat_AsDouble(b);
        if (v == 0.0f)
        {
            PyErr_SetString(PyExc_ZeroDivisionError, "float div");
            return NULL;
        }

        PyVector3 * c = helper::new_instance_ex<PyVector3>();
        c->m_vector = a->m_vector / v;
        return c;
    }

    PyVector3 * PyVector3_negative(PyVector3 * a)
    {
        if (!helper::has_instance<PyVector3>(a, true)) return nullptr;

        PyVector3 * c = helper::new_instance_ex<PyVector3>();
        c->m_vector = a->m_vector;
        c->m_vector.negative();
        return c;
    }

    static PyNumberMethods PyVector3_numberMethods = {
        (binaryfunc) PyVector3_add,
        (binaryfunc) PyVector3_sub,
        (binaryfunc) PyVector3_mutiply,
        (binaryfunc) PyVector3_div,
    };

    LAZYPY_IMP("Vector3", PyVector3, "Lazy");

    LAZYPY_BEGIN_EXTEN(PyVector3);
    LAZYPY_LINK_ATTR(tp_str, PyVector3::reprfunc);

    PyVector3_numberMethods.nb_negative = (unaryfunc) PyVector3_negative;
    LAZYPY_LINK_ATTR(tp_as_number, &PyVector3_numberMethods);


    LAZYPY_MEMEBER("x", T_FLOAT, m_vector.x);
    LAZYPY_MEMEBER("y", T_FLOAT, m_vector.y);
    LAZYPY_MEMEBER("z", T_FLOAT, m_vector.z);
    LAZYPY_METHOD(set);

    LAZYPY_END_EXTEN();

    PyVector3::PyVector3()
    {
    }


    LAZYPY_IMP_INIT(PyVector3)
    {
        return parse_object(m_vector, borrow_reference(arg));
    }

    LAZYPY_IMP_METHOD(PyVector3, set)
    {
        if (!PyArg_ParseTuple(arg, "fff", &m_vector.x, &m_vector.y, &m_vector.z)) return nullptr;

        Py_RETURN_NONE;
    }

    PyObject * PyVector3::reprfunc(PyObject * self)
    {
        PyVector3 * pThis = (PyVector3*)self;

        std::wstring str;
        Lazy::formatStringW(str, L"%f, %f, %f", pThis->m_vector.x, pThis->m_vector.y, pThis->m_vector.z);
        return fromString(str);
    }


    object build_object(const Math::Vector3 & v)
    {
        PyVector3 * p = helper::new_instance_ex<PyVector3>();
        p->m_vector = v;
        return new_reference(p);
    }

    bool parse_object(Math::Vector3 & v, object o)
    {
        if (helper::has_instance<PyVector3>(o.get()))
        {
            v = (o.cast<PyVector3>())->m_vector;
            return true;
        }
        else if (o.is_tuple())
        {
            tuple arg(o);

            try
            {
                arg.parse_tuple(&v.x, &v.y, &v.z);
            }
            catch (python_error e)
            {
                return false;
            }

            return true;
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "Must be a Vector3 or tuple3!");
        }

        return false;
    }
}