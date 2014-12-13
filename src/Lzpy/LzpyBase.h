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


    class PyScriptProxy : public PyBase
    {
        LZPY_DEF(PyScriptProxy);
    public:
        PyScriptProxy();
        ~PyScriptProxy();

        static PyScriptProxy * New(Lazy::ScriptObject *pObject);

        LZPY_DEF_GET(object, m_object->getSelf);

        LZPY_DEF_GET(script, m_object->getScript);
        LZPY_DEF_SET(script, m_object->setScript, object);

        PyObject * script() { return m_object->getPScript(); }

        Lazy::ScriptObject * get() { return m_object; }

    private:
        static PyObject *   PyGetAttr(PyObject * o, PyObject *attr_name);
        static int          PySetAttr(PyObject * o, PyObject *attr_name, PyObject * value);

        Lazy::ScriptObject * m_object;
    };

}// end namespace Lzpy
