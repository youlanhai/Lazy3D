#pragma once

#include "LzpyObject.h"

namespace Lzpy
{

    class tuple : public object
    {
    public:
        tuple() :
            object(new_reference(PyTuple_New(0)))
        {}

        explicit tuple(size_t n)
            : object(new_reference(PyTuple_New(n)))
        {}

        tuple(const tuple & o)
            : object(borrow_reference(o.m_ptr))
        {}

        tuple(const object & o)
            : object(o)
        {}

        tuple(const borrow_reference & o)
            : object(o)
        {}

        tuple(const new_reference & o)
            : object(o)
        {}

        const tuple & operator=(const tuple & o)
        {
            set_borrow(o.m_ptr);
            return *this;
        }

        const tuple & operator=(const object & o)
        {
            set_borrow(o.get());
            return *this;
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
            return borrow_reference(PyTuple_GetItem(m_ptr, i));
        }

        bool setitem(size_t i, const object & v)
        {
            return 0 == PyTuple_SetItem(m_ptr, i, xincref(v.get()));
        }

        template<typename T>
        bool setitem(size_t i, const T & v)
        {
            return setitem(i, build_object(v));
        }

        template<typename... Args>
        void build_tuple(Args... args)
        {
            size_t n = sizeof...(args);
            this->set_new(PyTuple_New(n));

            if (n > 0) this->_build_tuple(0, args...);
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
        bool parse_n_tuple(size_t n, Args... args) const
        {
            n = min(n, this->size());
            return this->_parse_n_tuple(n, 0, args...);
        }
    private:

        inline void _build_tuple(size_t i)
        {}

        template<typename T, typename... Args>
        void _build_tuple(size_t i, T v, Args... args)
        {
            this->setitem(i, v);
            this->_build_tuple(i + 1, args...);
        }

        inline bool _parse_tuple(size_t i) const
        {
            return true;
        }

        template<typename T, typename... Args>
        bool _parse_tuple(size_t i, T *v, Args... args) const
        {
            object o = this->getitem(i);
            if (!o.parse(*v))
            {
                PyErr_Format(PyExc_TypeError, "can't extract %dth argument!", i);
                return false;
            }

            return this->_parse_tuple(i + 1, args...);
        }

        inline bool _parse_n_tuple(size_t n, size_t i) const
        {}

        template<typename T, typename... Args>
        bool _parse_n_tuple(size_t n, size_t i, T *v, Args... args) const
        {
            if (i >= n) return;

            object o = this->getitem(i);
            if (!o.parse(*v))
            {
                PyErr_Format(PyExc_TypeError, "can't extract %dth argument!", i);
                return false;
            }

            return this->_parse_n_tuple(n, i + 1, args...);
        }

    };

    template<typename... Args>
    tuple build_tuple(Args... args)
    {
        tuple tp;
        tp.build_tuple(args...);
        return tp;
    }

}
