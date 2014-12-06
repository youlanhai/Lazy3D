#include "stdafx.h"
#include "LzpyBase.h"
#include "../utility/MemoryCheck.h"

namespace Lzpy
{
    namespace helper
    {
        LZPY_CLASS_BEG(PyBase);
        LZPY_CLASS_END();

        PyBase::PyBase()
        {
        }

        PyBase::~PyBase()
        {
        }

        LZPY_IMP_INIT(PyBase)
        {
            return true;
        }

    }
}
