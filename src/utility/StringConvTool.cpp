#include "StdAfx.h"
#include "StringConvTool.h"
#include "Misc.h"

namespace Lazy
{

    const tstring strBoolTrue = _T("true");
    const tstring strBoolFalse = _T("false");


    void buildString(tstring & str, int32 v)
    {
        buildStringNum(str, v, 10);
    }

    void buildString(tstring & str, uint32 v)
    {
        buildStringNum(str, v, 10);
    }

    void buildString(tstring & str, Bin v)
    {
        buildStringNum(str, v.v, 2);
    }

    void buildString(tstring & str, Oct v)
    {
        buildStringNum(str, v.v, 8);
    }

    void buildString(tstring & str, Hex v)
    {
        buildStringNum(str, v.v, 16);
    }







    int StringConvTool::asInt() const
    {
        return _tstoi(m_value.c_str());
    }

    size_t StringConvTool::asUint() const
    {
        size_t n;
        swscanf(m_value.c_str(), _T("%u"), &n);
        return n;
    }

    size_t StringConvTool::asHex() const
    {
        size_t n;
        swscanf(m_value.c_str(), _T("%x"), &n);
        return n;
    }

    bool StringConvTool::asBool() const
    {
        if (m_value == strBoolTrue)
        {
            return true;
        }
        else if (m_value == strBoolFalse)
        {
            return false;
        }
        return !!asInt();
    }

    float StringConvTool::asFloat() const
    {
        return (float) asDouble();
    }

    double StringConvTool::asDouble() const
    {
        return _tstof(m_value.c_str());;
    }


    void StringConvTool::setInt(int data)
    {
        formatFrom(_T("%d"), data);
    }

    void StringConvTool::setUint(size_t data)
    {
        formatFrom(_T("%u"), data);
    }

    void StringConvTool::setHex(size_t data)
    {
        formatFrom(_T("%8.8x"), data);
    }

    void StringConvTool::setBool(bool data)
    {
        if (data)
        {
            m_value = strBoolTrue;
        }
        else
        {
            m_value = strBoolFalse;
        }
    }

    void StringConvTool::setFloat(float data)
    {
        formatFrom(_T("%f"), data);
    }

    void StringConvTool::setDouble(double data)
    {
        formatFrom(_T("%lf"), data);
    }

    bool StringConvTool::formatFrom(LPCTSTR pFormat, ...)
    {
        va_list pArgList;
        va_start(pArgList, pFormat);

        bool ret = formatStringVS(m_value, pFormat, pArgList);

        va_end(pArgList);

        return ret;
    }

#pragma warning(push)
#pragma warning(disable: 4996)

    bool StringConvTool::formatTo(LPCTSTR pFormat, ...)
    {
        va_list pArgList;
        va_start(pArgList, pFormat);

        int ret = _vstscanf(m_value.c_str(), pFormat, pArgList);

        va_end(pArgList);

        return ret != EOF;
    }
#pragma warning(pop)

}//namespace Lazy