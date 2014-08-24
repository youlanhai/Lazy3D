//////////////////////////////////////////////////////////////////////////
/*
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "lzd.h"
#include "lzdParser.h"

#include "../FileTool.h"
#include "../Log.h"

#include <sstream>

namespace Lazy
{

//////////////////////////////////////////////////////////////////////////
    bool lzd::loadFromBuffer(const tchar* buffer, int length)
    {
        LzdParser parser(new StrStream(buffer, length), this, 1);
        parser.parse();

        if(parser.error())
        {
            LOG_ERROR(_T("Parse laz file failed! eno:%d, line:%d"),
                         parser.getErrorNo(), parser.getErrorLine());
        }

        return !parser.error();
    }

    lzd::lzd(void)
    {
    }

    lzd::lzd(const tstring & tag)
        : LZDataBase(tag)
    {
    }

    lzd::lzd(const tstring & tag, const tstring & value_)
        : LZDataBase(tag, value_)
    {
    }

    lzd::~lzd(void)
    {
    }

    LZDataPtr lzd::newOne(const tstring & tag, const tstring & value_)
    {
        return new lzd(tag, value_);
    }


    void lzd::print(tostream & out, int depth)
    {
        for (DataConstIterator it = m_children.begin(); it != m_children.end(); ++it)
        {
            LZDataPtr ptr = *it;
            if ( ptr->countChildren() > 0 )
            {
                printTable(out, depth);
                out << ptr->tag() << std::endl;

                printTable(out, depth);
                out << _T("{") << std::endl;

                ptr->print(out, depth + 1);

                printTable(out, depth);
                out << _T("}") << std::endl;
            }
            else
            {
                printTable(out, depth);
                tstring tempStr;
                LzdParser::real2transString(tempStr, ptr->value());
                out << ptr->tag() << _T(" = ") << tempStr << std::endl;
            }
        }
    }


    bool lzd::load(const tstring & fname)
    {
        std::string buffer;
        if(!getfs()->readString(fname, buffer)) return false;

        size_t cp = LzdParser::parseCodingHeader(buffer.c_str(), buffer.size());

        std::wstring fileBuffer;
        if(!charToWChar(fileBuffer, buffer, cp)) return false;

        return loadFromBuffer(&fileBuffer[0], fileBuffer.size());
    }

    bool lzd::save(const tstring & fname)
    {
        std::wostringstream stream;
        stream << _T("#coding = ") << Encode::utf8 << _T("\n\n");
        print(stream, 0);

        std::string buffer;
        if(!wcharToChar(buffer, stream.str(), CP::utf8)) return false;

        return getfs()->writeString(fname, buffer);
    }


}//namespace Lazy