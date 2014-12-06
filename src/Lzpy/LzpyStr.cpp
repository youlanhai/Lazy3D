#include "stdafx.h"
#include "LzpyStr.h"

namespace Lzpy
{
    str::str()
    {
        m_ptr = PyUnicode_FromString("");
    }

    str::str(const std::wstring & s)
    {
        m_ptr = PyUnicode_FromUnicode(s.c_str(), s.length());
    }

    const str & str::operator=(const std::wstring & s)
    {
        return *this = str(s);
    }

    std::wstring str::string() const
    {
        std::wstring s;
        parse_object(s, *this);
        return s;
    }

    const wchar_t * str::c_str() const
    {
        return PyUnicode_AsUnicode(m_ptr);
    }

    size_t str::size() const
    {
        assert(m_ptr);
        return PyUnicode_GetLength(m_ptr);
    }

    bool str::check() const
    {
        return m_ptr && PyUnicode_Check(m_ptr);
    }
}