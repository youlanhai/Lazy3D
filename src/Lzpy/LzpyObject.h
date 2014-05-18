#pragma once

namespace Lzpy
{

    class object;
    class tuple;
    class list;
    class str;
    class dict;
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

    class object_base
    {
    public:
        object_base();
        ~object_base();

        explicit object_base(PyObject *p);
        object_base(const object_base & t);

        const object_base & operator = (const object_base & t);

        bool operator == (const object_base & v) const;
        bool operator < (const object_base & v) const;
        bool operator >(const object_base & v) const;

        operator bool() const;

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
        T* cast() { return (T*) m_ptr; }

        inline PyObject *get() const {  return m_ptr; }

        inline void addRef(){ Py_XINCREF(m_ptr); }

        inline void delRef() { Py_XDECREF(m_ptr); }

    public:

        bool hasattr(const char *attr) const;
        object getattr(const char *attr) const;
        bool setattr(const char *attr, object value);
        bool delattr(const char *attr);
        Lazy::tstring repr();
        void print();

        object call_python();
        object call_python(tuple arg);
        object call_python(tuple arg, dict kwd);

        template<typename... Args>
        object call(Args... args);

        template<typename... Args>
        object call_quiet(Args... args);

        template<typename... Args>
        object call_method(const char * method, Args... args);

        template<typename... Args>
        object call_method_quiet(const char * method, Args... args);

    protected:
        PyObject *m_ptr;
    };

#define PY_OBJECT_DECLARE(OBJECT, BASE)                                 \
    explicit OBJECT(PyObject *v){ m_ptr = xincref(v); }                 \
    OBJECT(const object_base & v){ m_ptr = xincref(v.get()); }          \
    OBJECT(const new_reference & v){ m_ptr = v.get(); }                 \
    const OBJECT & operator = (const object_base & v){ set_borrow(v.get()); return *this; } \
    const OBJECT & operator = (const new_reference & v){ set_new(v.get()); return *this; }  \

    class object : public object_base
    {
    public:
        object();
        ~object();

        PY_OBJECT_DECLARE(object, object_base);

    protected:

        //borrowed refrence
        void set_borrow(PyObject *p);

        //new refrence
        void set_new(PyObject *p);
    };

    const object null_object;
    const object none_object(Py_None);

    template<typename... Args>
    object object_base::call(Args... args)
    {
        return call_python(build_tuple(args...));
    }

    template<typename... Args>
    object object_base::call_quiet(Args... args)
    {
        if (is_none() || is_null()) return null_object;
        return call(args...);
    }

    template<typename... Args>
    object object_base::call_method(const char * method, Args... args)
    {
        object o = getattr(method);
        return o.call(args...);
    }

    template<typename... Args>
    object object_base::call_method_quiet(const char * method, Args... args)
    {
        if (is_null() || !hasattr(method)) return null_object;
        return call_method(method, args...);
    }

}// end namespace Lzpy

