#include "stdafx.h"
#include "LzpyList.h"
#include "LzpyTuple.h"

namespace Lzpy
{
    tuple list::as_tuple() const
    {
        assert(m_ptr);
        return new_reference(PyList_AsTuple(m_ptr));
    }
}