#pragma once

#include "LzpyObject.h"

namespace Lzpy
{

    class str : public object
    {
    public:
        PY_OBJECT_DECLARE(str, object);

        str();

        str(const std::wstring & s);

        const str & operator=(const std::wstring & s);

        std::wstring string() const;

        const wchar_t * c_str() const;

        size_t size() const;

        bool check() const;
    };

}
