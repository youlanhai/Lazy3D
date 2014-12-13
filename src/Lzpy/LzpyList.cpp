#include "stdafx.h"
#include "LzpyList.h"
#include "LzpyTuple.h"

namespace Lazy
{
    list::list()
    {
        m_ptr = PyList_New(0);
    }

    list::list(size_t n)
    {
        m_ptr = PyList_New(n);
    }

    bool list::check() const
    {
        return m_ptr && PyList_Check(m_ptr);
    }

    size_t list::size() const
    {
        return PyList_Size(m_ptr);
    }

    object list::operator [](size_t i) const
    {
        return getitem(i);
    }

    object list::getitem(size_t i) const
    {
        //borrow reference
        return object(PyList_GetItem(m_ptr, i));
    }

    bool list::setitem(size_t i, const object & v)
    {
        //steal reference
        return 0 == PyList_SetItem(m_ptr, i, xincref(v.get()));
    }

    bool list::append(const object & v)
    {
        return 0 == PyList_Append(m_ptr, v.get());
    }

    bool list::insert(size_t i, const object & v)
    {
        return 0 == PyList_Insert(m_ptr, i, v.get());
    }

    tuple list::as_tuple() const
    {
        assert(m_ptr);
        return new_reference(PyList_AsTuple(m_ptr));
    }
}