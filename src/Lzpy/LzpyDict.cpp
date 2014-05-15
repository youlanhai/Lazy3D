#include "stdafx.h"
#include "LzpyDict.h"
#include "LzpyList.h"

namespace Lzpy
{
    list dict::keys()
    {
        return new_reference(PyDict_Keys(m_ptr));
    }

    list dict::values()
    {
        return new_reference(PyDict_Values(m_ptr));
    }
}