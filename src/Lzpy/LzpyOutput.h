﻿#pragma once

#include "LzpyBase.h"

namespace Lazy
{
    ///用于python输出重定向
    class PyOutput : public PyBase
    {
        LZPY_DEF(PyOutput);
    public:
        PyOutput();

        LZPY_DEF_METHOD_1(write);
        LZPY_DEF_METHOD_0(flush);

        std::wstring m_msg;
    };

}// end namespace Lazy

