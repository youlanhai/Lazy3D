//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xml.h"
#include "xmlParser.h"

namespace Lazy
{

//////////////////////////////////////////////////////////////////////////

xml::xml(void)
{
}

xml::~xml(void)
{
}

xml::xml(const tstring & tag)
    : LZDataBase(tag)
{

}
xml::xml(const tstring & tag, const tstring & value)
    : LZDataBase(tag, value)
{

}

bool xml::loadFromBuffer(const tchar* buffer, int length)
{
    xmlParser parser(buffer, length);
    parser.parse();
    return parser.root();
}

void xml::print(tostream & out, int depth/*=0*/)
{
    for (DataConstIterator it = m_children.begin(); it != m_children.end(); ++it)
    {
        LZDataPtr ptr = *it;
        if ( ptr->countChildren() > 0)
        {
            printTable(out, depth);
            out<< _T("<") << ptr->tag() << _T(">") <<std::endl;

            ptr->print(out, depth+1);

            printTable(out, depth);
            out<< _T("</") << ptr->tag() << _T(">") <<std::endl;
        }
        else
        {
            printTable(out, depth);
            out<< _T("<") << ptr->tag() << _T(">\t") << ptr->asString();
            out<< _T("\t</") << ptr->tag() << _T(">") <<std::endl;
        }
    }
}


LZDataPtr xml::newOne(const tstring & tag, const tstring & value_)
{
    return new xml(tag, value_);
}

}//namespace Lazy