#pragma once

#ifndef LAZY_PY_WRAPPER_H
#define LAZY_PY_WRAPPER_H

#include "LzpyTuple.h"
#include "LzpyStr.h"
#include "LzpyList.h"
#include "LzpyDict.h"

namespace Lzpy
{
    PyObject* fromString(const std::wstring & str);
    object import(const std::wstring & str);
    object import(const char *name);
    void addSysPath(const std::wstring & path);

    ////////////////////////////////////////////////////////////////////

    template<class T>
    T fromPyObject(PyObject *pValue)
    {
        T v;
        fromPyObject(v, pValue);
        return v;
    }

    void fromPyObject(int & v, PyObject *pValue);

    void fromPyObject(unsigned int & v, PyObject *pValue);

    void fromPyObject(size_t & v, PyObject *pValue);

    void fromPyObject(long & v, PyObject *pValue);

    void fromPyObject(unsigned long & v, PyObject *pValue);

    void fromPyObject(bool & v, PyObject *pValue);

    void fromPyObject(float & v, PyObject *pValue);

    void fromPyObject(double & v, PyObject *pValue);

    void fromPyObject(std::wstring & v, PyObject *pValue);

    void fromPyObject(std::string & v, PyObject *pValue);

    ////////////////////////////////////////////////////////////////////
    
    PyObject * toPyObject(PyObject *v);

    PyObject * toPyObject(int v);

    PyObject * toPyObject(unsigned int v);

    PyObject * toPyObject(size_t v);

    PyObject * toPyObject(long v);

    PyObject * toPyObject(unsigned long v);

    PyObject * toPyObject(bool v);

    PyObject * toPyObject(float v);

    PyObject * toPyObject(double v);

    PyObject * toPyObject(const std::wstring & v);

    PyObject * toPyObject(const std::string & v);

    ////////////////////////////////////////////////////////////////////

    void copyPyObject(PyObject **ppDest, PyObject *pSrc);
}

#endif //LAZY_PY_WRAPPER_H
