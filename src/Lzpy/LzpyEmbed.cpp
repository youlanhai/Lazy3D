#include "StdAfx.h"
#include "LzpyEmbed.h"

namespace Lzpy
{
    PyObject* fromString(const std::wstring & str)
    {
        return PyUnicode_FromUnicode(str.c_str(), str.length());
    }

    object import(const std::wstring & name)
    {
        return new_reference( PyImport_Import( str(name).get() ) );
    }

    object import(const char *name)
    {
        return new_reference(PyImport_ImportModule(name));
    }

    void addSysPath(const std::wstring & path)
    {
        object sys = import("sys");
        list syspath = sys.getattr("path");
        syspath.append(path);
    }

    ////////////////////////////////////////////////////////////////////

    void fromPyObject(int & v, PyObject *pValue)
    {
        v = PyLong_AsLong(pValue);
    }

    void fromPyObject(unsigned int & v, PyObject *pValue)
    {
        v = PyLong_AsUnsignedLong(pValue);
    }

    void fromPyObject(long & v, PyObject *pValue)
    {
        v = PyLong_AsLong(pValue);
    }

    void fromPyObject(unsigned long & v, PyObject *pValue)
    {
        v = PyLong_AsUnsignedLong(pValue);
    }

    void fromPyObject(bool & v, PyObject *pValue)
    {
        v = PyObject_IsTrue(pValue) != 0;
    }

    void fromPyObject(float & v, PyObject *pValue)
    {
        v = (float) PyFloat_AsDouble(pValue);
    }

    void fromPyObject(double & v, PyObject *pValue)
    {
        v = PyFloat_AsDouble(pValue);
    }

    void fromPyObject(std::wstring & v, PyObject *pValue)
    {
        v = PyUnicode_AsUnicode(pValue);
    }

    void fromPyObject(std::string & v, PyObject *pValue)
    {
        v = PyUnicode_AsUTF8(pValue);
    }

    ////////////////////////////////////////////////////////////////////

    PyObject * toPyObject(PyObject *v)
    {
        if (!v) Py_RETURN_NONE;

        Py_IncRef(v);
        return v;
    }

    PyObject * toPyObject(int v)
    {
        return PyLong_FromLong(v);
    }

    PyObject * toPyObject(unsigned int v)
    {
        return PyLong_FromUnsignedLong(v);
    }

    PyObject * toPyObject(long v)
    {
        return PyLong_FromLong(v);
    }

    PyObject * toPyObject(unsigned long v)
    {
        return PyLong_FromUnsignedLong(v);
    }

    PyObject * toPyObject(bool v)
    {
        return PyBool_FromLong(v);
    }

    PyObject * toPyObject(float v)
    {
        return PyFloat_FromDouble((double) v);
    }

    PyObject * toPyObject(double v)
    {
        return PyFloat_FromDouble(v);
    }

    PyObject * toPyObject(const std::wstring & v)
    {
        return PyUnicode_FromUnicode(v.c_str(), v.size());
    }

    PyObject * toPyObject(const std::string & v)
    {
        return PyUnicode_FromStringAndSize(v.c_str(), v.size());
    }

    ////////////////////////////////////////////////////////////////////

    void copyPyObject(PyObject **ppDest, PyObject *pSrc)
    {
        assert(ppDest);
        if (*ppDest == pSrc) return;

        Py_XINCREF(pSrc);
        Py_XDECREF(*ppDest);
        *ppDest = pSrc;
    }
}