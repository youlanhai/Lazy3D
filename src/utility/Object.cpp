#include "stdafx.h"
#include "Object.h"

#include "MemoryCheck.h"

namespace Lazy
{

    Object::Object(void)
        : m_nRefCounter(0)
        , m_pScript(nullptr)
    {
        MEMORY_CHECK_CONS(this);
    }

    Object::Object(const Object &)
        : m_nRefCounter(0)
        , m_pScript(nullptr)
    {
        MEMORY_CHECK_CONS(this);
    }

    Object::~Object(void)
    {
        MEMORY_CHECK_DEST(this);
    }
}
