#pragma once


///此宏用于声明导出类的部分公用属性和方法
#define LZPY_DEF(ClassType, BaseType)                  \
public:                                                         \
    typedef ClassType ThisClass;                                \
    typedef BaseType  BaseClass;                                \
    static PyExtenClass<ClassType>  s_factory;                  \
    static void s_initExtens();                                 \
    bool init(PyObject *args, PyObject *kwds)

////////////////////////////////////////////////////////////////////////
// 链接pytype的额外属性
////////////////////////////////////////////////////////////////////////

///链接额外属性
#define LZPY_LINK_ATTR(NAME, FUN) \
    helper::py_type<ThisClass>()->NAME = FUN

///定义init方法
#define LZPY_IMP_INIT(CLASS) \
    bool CLASS::init(PyObject *arg, PyObject *kwd)

////////////////////////////////////////////////////////////////////////
// 导出方法声明
////////////////////////////////////////////////////////////////////////

///导出方法声明，任意参数
#define LZPY_DEF_METHOD(NAME)                           \
    static PyObject* _wraper_py_##NAME(                 \
        PyObject *self, PyObject *arg)                  \
    {                                                   \
        tuple o_arg(arg);                               \
        object o_ret = ((ThisClass*) self)->py_##NAME(o_arg);\
        return xincref(o_ret.get());                    \
    }                                                   \
    object py_##NAME(const tuple & arg)

///导出方法声明，没有参数
#define LZPY_DEF_METHOD_0(NAME)                         \
    static PyObject * _wraper_py_##NAME##_0(            \
        PyObject *self)                                 \
    {                                                   \
        object o_ret = ((ThisClass*) self)->py_##NAME(); \
        return xincref(o_ret.get());                    \
    }                                                   \
    object py_##NAME()

///导出方法声明，只有一个参数
#define LZPY_DEF_METHOD_1(NAME)                         \
    static PyObject * _wraper_py_##NAME##_1(            \
        PyObject *self, PyObject *value)                \
    {                                                   \
        object o_arg(value);                            \
        object o_ret = ((ThisClass*) self)->py_##NAME(o_arg); \
        return xincref(o_ret.get());                    \
    }                                                   \
    object py_##NAME(const object & value)

///导出方法定义，多参数
#define LZPY_IMP_METHOD(CLASS, NAME)                  \
    object CLASS::py_##NAME(const tuple & arg)

///导出方法定义，无参数
#define LZPY_IMP_METHOD_0(CLASS, NAME)                \
    object CLASS::py_##NAME()

///导出方法定义，一个参数
#define LZPY_IMP_METHOD_1(CLASS, NAME)                \
    object CLASS::py_##NAME(const object & value)


////////////////////////////////////////////////////////////////////////
// 导出property声明
// 如果类型无法直接转换为object，有两种解决办法：
// 1.重载build_object/parse_object
// 2.get set方法的返回值/参数设为object类型，自己手动设置object.m_ptr。
////////////////////////////////////////////////////////////////////////

///声明读属性
#define LZPY_DEF_GET(NAME, FUN)                       \
    static PyObject * _wraper_get_##NAME(               \
        ThisClass *self, void*)                         \
    {                                                   \
        object ret = build_object(self->FUN());         \
        return xincref(ret.get());                      \
    }

///声明写属性
#define LZPY_DEF_SET(NAME, FUN, TYPE)                   \
    static int _wraper_set_##NAME(                      \
        ThisClass *self, PyObject *value, void*)        \
    {                                                   \
        TYPE v;                                         \
        if (!parse_object(v, object(value)))            \
        {                                               \
            LOG_ERROR(L"Script Error: set property failed!", #NAME);  \
            return -1;                                  \
        }                                               \
        self->FUN(v);                                   \
        return 0;                                       \
    }

#define LZPY_DEF_GETSET(NAME, FUN, TYPE)              \
    LZPY_DEF_GET(NAME, FUN)                           \
    LZPY_DEF_SET(NAME, FUN, TYPE)

#define LZPY_DEF_GET_MEMBER(NAME, MEMBER)             \
    static PyObject * _wraper_get_##NAME(               \
        ThisClass *self, void*)                         \
    {                                                   \
        object ret = build_object(self->MEMBER);        \
        return xincref(ret.get());                      \
    }

#define LZPY_DEF_SET_MEMBER(NAME, MEMBER)               \
    static int _wraper_set_##NAME(                      \
        ThisClass *self, PyObject *value, void*)        \
    {                                                   \
        if (!parse_object(self->MEMBER, object(value))) return -1;        \
        return 0;                                       \
    }

#define LZPY_DEF_GETSET_MEMBER(NAME, MEMBER)          \
    LZPY_DEF_GET_MEMBER(NAME, MEMBER)                 \
    LZPY_DEF_SET_MEMBER(NAME, MEMBER)

////////////////////////////////////////////////////////////////////////
// 导出成员
////////////////////////////////////////////////////////////////////////

///导出方法开始
#define LZPY_CLASS_BEG(ClassType)               \
    PyExtenClass<ClassType>  ClassType::s_factory;  \
    void ClassType::s_initExtens(void)              \
    {                                               \
        typedef ClassType ThisClass;

///导出方法
#define LZPY_METHOD(NAME) \
    ThisClass::s_factory.addMethod(#NAME, (PyCFunction) ThisClass::_wraper_py_##NAME)

#define LZPY_METHOD_0(NAME) \
    ThisClass::s_factory.addMethod(#NAME, (PyCFunction) ThisClass::_wraper_py_##NAME##_0, METH_NOARGS, "")

#define LZPY_METHOD_1(NAME) \
    ThisClass::s_factory.addMethod(#NAME, (PyCFunction) ThisClass::_wraper_py_##NAME##_1, METH_O, "")


///导出成员
#define LZPY_MEMEBER(NAME, Type, Var)  \
    ThisClass::s_factory.addMember(#NAME, Type, offsetof(ThisClass, Var))

///导出成员
#define LZPY_MEMEBER_FLAG(NAME, Type, Var, Flag)  \
    ThisClass::s_factory.addMember(#NAME, Type, offsetof(ThisClass, Var), Flag, "")


///导出属性
#define LZPY_GETSET(NAME)             \
    ThisClass::s_factory.addGetSet(     \
        #NAME,                          \
        (getter) ThisClass::_wraper_get_##NAME, \
        (setter) ThisClass::_wraper_set_##NAME)

///导出只读属性
#define LZPY_GET(NAME)             \
    ThisClass::s_factory.addGetSet( #NAME, (getter)ThisClass::_wraper_get_##NAME, NULL)

///导出只写属性
#define LZPY_SET(NAME)             \
    ThisClass::s_factory.addGetSet( #NAME, NULL, (setter)ThisClass::_wraper_set_##NAME)


///导出方法结束
#define LZPY_CLASS_END()   }


////////////////////////////////////////////////////////////////////////
// 导出模块
////////////////////////////////////////////////////////////////////////

///声明导出模块
#define LZPY_DEF_MODULE(MODULE)  \
    PyMODINIT_FUNC PyInit_##MODULE(void);

///定义导出模块
#define LZPY_MODULE_BEG(MODULE)                   \
    PyMODINIT_FUNC PyInit_##MODULE(void)            \
    {                                               \
        const char * module = #MODULE;

#define LZPY_REGISTER_CLASS(PY_CLASS, CXX_CLASS) \
    CXX_CLASS::s_factory.registerForExten(module, #PY_CLASS)

#define LZPY_REGISTER_CLASS_EX(PY_CLASS, CXX_CLASS, PY_MODULE) \
    CXX_CLASS::s_factory.registerForExten(#PY_MODULE, #PY_CLASS)

#define LZPY_MODULE_END() \
        return helper::extenModule(module);        \
    }


////////////////////////////////////////////////////////////////////////
// 导出模块函数
////////////////////////////////////////////////////////////////////////

#define LZPY_FUN_NAME(NAME) _wraper_fun##NAME

///定义函数，多参数
#define LZPY_DEF_FUN(NAME) \
    static PyObject * LZPY_FUN_NAME(NAME)(PyObject * self, PyObject *arg)

///定义函数，没有参数
#define LZPY_DEF_FUN_0(NAME) \
    static PyObject * _wraper_fun##NAME##_0(PyObject * self)

///定义函数，一个参数
#define LZPY_DEF_FUN_1(NAME) \
    static PyObject * _wraper_fun##NAME##_1(PyObject * self, PyObject *value)

///开始导出函数
#define LZPY_EXTEN_FUN(MODULE, NAME)  \
static struct Fun_##MODULE##_##NAME     \
{                                       \
    Fun_##MODULE##_##NAME()             \
    {                                   \
        pyExtenFunction(#MODULE, #NAME, _wraper_fun##NAME); \
    }                                   \
}  s_extendFun_##MODULE##_##NAME

///开始导出函数
#define LZPY_BEGIN_FUN(MODULE)            \
namespace {                                 \
    struct BeginExtenFun {                  \
        BeginExtenFun() {                   \
        const char *module = #MODULE;

///导出函数，多参数
#define LZPY_FUN(NAME) \
    pyExtenFunction(module, #NAME, _wraper_fun##NAME)

///导出函数，没有参数
#define LZPY_FUN_0(NAME) \
    pyExtenFunction(module, #NAME, (PyCFunction) _wraper_fun##NAME##_0, METH_NOARGS, "")

///导出函数，一个参数
#define LZPY_FUN_1(NAME) \
    pyExtenFunction(module, #NAME, (PyCFunction) _wraper_fun##NAME##_1, METH_O, "")

///导出函数结束
#define LZPY_END_FUN()                    \
        }/*end contruct.*/                  \
    };/*end class*/                         \
    static BeginExtenFun s_extenFunFactory; \
}/*end namespace*/