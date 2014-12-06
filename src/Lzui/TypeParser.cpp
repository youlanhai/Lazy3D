#include "stdafx.h"
#include "TypeParser.h"

#include <sstream>

namespace Lazy
{

    namespace TypeParser
    {
        
        bool parseBool(const tstring & val)
        {
            if (val == L"true")
                return true;
            else if (val == L"false")
                return false;

            return parseInt(val) != 0;
        }

        int parseInt(const tstring & val)
        {
            return _wtoi(val.c_str());
        }

        uint32 parseUint(const tstring & val)
        {
            return (uint32)_wtoi(val.c_str());
        }

        uint32 parseHex(const tstring & val)
        {
            const tchar * it = val.c_str();
            const tchar * end = it + val.size();

            uint32 v = 0;

            //trim white space
            for (;
                it < end && (*it == ' ' || *it == '\t');
                ++it
                );

            //0x or 0X
            if (it < end && *it == '0')
            {
                ++it;
                if (it < end && (*it == 'x' || *it == 'X'))
                    ++it;
            }

            for (; it < end; ++it)
            {
                if (*it >= '0' && *it <= '9')
                {
                    v <<= 4;
                    v += *it - '0';
                }
                else if (*it >= 'A' && *it <= 'F')
                {
                    v <<= 4;
                    v += *it - 'A' + 10;
                }
                else if (*it >= 'a' && *it <= 'f')
                {
                    v <<= 4;
                    v += *it - 'a' + 10;
                }
                else
                    break;
            }

            return v;
        }

        float parseFloat(const tstring & val)
        {
            return (float)_wtof(val.c_str());
        }

        double parseDouble(const tstring & val)
        {
            return (float) _wtof(val.c_str());
        }

        const tstring & parseString(const tstring & val)
        {
            return val;
        }

        CPoint parsePoint(const tstring & val)
        {
            CPoint pt;
            pt.x = _wtoi(val.c_str());

            size_t i = val.find_first_of(L" \t");
            if (i != val.npos)
                pt.y = _wtoi(val.c_str() + i + 1);

            return pt;
        }

        CSize parseSize(const tstring & val)
        {
            CSize size;
            size.cx = _wtoi(val.c_str());

            size_t i = val.find_first_of(L" \t");
            if (i != val.npos)
                size.cy = _wtoi(val.c_str() + i + 1);

            return size;
        }

    }

}