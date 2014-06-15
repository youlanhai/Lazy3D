#pragma once

#include "../Math/Math.h"
#include "../Physics/Physics.h"
#include "../Lzpy/Lzpy.h"

namespace Lzpy
{
    class PyVector3 : public PyBase
    {
        LZPY_DEF(PyVector3, PyBase);

    public:
        PyVector3();

        static PyObject * reprfunc(PyObject * self);

        LZPY_DEF_METHOD(set);


        Math::Vector3 m_vector;
    };

    object build_object(const Math::Vector3 & v);
    bool parse_object(Math::Vector3 & v, object o);

    void exportPyPhsicis(const char * module);
}