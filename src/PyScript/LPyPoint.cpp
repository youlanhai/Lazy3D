#include "stdafx.h"
#include "LPyPoint.h"

#include <sstream>

namespace Lazy
{
    LZPY_CLASS_EXPORT(PyPoint)
    {
        LZPY_MEMEBER(x, T_INT, m_pt.x);
        LZPY_MEMEBER(x, T_INT, m_pt.y);

        LZPY_METHOD(set);
    }

    LZPY_IMP_INIT(PyPoint)
    {
        return true;
    }

    PyPoint::PyPoint()
    {
    }


    PyPoint::~PyPoint()
    {
    }

    /*static*/ PyObject * PyPoint::reprfunc(PyObject * self)
    {
        PyPoint * pThis = (PyPoint*) self;

        std::wostringstream os;
        os << pThis->m_pt.x << ", " << pThis->m_pt.y;
        std::wstring str = os.str();
        return PyUnicode_FromWideChar(str.c_str(), str.size());
    }

    LZPY_IMP_METHOD(PyPoint, set)
    {
        if (!arg.parse_tuple(&m_pt.x, &m_pt.y))
            return null_object;

        return none_object;
    }

    object build_object(const CPoint & pt)
    {
        PyPoint * p = new_instance_ex<PyPoint>();
        p->m_pt = pt;
        return new_reference(p);
    }

    bool parse_object(CPoint & v, object o)
    {
        if (has_instance<PyPoint>(o.get()))
        {
            v = (o.cast<PyPoint>())->m_pt;
            return true;
        }
        else if (o.is_tuple())
        {
            tuple arg(o);
            if (arg.parse_tuple(&v.x, &v.y))
                return true;
        }

        PyErr_SetString(PyExc_TypeError, "Must be a Point or tuple2!");
        return false;
    }
    //////////////////////////////////////////////
    ///
    //////////////////////////////////////////////
    LZPY_CLASS_EXPORT(PyRect)
    {
        LZPY_MEMEBER(left, T_INT, m_rc.left);
        LZPY_MEMEBER(top, T_INT, m_rc.top);

        LZPY_MEMEBER(right, T_INT, m_rc.right);
        LZPY_MEMEBER(bottom, T_INT, m_rc.bottom);

        LZPY_METHOD(set);
    }

    LZPY_IMP_INIT(PyRect)
    {
        return true;
    }

    PyRect::PyRect()
    {
    }


    PyRect::~PyRect()
    {
    }

    /*static*/ PyObject * PyRect::reprfunc(PyObject * self)
    {
        PyRect * pThis = (PyRect*) self;

        std::wostringstream os;
        os << pThis->m_rc.left << ", " << pThis->m_rc.top << ", "
            << pThis->m_rc.right << ", " << pThis->m_rc.bottom;

        std::wstring str = os.str();
        return PyUnicode_FromWideChar(str.c_str(), str.size());
    }

    LZPY_IMP_METHOD(PyRect, set)
    {
        if (!arg.parse_tuple(&m_rc.left, &m_rc.top, &m_rc.right, &m_rc.bottom))
            return null_object;

        return none_object;
    }

    object build_object(const CRect & pt)
    {
        PyRect * p = new_instance_ex<PyRect>();
        p->m_rc = pt;
        return new_reference(p);
    }

    bool parse_object(CRect & v, object o)
    {
        if (has_instance<PyRect>(o.get()))
        {
            v = (o.cast<PyRect>())->m_rc;
            return true;
        }
        else if (o.is_tuple())
        {
            tuple arg(o);
            if (arg.parse_tuple(&v.left, &v.top, &v.right, &v.bottom))
                return true;
        }

        PyErr_SetString(PyExc_TypeError, "Must be a Rect or tuple4!");
        return false;
    }

}// end namespace Lazy
