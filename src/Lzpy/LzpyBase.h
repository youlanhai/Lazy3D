#pragma once

#include "LzpyExtendHelper.h"
#include "LzpyExtendType.h"

///此宏用于声明导出类的部分公用属性和方法
#define LZPY_DEF_COMMON(ClassType, BaseType)                    \
public:                                                         \
    typedef BaseType  BaseClass;                                \
    typedef ClassType ThisClass;                                \
    static PyExtenClass<ClassType>  s_factory;                  \
    static void s_initExtens();                                 \
    bool init(PyObject *args, PyObject *kwds)

#define LZPY_DEF(ClassType) LZPY_DEF_COMMON(ClassType, ThisClass)

///python交互
namespace Lzpy
{

    ///导出基类
    class PyBase : public PyObject
    {
        LZPY_DEF_COMMON(PyBase, PyObject);
    public:
        PyBase();
        ~PyBase();
    };

    template<>
    inline void PyExtenClass<PyBase>::extenMethod()
    {
        if (isTypeReady())
            return;

        this->readyType();
    }

}// end namespace Lzpy
