#include "stdafx.h"
#include "LPyPhysics.h"

#include <sstream>

namespace Lazy
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

    LZPY_IMP_INIT(PyVector2)
    {
        return true;
    }

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
    /// PyVector4
    //////////////////////////////////////////////////////////////////////

    LZPY_CLASS_EXPORT(PyVector4)
    {
        LZPY_LINK_ATTR(tp_str, PyVector4::reprfunc);

        LZPY_MEMEBER(x, T_FLOAT, m_vector.x);
        LZPY_MEMEBER(y, T_FLOAT, m_vector.y);
        LZPY_METHOD(set);
    }

    PyVector4::PyVector4()
    {}

    LZPY_IMP_INIT(PyVector4)
    {
        return true;
    }

    /*static*/ PyObject * PyVector4::reprfunc(PyObject * self)
    {
        PyVector4 * pThis = (PyVector4*) self;

        std::wostringstream os;
        os << pThis->m_vector.x << ", " << pThis->m_vector.y << ", "
            << pThis->m_vector.z << ", " << pThis->m_vector.w;
        std::wstring str = os.str();
        return PyUnicode_FromWideChar(str.c_str(), str.size());
    }

    LZPY_IMP_METHOD(PyVector4, set)
    {
        if (!arg.parse_tuple(&m_vector.x, &m_vector.y, &m_vector.z, &m_vector.w))
            return null_object;

        return none_object;
    }
    //////////////////////////////////////////////////////////////////////
    /// PyQuaternion
    //////////////////////////////////////////////////////////////////////

    LZPY_CLASS_EXPORT(PyQuaternion)
    {
        LZPY_LINK_ATTR(tp_str, PyQuaternion::reprfunc);

        LZPY_MEMEBER(x, T_FLOAT, m_value.x);
        LZPY_MEMEBER(y, T_FLOAT, m_value.y);
        LZPY_METHOD(set);
    }

    PyQuaternion::PyQuaternion()
    {}

    LZPY_IMP_INIT(PyQuaternion)
    {
        return true;
    }

    /*static*/ PyObject * PyQuaternion::reprfunc(PyObject * self)
    {
        PyQuaternion * pThis = (PyQuaternion*) self;

        std::wostringstream os;
        os << pThis->m_value.x << ", " << pThis->m_value.y << ", "
            << pThis->m_value.z << ", " << pThis->m_value.w;
        std::wstring str = os.str();
        return PyUnicode_FromWideChar(str.c_str(), str.size());
    }

    LZPY_IMP_METHOD(PyQuaternion, set)
    {
        if (!arg.parse_tuple(&m_value.x, &m_value.y, &m_value.z, &m_value.w))
            return null_object;

        return none_object;
    }
    //////////////////////////////////////////////////////////////////////
    /// PyAABB
    //////////////////////////////////////////////////////////////////////

    LZPY_CLASS_EXPORT(PyAABB)
    {
        LZPY_LINK_ATTR(tp_str, PyAABB::reprfunc);

        LZPY_GETSET(min);
        LZPY_GETSET(max);
    }

    PyAABB::PyAABB()
    {}

    LZPY_IMP_INIT(PyAABB)
    {
        return true;
    }

    /*static*/ PyObject * PyAABB::reprfunc(PyObject * self)
    {
        PyAABB * pThis = (PyAABB*) self;

        std::wostringstream os;
        os << pThis->m_aabb.min.x << ", " << pThis->m_aabb.min.y << ", " << pThis->m_aabb.min.z << ", "
            << pThis->m_aabb.max.x << ", " << pThis->m_aabb.max.y << ", " << pThis->m_aabb.max.z;
        std::wstring str = os.str();
        return PyUnicode_FromWideChar(str.c_str(), str.size());
    }
    //////////////////////////////////////////////////////////////////////
    /// PyFRect
    //////////////////////////////////////////////////////////////////////

    LZPY_CLASS_EXPORT(PyFRect)
    {
        LZPY_LINK_ATTR(tp_str, PyFRect::reprfunc);

        LZPY_MEMEBER(left, T_FLOAT, m_rc.left);
        LZPY_MEMEBER(top, T_FLOAT, m_rc.top);
        LZPY_MEMEBER(right, T_FLOAT, m_rc.right);
        LZPY_MEMEBER(bottom, T_FLOAT, m_rc.bottom);

        LZPY_METHOD(set);
    }

    PyFRect::PyFRect()
    {}

    LZPY_IMP_INIT(PyFRect)
    {
        return true;
    }

    /*static*/ PyObject * PyFRect::reprfunc(PyObject * self)
    {
        PyFRect * pThis = (PyFRect*) self;

        std::wostringstream os;
        os << pThis->m_rc.left << ", " << pThis->m_rc.top << ", "
            << pThis->m_rc.right << ", " << pThis->m_rc.bottom;
        std::wstring str = os.str();
        return PyUnicode_FromWideChar(str.c_str(), str.size());
    }

    LZPY_IMP_METHOD(PyFRect, set)
    {
        if (!arg.parse_tuple(&m_rc.left, &m_rc.top, &m_rc.right, &m_rc.bottom))
            return null_object;

        return none_object;
    }
    //////////////////////////////////////////////////////////////////////
    /// export entry
    //////////////////////////////////////////////////////////////////////
    void exportPhysics(const char * module)
    {
        LZPY_REGISTER_CLASS(Vector2, PyVector2);
        LZPY_REGISTER_CLASS(Vector3, PyVector3);
        LZPY_REGISTER_CLASS(Vector4, PyVector4);
        LZPY_REGISTER_CLASS(Quaternion, PyQuaternion);
        LZPY_REGISTER_CLASS(AABB, PyAABB);
        LZPY_REGISTER_CLASS(FRect, PyFRect);
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


    object build_object(const Vector4 & v)
    {
        PyVector4 * p = new_instance_ex<PyVector4>();
        p->m_vector = v;
        return new_reference(p);
    }

    bool parse_object(Vector4 & v, object o)
    {
        if (has_instance<PyVector4>(o.get()))
        {
            v = (o.cast<PyVector4>())->m_vector;
            return true;
        }
        else if (o.is_tuple())
        {
            tuple arg(o);
            if (arg.parse_tuple(&v.x, &v.y, &v.z, &v.w))
                return true;
        }

        PyErr_SetString(PyExc_TypeError, "Must be a Vector4 or tuple4!");
        return false;
    }


    object build_object(const Quaternion & v)
    {
        PyQuaternion * p = new_instance_ex<PyQuaternion>();
        p->m_value = v;
        return new_reference(p);
    }

    bool parse_object(Quaternion & v, object o)
    {
        if (has_instance<PyQuaternion>(o.get()))
        {
            v = (o.cast<PyQuaternion>())->m_value;
            return true;
        }
        else if (o.is_tuple())
        {
            tuple arg(o);
            if (arg.parse_tuple(&v.x, &v.y, &v.z, &v.w))
                return true;
        }

        PyErr_SetString(PyExc_TypeError, "Must be a Quaternion or tuple4!");
        return false;
    }

    object build_object(const AABB & v)
    {
        PyAABB *p = new_instance_ex<PyAABB>();
        p->m_aabb = v;
        return new_reference(p);
    }

    bool parse_object(AABB & v, object o)
    {
        if (!CHECK_INSTANCE(PyAABB, o.get()))
            return false;

        o.cast<PyAABB>()->m_aabb = v;
        return true;
    }

    object build_object(const FRect & v)
    {
        PyFRect * p = new_instance_ex<PyFRect>();
        p->m_rc = v;
        return new_reference(p);
    }

    bool parse_object(FRect & v, object o)
    {
        if (has_instance<PyFRect>(o.get()))
        {
            v = (o.cast<PyFRect>())->m_rc;
            return true;
        }
        else if (o.is_tuple())
        {
            tuple arg(o);
            if (arg.parse_tuple(&v.left, &v.top, &v.right, &v.bottom))
                return true;
        }

        PyErr_SetString(PyExc_TypeError, "Must be a FRect or tuple4!");
        return false;
    }
}