#pragma once

#include "LzpyExtendHelper.h"

///python交互
namespace Lzpy
{
    namespace helper
    {

        ///导出基类
        class PyBase : public PyObject
        {
            LZPY_DEF(PyBase, PyObject);
        public:
            PyBase();
            ~PyBase();

        };

        template<>
        inline void ready_type<PyBase>()
        {
            PyBase::s_factory.readyType();
        }

    }

    using helper::PyBase;

}// end namespace Lzpy
