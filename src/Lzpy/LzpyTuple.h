#pragma once

#include "LzpyObject.h"

namespace Lzpy
{

    class tuple : public object
    {
    public:
        PY_OBJECT_DECLARE(tuple, object);

        tuple()
        {
            m_ptr = PyTuple_New(0);
        }

        explicit tuple(size_t n)
        {
            m_ptr = PyTuple_New(n);
        }

        size_t size() const
        {
            return PyTuple_Size(m_ptr);
        }

        bool resize(size_t n)
        {
            return 0 == _PyTuple_Resize(&m_ptr, n);
        }

        object operator [](size_t i) const
        {
            return getitem(i);
        }

    public:

        bool check() const
        {
            return is_tuple();
        }

        object getitem(size_t i) const
        {
            //borrow reference
            return object(PyTuple_GetItem(m_ptr, i));
        }

        bool setitem(size_t i, const object & v)
        {
            //steal reference
            return 0 == PyTuple_SetItem(m_ptr, i, xincref(v.get()));
        }

        template<typename T>
        bool setitem(size_t i, const T & v)
        {
            return setitem(i, build_object(v));
        }

        template<typename... Args>
        bool parse_tuple(Args... args) const
        {
            if (this->size() != sizeof...(args))
            {
                PyErr_SetString(PyExc_TypeError, "argument count doesn't match the tuple size!");
                return false;
            }

            return this->_parse_tuple(0, args...);
        }

        template<typename... Args>
        bool parse_tuple_default(size_t nMust, Args... args) const
        {
            if (this->size() < nMust)
            {
                PyErr_SetString(PyExc_TypeError, "argument count doesn't match the tuple size!");
                return false;
            }

            assert(sizeof...(args) >= nMust);

            return this->_parse_tuple(0, args...);
        }
    private:

        inline bool _parse_tuple(size_t i) const
        {
            return true;
        }

        template<typename T, typename... Args>
        bool _parse_tuple(size_t i, T *v, Args... args) const
        {
            if (i >= this->size()) return true;

            object o = this->getitem(i);
            if (!parse_object(*v, o))
            {
                PyErr_Format(PyExc_TypeError, "can't extract %dth argument!", i);
                return false;
            }

            return this->_parse_tuple(i + 1, args...);
        }
    };

    template<typename... Args>
    tuple build_tuple(Args... args)
    {
        size_t n = sizeof...(args);
        tuple tp(n);
        set_tuple_item(tp, 0, args...);
        return tp;
    }

    inline void set_tuple_item(tuple & tp, size_t i)
    {}

    template<typename T, typename... Args>
    void set_tuple_item(tuple & tp, size_t i, T v, Args... args)
    {
        tp.setitem(i, v);
        set_tuple_item(tp, i + 1, args...);
    }
}
