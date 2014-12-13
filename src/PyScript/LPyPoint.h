#pragma once

#include "../utility/Tools2d.h"

namespace Lazy
{
    class PyPoint : public PyBase
    {
        LZPY_DEF(PyPoint);

    public:
        PyPoint();
        ~PyPoint();

        static PyObject * reprfunc(PyObject * self);
        LZPY_DEF_METHOD(set);

        CPoint m_pt;
    };

    object build_object(const CPoint & pt);
    bool parse_object(CPoint & pt, object o);

    class PyRect : public PyBase
    {
        LZPY_DEF(PyRect);

    public:
        PyRect();
        ~PyRect();

        static PyObject * reprfunc(PyObject * self);
        LZPY_DEF_METHOD(set);

        CRect m_rc;
    };

    object build_object(const CRect & pt);
    bool parse_object(CRect & pt, object o);

} //end namespace Lazy
