#pragma once

namespace Lzpy
{

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

        bool isTypeReady() { return pytype_.tp_dict != nullptr; }

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
        typedef Type class_type;

        PyExtenClass()
        {}

        ~PyExtenClass()
        {}

        virtual void extenMethod() override;
        virtual void readyType() override;


        ///导出类内存分配
        static PyObject* tp_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
        {
            class_type* self = static_cast<class_type*> (type->tp_alloc(type, 0));
            if (self == nullptr)
                return nullptr;

            new (self) class_type();
            return self;
        }

        ///导出类内存回收
        static void tp_dealloc(PyObject* self)
        {
            ((class_type*) self)->~class_type();
            Py_TYPE(self)->tp_free(self);
        }

        ///导出类构造方法
        static int tp_init(PyObject *self, PyObject *args, PyObject *kwds)
        {
            bool ret = ((class_type*) self)->init(args, kwds);
            return ret ? 0 : -1;
        }
    };

    template<class Type>
    void PyExtenClass<Type>::extenMethod()
    {
        if (isTypeReady())
            return;

        if (!Type::BaseClass::s_factory.isTypeReady())
            Type::BaseClass::s_factory.extenMethod();

        this->readyType();
    }

    template<class Type>
    void PyExtenClass<Type>::readyType()
    {
        assert(!isTypeReady());

#ifdef ENABLE_EXPORT_MSG
        Lazy::debugMessageA("Export class %s.%s", mod_.c_str(), cls_.c_str());
#endif

        PyObject *module = helper::getModule(mod_);
        assert(module);

        PyTypeObject t =
        {
            PyVarObject_HEAD_INIT(NULL, 0)
            cls_.c_str(),  /* tp_name */
            sizeof(class_type),
            0,
        };
        pytype_ = t;
        pytype_.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
        pytype_.tp_new = this->tp_new;
        pytype_.tp_init = this->tp_init;
        pytype_.tp_dealloc = this->tp_dealloc;
        pytype_.tp_base = helper::py_type<class_type::BaseClass>();

        //We can do something in this function.
        class_type::s_initExtens();

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

} // end namespace Lzpy
