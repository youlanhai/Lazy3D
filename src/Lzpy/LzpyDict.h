#pragma once

#include "LzpyObject.h"

namespace Lzpy
{
    class list;

    class dict : public object
    {
    public:
        PY_OBJECT_DECLARE(dict, object);

        dict()
        {
            m_ptr = PyDict_New();
        }

    public:

        bool check() const
        {
            return m_ptr && PyDict_Check(m_ptr);
        }

        size_t size() const
        {
            return PyDict_Size(m_ptr);
        }

        object getitem(object k) const
        {
            //borrow reference
            return object(PyDict_GetItem(m_ptr, k.get()));
        }

        template<typename K>
        object getitem(const K & k) const
        {
            return getitem(build_object(k));
        }

        bool setitem(const object & k, const object & v)
        {
            return 0 == PyDict_SetItem(m_ptr, k.get(), v.get());
        }

        template<typename K, typename V>
        bool setitem(const K & k, const V & v)
        {
            return setitem(build_object(k), build_object(v));
        }

        list keys();
        list values();
        
    };

}