#pragma once

#include "../utility/Tools2d.h"

namespace Lazy
{

    namespace TypeParser
    {
        bool parseBool(const tstring & val);
        int parseInt(const tstring & val);
        uint32 parseUint(const tstring & val);
        uint32 parseHex(const tstring & val);
        float parseFloat(const tstring & val);
        double parseDouble(const tstring & val);
        const tstring & parseString(const tstring & val);
        CPoint parsePoint(const tstring & val);
        CSize parseSize(const tstring & val);

    }

}