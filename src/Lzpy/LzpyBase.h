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

        PyObject * pScript() { return m_object->getPScript(); }
        object script() { return m_object->getScript(); }

        Lazy::ScriptObject * get() { return m_object; }

    private:
        static PyObject *   PyGetAttr(PyObject * o, PyObject *attr_name);
        static int          PySetAttr(PyObject * o, PyObject *attr_name, PyObject * value);

        Lazy::ScriptObject * m_object;
    };


    template<typename TPy, typename TCxx>
    object build_script_object(TCxx * v)
    {
        if (nullptr == v)
            return none_object;

        return new_reference(PyScriptProxy::New(v));
    }

    template<typename TPy, typename TCxx>
    bool parse_script_object(TCxx *& v, object o)
    {
        if (o.is_none())
        {
            v = nullptr;
            return true;
        }

        if (has_instance<PyScriptProxy>(o.get()))
        {
            o = o.cast<PyScriptProxy>()->script();
        }

        if (!CHECK_INSTANCE(TPy, o.get()))
            return false;

        v = static_cast<TCxx*>(o.cast<TPy>()->m_object);
        return true;
    }

#define BUILD_AND_PARSE_SCRIPT_OBJECT(TYPE_PY, TYPE_CXX) \
    inline object build_object(TYPE_CXX * v)\
    {\
        return build_script_object<TYPE_PY, TYPE_CXX>(v); \
    }\
    inline bool parse_object(TYPE_CXX *& v, object o)\
    {\
        return parse_script_object<TYPE_PY, TYPE_CXX>(v, o); \
    }\
    inline object build_object(Lazy::RefPtr<TYPE_CXX> v)\
    {\
        return build_script_object<TYPE_PY, TYPE_CXX>(v.get()); \
    }\
    inline bool parse_object(Lazy::RefPtr<TYPE_CXX> & v, object o)\
    {\
        TYPE_CXX *p = nullptr; \
        bool ret = parse_script_object<TYPE_PY, TYPE_CXX>(p, o); \
        v = p; \
        return ret; \
    }

}// end namespace Lzpy
