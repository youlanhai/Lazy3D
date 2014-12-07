#pragma once

#include "../Physics/Math.h"
#include "../Lzpy/Lzpy.h"

namespace Lzpy
{
    using namespace Lazy;

    void exportPyPhsicis(const char * module);

    object build_object(const Vector2 & v);
    bool parse_object(Vector2 & v, object o);

    object build_object(const Vector3 & v);
    bool parse_object(Vector3 & v, object o);

    object build_object(const Vector4 & v);
    bool parse_object(Vector4 & v, object o);

    object build_object(const Quaternion & v);
    bool parse_object(Quaternion & v, object o);

    object build_object(const AABB & v);
    bool parse_object(AABB & v, object o);


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

    class PyVector4 : public PyBase
    {
        LZPY_DEF(PyVector4);

    public:
        PyVector4();

        static PyObject * reprfunc(PyObject * self);

        LZPY_DEF_METHOD(set);

        Vector4 m_vector;
    };

    class PyQuaternion : public PyBase
    {
        LZPY_DEF(PyQuaternion);

    public:
        PyQuaternion();

        static PyObject * reprfunc(PyObject * self);

        LZPY_DEF_METHOD(set);


        Quaternion m_value;
    };


    class PyAABB : public PyBase
    {
        LZPY_DEF(PyAABB);
    public:
        PyAABB();
        
        static PyObject * reprfunc(PyObject * self);

        LZPY_DEF_GETSET_MEMBER(min, m_aabb.min);
        LZPY_DEF_GETSET_MEMBER(max, m_aabb.max);

        AABB m_aabb;
    };

}
