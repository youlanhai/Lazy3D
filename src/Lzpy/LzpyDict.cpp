#include "stdafx.h"
#include "LzpyDict.h"
#include "LzpyList.h"

namespace Lzpy
{
    dict::dict()
    {
        m_ptr = PyDict_New();
    }

    bool dict::check() const
    {
        return m_ptr && PyDict_Check(m_ptr);
    }

    size_t dict::size() const
    {
        return PyDict_Size(m_ptr);
    }

    object dict::getitem(object k) const
    {
        //borrow reference
        return object(PyDict_GetItem(m_ptr, k.get()));
    }

    bool dict::setitem(const object & k, const object & v)
    {
        return 0 == PyDict_SetItem(m_ptr, k.get(), v.get());
    }

    list dict::keys()
    {
        return new_reference(PyDict_Keys(m_ptr));
    }

    list dict::values()
    {
        return new_reference(PyDict_Values(m_ptr));
    }
}