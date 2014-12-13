#include "stdafx.h"
#include "LzpyOutput.h"


namespace Lazy
{
    LZPY_CLASS_BEG(PyOutput);
    LZPY_METHOD_1(write);
    LZPY_METHOD_0(flush);
    LZPY_CLASS_END();

    PyOutput::PyOutput()
    {

    }

    LZPY_IMP_INIT(PyOutput)
    {
        return true;
    }

    LZPY_IMP_METHOD_1(PyOutput, write)
    {
        std::wstring msg;
        if (!parse_object(msg, value)) return null_object;

        if (msg == L"\n")
        {
            OutputDebugStringW(m_msg.c_str());
            m_msg.clear();
        }
        else
        {
            m_msg += msg;
        }

        return none_object;
    }

    LZPY_IMP_METHOD_0(PyOutput, flush)
    {
        if (!m_msg.empty())
        {
            OutputDebugStringW(m_msg.c_str());
            m_msg.clear();
        }

        return none_object;
    }

}