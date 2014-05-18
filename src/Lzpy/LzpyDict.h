#pragma once

#include "LzpyObject.h"

namespace Lzpy
{
    class list;

    class dict : public object
    {
    public:
        PY_OBJECT_DECLARE(dict, object);

        dict();

        bool check() const;

        size_t size() const;

        object getitem(object k) const;
        bool setitem(const object & k, const object & v);

        list keys();
        list values();

    public:

        template<typename K>
        object getitem(const K & k) const
        {
            return getitem(build_object(k));
        }

        template<typename K, typename V>
        bool setitem(const K & k, const V & v)
        {
            return setitem(build_object(k), build_object(v));
        }
    };

}