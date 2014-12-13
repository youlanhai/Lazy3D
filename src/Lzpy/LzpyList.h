#pragma once

#include "LzpyObject.h"

namespace Lazy
{
    class tuple;

    class list : public object
    {
    public:
        PY_OBJECT_DECLARE(list, object);

        list();

        explicit list(size_t n);

        bool check() const;

        size_t size() const;

        object operator [](size_t i) const;

        object getitem(size_t i) const;

        bool setitem(size_t i, const object & v);

        bool append(const object & v);

        bool insert(size_t i, const object & v);

        tuple as_tuple() const;

    public:

        template<typename T>
        bool setitem(size_t i, const T & v)
        {
            return setitem(i, build_object(v));
        }

        template<typename T>
        bool append(const T & v)
        {
            return append(build_object(v));
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

    };
}
