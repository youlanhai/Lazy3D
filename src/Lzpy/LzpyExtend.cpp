#include "stdafx.h"
#include "LzpyExtend.h"
#include "LzpyOutput.h"

namespace Lzpy
{
    ////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////

    LZPY_CLASS_BEG(PyTestClass);
    LZPY_METHOD_0(test0);
    LZPY_METHOD_1(test1);
    LZPY_METHOD(test2);
    LZPY_METHOD(hello);
    LZPY_MEMEBER(data, T_INT, mydata);
    LZPY_GETSET(testId);
    LZPY_CLASS_END();

    PyTestClass::PyTestClass()
        : mydata(11)
    {}

    LZPY_IMP_INIT(PyTestClass)
    {
        return true;
    }

    LZPY_IMP_METHOD(PyTestClass, hello)
    {
        wchar_t *msg;
        if (!arg.parse_tuple(&msg)) return null_object;

        std::wcout << L"c++ to python[" << mydata << L"]: " << msg << std::endl;

        return none_object;
    }

    LZPY_IMP_METHOD_0(PyTestClass, test0)
    {
        debugMessage(L"Test 0, no arg.");

        return none_object;
    }

    LZPY_IMP_METHOD_1(PyTestClass, test1)
    {
        if (!value.is_str()) return null_object;

        debugMessage(L"Test 1, 1 arg. %s", parse_object<wchar_t*>(value));

        return none_object;
    }

    LZPY_IMP_METHOD(PyTestClass, test2)
    {
        size_t len = arg.size();
        debugMessage(L"Test 2, %u args.", len);

        return none_object;
    }

    ////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////

    LZPY_DEF_FUN_0(hello)
    {
        std::wcout << L"lazyHello: Hello world!" << std::endl;
        Py_RETURN_NONE;
    }

    LZPY_DEF_FUN(debugMsg)
    {
        wchar_t *msg;
        if (!PyArg_ParseTuple(arg, "u", &msg)) return NULL;

        OutputDebugStringW(msg);

        Py_RETURN_NONE;
    }


    LZPY_DEF_FUN_0(test0)
    {
        debugMessage(L"Test 0, no arg.");
        Py_RETURN_NONE;
    }

    LZPY_DEF_FUN_1(test1)
    {
        str s(value);
        debugMessage(L"Test 1, 1 arg. %s", s.c_str());

        Py_RETURN_NONE;
    }

    LZPY_DEF_FUN(test2)
    {
        size_t len = PyTuple_Size(arg);
        debugMessage(L"Test 2, %u args.", len);

        Py_RETURN_NONE;
    }


    LZPY_MODULE_BEG(helper);
    LZPY_REGISTER_CLASS(Base, PyBase);
    LZPY_REGISTER_CLASS(TestClass, PyTestClass);
    LZPY_REGISTER_CLASS(Output, PyOutput);
    LZPY_FUN_0(hello);
    LZPY_FUN_0(test0);
    LZPY_FUN_1(test1);
    LZPY_FUN(test2);
    LZPY_FUN(debugMsg);
    LZPY_MODULE_END();
}