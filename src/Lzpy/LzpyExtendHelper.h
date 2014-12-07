#pragma once

#include "LzpyEmbed.h"

namespace Lzpy
{

    ///辅助方法
    namespace helper
    {
        ///缓存字符串，防止字符串析构。
        char* cacheName(const std::string & name);

        ///获取模块。如果模块不存在，会自动创建。
        PyObject* getModule(const std::string & modulename);

        ///导出模块内容
        PyObject* extenModule(const std::string & modulename);

    }

    ///C++包装类的python类型
    template<typename class_type>
    inline PyTypeObject * py_type()
    {
        return class_type::s_factory.pytype();
    }

    template<>
    inline PyTypeObject * py_type<PyObject>()
    {
        return &PyBaseObject_Type;
    }


    /** 创建python实例，并调用python的__init__方法。*/
    template < typename class_type>
    class_type* new_instance(PyObject * args = nullptr, PyObject *kwds = nullptr)
    {
        PyTypeObject * type = py_type<class_type>();
        PyObject *p = type->tp_new(type, nullptr, nullptr);
        if (!p) return nullptr;

        if (type->tp_init(p, args, kwds) < 0)
        {
            type->tp_dealloc(p);
            return nullptr;
        }
        return (class_type*) p;
    }

    /** 创建python实例，不调用python的__init__方法。*/
    template < typename class_type>
    class_type* new_instance_ex()
    {
        PyTypeObject *type = py_type<class_type>();
        PyObject *p = type->tp_new(type, nullptr, nullptr);
        if (!p) return nullptr;

        return (class_type*) p;
    }

    template<typename class_type>
    bool has_instance(PyObject *p, bool set_error = false)
    {
        PyTypeObject * pType = py_type<class_type>();
        bool ret(PyObject_IsInstance(p, (PyObject*) pType) != 0);

        if (!ret && set_error)
        {
            std::string desc = object_base(p).repr_a();

            std::string msg;
            Lazy::formatStringA(msg, "Type %s needed, but %s was given.", typeid(class_type).name(), desc.c_str());

            PyErr_SetString(PyExc_TypeError, msg.c_str());
            Lazy::debugMessageA("Script Error - has_instance: ", msg.c_str());
        }

        return ret;
    }

    template<typename class_type>
    bool has_sub_class(PyObject * p, bool set_error = false)
    {
        bool ret(PyObject_IsSubclass(p, (PyObject*) py_type<class_type>()) != 0);

        if (!ret && set_error)
        {
            std::string desc = object_base(p).repr_a();

            std::string msg;
            Lazy::formatStringA(msg, "Type %s needed, but %s was given.", typeid(class_type).name(), desc.c_str());

            PyErr_SetString(PyExc_TypeError, msg.c_str());
            Lazy::debugMessageA("Script Error - has_sub_class: ", msg.c_str());
        }

        return ret;
    }

    ///用于导出模块的函数
    void pyExtenFunction(const char *modulename, const char *funname, PyCFunction func);
    void pyExtenFunction(const char *modulename, const char *funname,
                         PyCFunction func, int argFlag, char *doc);



    /** py资源管理接口。所有派生类实例，都会自动加入管理池。
     *  在python虚拟机初始化后会调用init方法，虚拟机卸载之前调用fini。
     */
    class LzpyResInterface
    {
    public:
        LzpyResInterface();
        virtual ~LzpyResInterface();

        virtual void init() = 0;
        virtual void fini() = 0;

    public:

        ///在python虚拟机初始化完毕后，必须调用此方法
        static void initAll();

        ///python虚拟机卸载之前，必须调用此方法
        static void finiAll();

    };

}

inline bool _check_instance(const char * desc, const char * func, const char * file, int line)
{
    ::Lazy::debugMessageA("Script Error: %s, func: %s, line: %d, file: %s.",
                          desc, func, line, file);
    return false;
}

#define CHECK_INSTANCE(CLASS, INSTANCE) \
    (::Lzpy::has_instance<CLASS>(INSTANCE, true) || \
    _check_instance("CHECK_INSTANCE(" #CLASS ", " #INSTANCE ")", __FUNCTION__, __FILE__, __LINE__))

#define CHECK_SUBCLASS(CLASS, SUBCLASS) \
    (::Lzpy::has_sub_class<CLASS>(SUBCLASS, true) || \
    _check_instance("CHECK_SUBCLASS(" #CLASS ", " #SUBCLASS ")", __FUNCTION__, __FILE__, __LINE__))
