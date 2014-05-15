#include "stdafx.h"
#include "SmartPtr.h"
#include "MemoryCheck.h"

namespace Lazy
{
    IBase::IBase(void)
        : m_nRefCounter_(0)
    {
        MEMORY_CHECK_CONS(this);
    }

    IBase::IBase(const IBase &)
        : m_nRefCounter_(0)
    {
        MEMORY_CHECK_CONS(this);
    }

    IBase::~IBase(void)
    {
        MEMORY_CHECK_DEST(this);
    }

    const IBase & IBase::operator = (const IBase &)
    {
        return *this;
    }

    void IBase::addRef(void) 
    { 
        ++m_nRefCounter_;
    }

    void IBase::delRef(void)
    {
        --m_nRefCounter_;
        if (m_nRefCounter_ <= 0)
        {
            destroyThis();
        }
    }

    int IBase::getRef(void) const 
    {
        return m_nRefCounter_;
    }

    bool IBase::isRefUnique() const 
    {
        return m_nRefCounter_ == 1;
    }

}//end namespace Lazy