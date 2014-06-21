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

        ///导出类内存分配
        template<typename class_type>
        static PyObject* tp_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
        {
            class_type* self = (class_type*) type->tp_alloc(type, 0);
            if (self == nullptr) return nullptr;

            new (self) class_type();
            return self;
        }

        ///导出类内存回收
        template<typename class_type>
        static void tp_dealloc(class_type* self)
        {
            self->~class_type();
            Py_TYPE(self)->tp_free((PyObject*) self);
        }

        ///导出类构造方法
        template<typename class_type>
        static int tp_init(class_type *self, PyObject *args, PyObject *kwds)
        {
            bool ret = self->init(args, kwds);
            return ret ? 0 : -1;
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

        template<typename class_type>
        bool is_ready()
        {
            return py_type<class_type>()->tp_dict != nullptr;
        }

        template<typename class_type>
        inline void ready_type()
        {
            if (!is_ready<class_type::BaseClass>())
                ready_type<class_type::BaseClass>();

            class_type::s_factory.readyType();
        }

        class PyBase;

        template<>
        inline void ready_type<PyBase>();


        ///创建python实例，并调用python的__init__方法。
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

        ///创建python实例，不调用python的__init__方法。
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

    }

    ///用于导出的接口
    class PyExtenInterface
    {
    public:
        ///在导出的时候，会执行此对象的extenMethod方法。
        PyExtenInterface();
        virtual ~PyExtenInterface();

        void registerForExten(const std::string & mod, const std::string & cls);

        ///导出的时候，会执行此方法。
        virtual void extenMethod() = 0;

        ///真正执行导出
        virtual void readyType() = 0;

        virtual bool isTypeReady() = 0;

        ///类类型
        PyTypeObject* pytype() { return &pytype_; }

        ///添加类方法
        void addMethod(const char* name, PyCFunction fun);
        void addMethod(const char *name, PyCFunction fun, int meth, const char *doc);

        ///添加类成员变量
        void addMember(const char *name, int type, Py_ssize_t offset);
        void addMember(const char *name, int type, Py_ssize_t offset, int flags, char *doc);

        ///添加类属性
        void addGetSet(const char *name, getter get, setter set);
        void addGetSet(const char *name, getter get, setter set, char *doc, void *closure);

    protected:
        PyTypeObject pytype_;
        std::vector<PyMethodDef> methods_;
        std::vector<PyMemberDef> members_;
        std::vector<PyGetSetDef> getsets_;

        std::string cls_;///<模块名称
        std::string mod_;///<导出类名称
    };

    ///用于导出类
    template<class Type>
    class PyExtenClass : public PyExtenInterface
    {
    public:
        typedef Type ThisType;

        PyExtenClass()
        {}

        ~PyExtenClass()
        {}

        virtual void extenMethod() override
        {
            helper::ready_type<ThisType>();
        }

        virtual bool isTypeReady() override
        {
            return helper::is_ready<ThisType>();
        }

        virtual void readyType() override;

    };


    template<class Type>
    void PyExtenClass<Type>::readyType()
    {
        if (isTypeReady()) return;

#ifdef ENABLE_EXPORT_MSG
        Lazy::debugMessageA("Export class %s.%s", mod_.c_str(), cls_.c_str());
#endif

        PyObject *module = helper::getModule(mod_);
        assert(module);

        PyTypeObject t =
        {
            PyVarObject_HEAD_INIT(NULL, 0)
            cls_.c_str(),  /* tp_name */
            sizeof(ThisType),
            0,
        };
        pytype_ = t;
        pytype_.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
        pytype_.tp_new = helper::tp_new<ThisType>;
        pytype_.tp_init = (initproc) helper::tp_init<ThisType>;
        pytype_.tp_dealloc = (destructor) helper::tp_dealloc<ThisType>;
        pytype_.tp_base = helper::py_type<ThisType::BaseClass>();

        //We can do something in this function.
        ThisType::s_initExtens();

        PyMethodDef endMethod = { NULL, NULL, 0, NULL };
        methods_.push_back(endMethod);
        pytype_.tp_methods = &methods_[0];

        PyMemberDef endMember = { NULL, 0, 0, 0, NULL };
        members_.push_back(endMember);
        pytype_.tp_members = &members_[0];

        PyGetSetDef endGetSet = { NULL, NULL, NULL, NULL, NULL };
        getsets_.push_back(endGetSet);
        pytype_.tp_getset = &getsets_[0];

        if (PyType_Ready(&pytype_) < 0)
            throw(python_error("Initialize Extern Class Type failed!"));

        Py_INCREF(&pytype_);
        PyModule_AddObject(module, cls_.c_str(), (PyObject *) &pytype_);
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
    (::Lzpy::helper::has_instance<CLASS>(INSTANCE, true) || \
    _check_instance("CHECK_INSTANCE(" #CLASS ", " #INSTANCE ")", __FUNCTION__, __FILE__, __LINE__))

#define CHECK_SUBCLASS(CLASS, SUBCLASS) \
    (::Lzpy::helper::has_sub_class<CLASS>(SUBCLASS, true) || \
    _check_instance("CHECK_SUBCLASS(" #CLASS ", " #SUBCLASS ")", __FUNCTION__, __FILE__, __LINE__))
