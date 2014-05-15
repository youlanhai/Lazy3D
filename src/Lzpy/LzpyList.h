#pragma once

#include "LzpyObject.h"

namespace Lzpy
{
    class tuple;

    class list : public object
    {
    public:
        list()
        {
            m_ptr = PyList_New(0);
        }

        explicit list(size_t n)
        {
            m_ptr = PyList_New(n);
        }

        list(const list & l)
        {
            m_ptr = xincref(l.m_ptr);
        }

        list(const object & o)
        {
            m_ptr = xincref(o.get());
        }

        const list & operator=(const list & l)
        {
            if (&l != this) set_borrow(l.m_ptr);
            
            return *this;
        }

        const list & operator=(const object & o)
        {
            if (&o != this) set_borrow(o.get());

            return *this;
        }

    public:

        bool check() const
        {
            return m_ptr && PyList_Check(m_ptr);
        }

        size_t size() const
        {
            return PyList_Size(m_ptr);
        }

        object operator [](size_t i) const
        {
            return getitem(i);
        }

        object getitem(size_t i) const
        {
            return borrow_reference(PyList_GetItem(m_ptr, i));
        }

        bool setitem(size_t i, const object & v)
        {
            return 0 == PyList_SetItem(m_ptr, i, xincref(v.get()));
        }

        template<typename T>
        bool setitem(size_t i, const T & v)
        {
            return setitem(i, build_object(v));
        }

        bool append(const object & v)
        {
            return 0 == PyList_Append(m_ptr, v.get());
        }

        template<typename T>
        bool append(const T & v)
        {
            return append(build_object(v));
        }

        bool insert(size_t i, const object & v)
        {
            return 0 == PyList_Insert(m_ptr, i, v.get());
        }

        template<typename T>
        bool insert(size_t i, const T & v)
        {
            return insert(i, build_object(v));
        }

        template<typename T>
        bool remove(const T & v)
        {
            return call_method("remove", v);
        }

        tuple as_tuple() const;
    };
}
