#include "stdafx.h"
#include "Test.h"

struct TestWrapper
{
    int value;

    TestWrapper()
        : value(0)
    {

    }

    void hello()
    {
        debugMessage(L"hello!");
    }

    void copy(object o)
    {
        extract<TestWrapper> t(o);
        if (!t.check())
        {
            debugMessage(L"must be %s", typeid(*this).name());
            return;
        }
        TestWrapper & x = extract<TestWrapper&>(o);
        value = x.value;
    }

};



void wrapperTest()
{
    class_<TestWrapper>("WrapperTest", init<>())
        .def_readwrite("value", &TestWrapper::value)
        .def("hello", &TestWrapper::hello)
        .def("copy", &TestWrapper::copy)
        ;
}