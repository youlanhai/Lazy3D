// Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include "utility/Utility.h"
#include "Lzpy/Lzpy.h"

///////////////////////////////////////////////////////////////////////
//导出函数
///////////////////////////////////////////////////////////////////////
//1.定义导出函数
static PyObject* print_msg(PyObject* self, PyObject* argv)
{
    wchar_t* msg;
    if (!PyArg_ParseTuple(argv, "u", &msg)) return NULL;

    std::wcout << "python:"<< msg << std::endl;
    Py_RETURN_NONE;
}

//2.定义函数列表
static PyMethodDef pymethods[] = {
    {"print", print_msg, METH_VARARGS, "print a msg."},
    {NULL, NULL, 0, NULL}
};

//3.定义导出模块
static PyModuleDef pymodule = {
    PyModuleDef_HEAD_INIT,
    "Lzpy",
    NULL,
    -1,
    pymethods,
    NULL, NULL, NULL, NULL
};

//4.定义导出入口
PyMODINIT_FUNC PyInit_LazyPy(void)
{
    PyObject* module = PyModule_Create(&pymodule);
    return module;
}

///////////////////////////////////////////////////////////////////////
//导出类
///////////////////////////////////////////////////////////////////////
#include "structmember.h"

//1.声明导出类
typedef struct {
    PyObject_HEAD
        PyObject *first; /* first name */
    PyObject *last;  /* last name */
    int number;
} Noddy;

//析构函数
static void
    Noddy_dealloc(Noddy* self)
{
    Py_XDECREF(self->first);
    Py_XDECREF(self->last);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

//内存分配函数。
static PyObject *
    Noddy_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Noddy *self;

    self = (Noddy *) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->first = PyUnicode_FromString("");
        if (self->first == NULL) {
            Py_DECREF(self);
            return NULL;
        }

        self->last = PyUnicode_FromString("");
        if (self->last == NULL) {
            Py_DECREF(self);
            return NULL;
        }

        self->number = 0;
    }

    return (PyObject *) self;
}

//构造函数
static int
    Noddy_init(Noddy *self, PyObject *args, PyObject *kwds)
{
    PyObject *first = NULL, *last = NULL, *tmp;

    static char *kwlist [] = { "first", "last", "number", NULL };

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OOi", kwlist,
        &first, &last,
        &self->number))
        return -1;

    if (first) {
        tmp = self->first;
        Py_INCREF(first);
        self->first = first;
        Py_XDECREF(tmp);
    }

    if (last) {
        tmp = self->last;
        Py_INCREF(last);
        self->last = last;
        Py_XDECREF(tmp);
    }

    return 0;
}

//导出成员表
static PyMemberDef Noddy_members [] = {
    {"first", T_OBJECT_EX, offsetof(Noddy, first), 0, "first name"},
    { "last", T_OBJECT_EX, offsetof(Noddy, last), 0, "last name" },
    { "number", T_INT, offsetof(Noddy, number), 0, "noddy number" },
    { NULL }  /* Sentinel */
};


static PyObject * Noddy_name(Noddy* self)
{
    if (self->first == NULL) {
        PyErr_SetString(PyExc_AttributeError, "first");
        return NULL;
    }

    if (self->last == NULL) {
        PyErr_SetString(PyExc_AttributeError, "last");
        return NULL;
    }

    return PyUnicode_FromFormat("%S %S", self->first, self->last);
}

//定义导出方法表
static PyMethodDef Noddy_methods [] = {
    {"name", (PyCFunction) Noddy_name, METH_NOARGS, "Return the name, combining the first and last name" },
    { NULL }  /* Sentinel */
};

//定义导出类的类型
static PyTypeObject NoddyType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "noddy.Noddy",             /* tp_name */
    sizeof(Noddy),             /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor) Noddy_dealloc, /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
    Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "Noddy objects",           /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    Noddy_methods,             /* tp_methods */
    Noddy_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc) Noddy_init,      /* tp_init */
    0,                         /* tp_alloc */
    Noddy_new,                 /* tp_new */
};

//定义导出模块
static PyModuleDef noddy2module = {
    PyModuleDef_HEAD_INIT,
    "noddy2",
    "Example module that creates an extension type.",
    -1,
    NULL, NULL, NULL, NULL, NULL
};

//定义导出入口
PyMODINIT_FUNC PyInit_noddy2(void)
{
    PyObject* m;

    if (PyType_Ready(&NoddyType) < 0)
        return NULL;

    m = PyModule_Create(&noddy2module);
    if (m == NULL) return NULL;

    Py_INCREF(&NoddyType);
    PyModule_AddObject(m, "Noddy", (PyObject *) &NoddyType);
    return m;
}

void testCase(const char * name, bool ret)
{
    if (ret) std::cout << "OK: " << name << std::endl;
    else
    {
        std::cout << "Failed: " << name << std::endl;
        exit(1);
    }
}

void doTest();
void doTestObject();

int _tmain(int argc, _TCHAR* argv[])
{
    Py_SetPythonHome(L"../../third_party/python33");
    PyImport_AppendInittab("Lzpy", PyInit_LazyPy);
    PyImport_AppendInittab("noddy2", PyInit_noddy2);
    PyImport_AppendInittab("helper", Lzpy::PyInit_helper);
    Py_SetProgramName(argv[0]);

    Py_Initialize();

    try
    {
        doTestObject();
        doTest();
    }
    catch (std::exception e)
    {
        std::cout << "excption: " << e.what() << std::endl;
    }

    Py_Finalize();
	return 0;
}

void doTestObject()
{
    using namespace Lzpy;

    //testCase("test false", false);

    std::wstring hello(L"你好，中文！");
    tuple args = build_tuple(1, 1.2f, 3.14, "hello world", hello);
    
    int a;
    float b;
    double c;
    char * d;
    std::wstring e;
    args.parse_tuple(&a, &b, &c, &d, &e);

    std::cout << a << " "
        << b << " "
        << c << " "
        << d << " "
        //<< e << " "
        << std::endl;

    testCase("parse_tuple wstring", e == hello);

    //object x1 = 1; //语法错误，不能隐式构造
    int x1 = 9999;
    object x2 = build_object(x1);
    testCase("object.parse", parse_object<int>(x2) == x1);
    //x2.parse<char>(); //语法错误，不支持的类型
    //x2.parse<const char *>(); //语法错误，不支持的类型

    int x3;
    parse_object(x3, x2);
    testCase("parse_object", x3 == x1);

    std::string x4("string.haha!");
    object x5 = build_object(x4);
    testCase("object.build", parse_object<char*>(x5) == x4);

    float x6 = 3.14f;
    object x7 = build_object(x6);
    testCase("build_object", parse_object<float>(x7) == x6);

    int x8 = 1234;
    object x9 = build_object("hello str");
    testCase("parse_object wrong", parse_object(x8, x9) == false);

    try
    {
        object x9 = build_object(123);
        std::string x10 = parse_object<std::string>(x9);
    }
    catch (python_error e)
    {
        std::cout << "python error: "<< e.what() << std::endl;
    }
}

void doTest()
{
    Lzpy::addSysPath(L"./");

#if 0
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("print(sys.path)");
#endif

    Lzpy::object module = Lzpy::import(L"LazyTest");
    if (module)
    {
#if 0
        Lzpy::object testFun = module.getattr(L"doTest");
        if (testFun) testFun();
        else std::cout << "doTest was not found!" << std::endl;
#else
        module.call_method("doTest");
#endif
        Lzpy::object test2 = module.getattr("test2");
        if (test2)
        {
            test2.call(L"hello world!");
        }
        else std::cout << "test2 was not found!" << std::endl;
    }
    else
    {
        std::wcout << L"import LazyTest failed!" << std::endl;
    }

    if (PyErr_Occurred())
        PyErr_Print();

}