#pragma once

namespace LazyPy
{
    class PyVector3 : public PyBase
    {
        LAZYPY_DEF(PyVector3, PyBase);

    public:
        PyVector3();

        static PyObject * reprfunc(PyObject * self);

        LAZYPY_DEF_METHOD(set);


        Math::Vector3 m_vector;
    };

    object build_object(const Math::Vector3 & v);
    bool parse_object(Math::Vector3 & v, object o);
}