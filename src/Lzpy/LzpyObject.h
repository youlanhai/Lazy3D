#pragma once

namespace Lzpy
{

    class object;
    class tuple;
    class list;
    class str;
    class dict;
    class borrow_reference;
    class new_reference;

    inline PyObject * xincref(PyObject * p)
    {
        Py_XINCREF(p);
        return p;
    }

    inline PyObject * xdecref(PyObject * p)
    {
        Py_XDECREF(p);
        return p;
    }

    //基本数据类型打包
    object build_object(bool v);
    object build_object(int v);
    object build_object(unsigned int v);
    object build_object(long v);
    object build_object(unsigned long v);
    object build_object(float v);
    object build_object(double v);
    object build_object(const std::string & v);
    object build_object(const std::wstring & v);
    object build_object(const char * v);
    object build_object(const wchar_t * v);
    object build_object(const object & v);
    object build_object(const tuple & v);
    object build_object(const dict & v);
    object build_object(const str & v);
    object build_object(const list & v);
    object build_object(PyObject *p);
    object build_object(const borrow_reference & v);
    object build_object(const new_reference & v);

    //基本类型拆包
    bool parse_object(bool &v,              const object & o);
    bool parse_object(int & v,              const object & o);
    bool parse_object(unsigned int & v,     const object & o);
    bool parse_object(long & v,             const object & o);
    bool parse_object(unsigned long & v,    const object & o);
    bool parse_object(float & v,            const object & o);
    bool parse_object(double & v,           const object & o);
    bool parse_object(std::string & v,      const object & o);
    bool parse_object(std::wstring & v,     const object & o);
    bool parse_object(char *& v,            const object & o);
    bool parse_object(wchar_t *& v,         const object & o);
    bool parse_object(object & v,           const object & o);
    bool parse_object(tuple & v,            const object & o);
    bool parse_object(dict & v,             const object & o);
    bool parse_object(str & v,              const object & o);
    bool parse_object(list & v,             const object & o);

    template<typename T>
    T parse_object(const object & o)
    {
        T v;
        if (!parse_object(v, o)) 
            throw(python_error("extract value failed!"));
        return v;
    }


    ///python出现异常
    class python_error : public std::exception
    {
    public:
        python_error();

        python_error(const char * error);

        ~python_error()
        {}
    };


    class borrow_reference
    {
        PyObject *m_ptr;

    public:

        explicit borrow_reference(PyObject *p)
            : m_ptr(xincref(p))
        {}

        PyObject * get() const 
        { 
            return m_ptr; 
        }

        friend class object;
    };


    class new_reference
    {
        PyObject *m_ptr;

    public:

        explicit new_reference(PyObject *p)
            : m_ptr(p)
        {}

        PyObject * get() const
        {
            return m_ptr;
        }

        friend class object;
    };


    class none
    {
        PyObject *m_ptr;

    public:

        none()
            : m_ptr(xincref(Py_None))
        {}

        friend class object;
    };


    class object
    {
    public:
        object()
            : m_ptr(nullptr)
        {}

        object(borrow_reference const & t)
            : m_ptr(t.m_ptr)
        {}

        object(new_reference const & t)
            : m_ptr(t.m_ptr)
        {}

        object(none const & t)
            : m_ptr(t.m_ptr)
        {}

        object(const object & obj)
            : m_ptr(xincref(obj.m_ptr))
        {}

        explicit object(PyObject *p)
            : m_ptr(xincref(p))
        {}

        ~object()
        {
            delRef(); 
        }

        const object & operator = (const object & obj)
        {
            set_borrow(obj.m_ptr);
            return *this;
        }

        const object & operator = (borrow_reference const & t)
        {
            set_borrow(t.m_ptr);
            return *this;
        }

        const object & operator = (new_reference const & t)
        {
            set_borrow(t.m_ptr);
            return *this;
        }

        const object & operator = (none const & t)
        {
            set_borrow(t.m_ptr);
            return *this;
        }

        operator bool() const
        {
            return m_ptr != nullptr && PyObject_IsTrue(m_ptr);
        }

        bool operator == (const object & v) const
        {
            return m_ptr == v.m_ptr;
        }

        bool operator < (const object & v) const
        {
            return m_ptr < v.m_ptr;
        }

        bool operator > (const object & v) const
        {
            return m_ptr > v.m_ptr;
        }

        bool callable() const;

        bool is_null() const;
        bool is_none() const;
        bool is_int() const;
        bool is_float() const;
        bool is_bool() const;
        bool is_str() const;
        bool is_tuple() const;
        bool is_list() const;
        bool is_dict() const;
        bool is_module() const;

    public:

        template<typename T>
        T* cast()
        {
            return (T*) m_ptr;
        }

        inline PyObject *get() const
        {
            return m_ptr;
        }

        inline void addRef()
        {
            Py_XINCREF(m_ptr);
        }

        inline void delRef()
        {
            Py_XDECREF(m_ptr);
        }

    public:

        bool hasattr(const char *attr) const;
        object getattr(const char *attr) const;
        bool setattr(const char *attr, object value);
        bool delattr(const char *attr);
        Lazy::tstring repr();
        void print();

        template<typename T>
        void build(const T & v)
        {
            *this = build_object(v);
        }

        template<typename T>
        bool parse(T & v) const
        {
            return parse_object(v, *this);
        }

        template<typename T>
        T parse() const
        {
            T v;
            if (!parse_object(v, *this)) throw(python_error("extract value failed!"));
            return v;
        }

        template<typename... Args>
        object call(Args... args)
        {
            if (!callable())
                throw(python_error("This object doesn't callable!"));

            tuple tp;
            tp.build_tuple(args...);

            PyObject *ret = PyObject_Call(m_ptr, tp.get(), nullptr);
            if (ret == nullptr)
                throw(python_error("Function call failed!"));

            return new_reference(ret);
        }

        template<typename... Args>
        object call_quiet(Args... args)
        {
            if (is_none() || is_null()) return null_object;
            return call(args...);
        }

        template<typename... Args>
        object call_method(const char * method, Args... args)
        {
            object o = getattr(method);
            return o.call(args...);
        }

        template<typename... Args>
        object call_method_quiet(const char * method, Args... args)
        {
            if (is_null() || !hasattr(method)) return null_object;
            return call_method(method, args...);
        }

        object call_python();
        object call_python(tuple arg);
        object call_python(tuple arg, dict kwd);

    protected:

        //borrowed refrence
        void set_borrow(PyObject *p)
        {
            if (m_ptr == p) return;

            xdecref(m_ptr);
            m_ptr = xincref(p);
        }

        //new refrence
        void set_new(PyObject *p)
        {
            if (m_ptr == p) return;

            xdecref(m_ptr);
            m_ptr = p;
        }

        PyObject *m_ptr;
    };

    const object null_object;
    const object none_object = none();


    template<typename T>
    class object_ptr : public object
    {
    public:
        typedef object_ptr<T> this_type;
        typedef T   value_type;
        typedef T*  pointer;
        typedef T&  refrence;

        object_ptr()
        {}

        object_ptr(const this_type & o)
        {
            m_ptr = xincref(o.get());
        }

        object_ptr(const object & o)
            : object(o)
        {}

        object_ptr(const borrow_reference & v)
            : object(v)
        {}

        object_ptr(const new_reference & v)
            : object(v)
        {}

        
        const this_type & operator=(const this_type & o)
        {
            set_borrow(o.get());
            return *this;
        }

        const this_type & operator=(const object & o)
        {
            set_borrow(o.get());
            return *this;
        }

        const this_type & operator=(const borrow_reference & o)
        {
            set_borrow(o.get());
            return *this;
        }

        const this_type & operator=(const new_reference & o)
        {
            set_borrow(o.get());
            return *this;
        }

        pointer operator->()
        {
            return (pointer) m_ptr;
        }
        
        refrence operator*()
        {
            return *((pointer) m_ptr);
        }

        bool operator==(const this_type & o) const
        {
            return m_ptr == o.get();
        }

        bool operator==(const object & o) const
        {
            return m_ptr == o.get();
        }
    };
}
