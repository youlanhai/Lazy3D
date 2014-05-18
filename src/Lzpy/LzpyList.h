#pragma once

#include "LzpyObject.h"

namespace Lzpy
{
    class tuple;

    class list : public object
    {
    public:
        PY_OBJECT_DECLARE(list, object);

        list()
        {
            m_ptr = PyList_New(0);
        }

        explicit list(size_t n)
        {
            m_ptr = PyList_New(n);
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
            //borrow reference
            return object(PyList_GetItem(m_ptr, i));
        }

        bool setitem(size_t i, const object & v)
        {
            //steal reference
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
