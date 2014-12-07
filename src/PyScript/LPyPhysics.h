#pragma once

#include "../Physics/Math.h"
#include "../Lzpy/Lzpy.h"

namespace Lzpy
{
    using namespace Lazy;

    class PyVector2 : public PyBase
    {
        LZPY_DEF(PyVector2);

    public:
        PyVector2();

        static PyObject * reprfunc(PyObject * self);

        LZPY_DEF_METHOD(set);


        Vector2 m_vector;
    };

    class PyVector3 : public PyBase
    {
        LZPY_DEF(PyVector3);

    public:
        PyVector3();

        static PyObject * reprfunc(PyObject * self);

        LZPY_DEF_METHOD(set);


        Vector3 m_vector;
    };

    object build_object(const Vector2 & v);
    bool parse_object(Vector2 & v, object o);

    object build_object(const Vector3 & v);
    bool parse_object(Vector3 & v, object o);

    void exportPyPhsicis(const char * module);
}