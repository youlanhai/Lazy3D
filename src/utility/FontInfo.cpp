#include "stdafx.h"
#include "FontInfo.h"
#include "Misc.h"

namespace Lazy
{
    FontInfo::FontInfo()
        : height(0)
        , bold(0)
    {

    }

    FontInfo::FontInfo(const tstring & fontName)
        : height(0)
        , bold(0)
    {
        from(fontName);
    }

    void FontInfo::from(const tstring & fontName)
    {
        tstring name = fontName;

        int i = 0;
        size_t lastPos = 0;
        size_t pos = 0;

        while (true)
        {
            pos = name.find(L'#', lastPos); //‘#’分隔符以后会删除，推荐用符号‘|’
            if (pos == name.npos)
            {
                pos = name.find(L'|', lastPos);

                if (pos == name.npos)
                {
                    pos = name.size();
                }
            }

            if (i == 0)
            {
                this->name = name.substr(lastPos, pos);
            }
            else
            {
                tstring temp = name.substr(lastPos, pos - lastPos);
                int data = 0;
                swscanf(temp.c_str(), _T("%d"), &data);
                if (i == 1)
                {
                    height = data;
                }
                else if (i == 2)
                {
                    bold = data ? 1 : 0;
                }
                else
                {
                    break;;
                }
            }

            if (pos >= name.size())
            {
                break;
            }

            ++i;
            lastPos = pos + 1;
        }
    }

    void FontInfo::to(tstring & fontName)
    {
        bold = bold ? 1 : 0;
        formatString(fontName, _T("%s|%d|%d"), fontName.c_str(), height, bold);
    }

    bool FontInfo::valid() const
    {
        if (name.empty())
        {
            return false;
        }

        if (height < 5 || height > 100)
        {
            return false;
        }

        return true;
    }

}