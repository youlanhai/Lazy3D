#include "stdafx.h"
#include "LzpyTuple.h"


namespace Lzpy
{
    tuple::tuple()
    {
        m_ptr = PyTuple_New(0);
    }

    tuple::tuple(size_t n)
    {
        m_ptr = PyTuple_New(n);
    }

    size_t tuple::size() const
    {
        return PyTuple_Size(m_ptr);
    }

    object tuple::operator [](size_t i) const
    {
        return getitem(i);
    }

    bool tuple::check() const
    {
        return is_tuple();
    }

    object tuple::getitem(size_t i) const
    {
        //borrow reference
        return object(PyTuple_GetItem(m_ptr, i));
    }

    bool tuple::setitem(size_t i, const object & v)
    {
        //steal reference
        return 0 == PyTuple_SetItem(m_ptr, i, xincref(v.get()));
    }
}
