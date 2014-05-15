#pragma once

#include "LzpyObject.h"

namespace Lzpy
{

    class str : public object
    {
    public:
        str()
            : object(new_reference(PyUnicode_FromString("")))
        {}

        str(const std::wstring & s)
        {
            m_ptr = PyUnicode_FromUnicode(s.c_str(), s.length());
        }

        str(const object & o)
            : object(o)
        {}

        str(const str & o)
            : object(borrow_reference(o.m_ptr))
        {}

        const str & operator=(const str & o)
        {
            set_borrow(o.get());
            return *this;
        }

        const str & operator=(const object & o)
        {
            set_borrow(o.get());
            return *this;
        }

        const str & operator=(const std::wstring & s)
        {
            return *this = str(s);
        }

        size_t size() const
        {
            assert(m_ptr);
            return PyUnicode_GetLength(m_ptr);
        }

        bool check() const
        {
            return m_ptr && PyUnicode_Check(m_ptr);
        }

    };

}

