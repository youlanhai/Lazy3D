//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xmlParser.h"
#include "xml.h"

namespace Lazy
{


//////////////////////////////////////////////////////////////////////////
xmlParser::xmlParser(const tchar* buffer, int size)
    : m_stream(buffer, size)
{
}

xmlParser::~xmlParser(void)
{
}

bool xmlParser::parse(LZDataPtr parent)
{
    if (!parent)
    {
        m_root = new xml(_T("root"));
    }
    else
    {
        m_root = parent;
    }
    return parseReal(m_root);
}

bool xmlParser::parseReal(LZDataPtr parent)
{
    while(!m_stream.empty())
    {
        int begin = m_stream.find(_T('<'));
        if (begin == -1)
        {
            break;
        }
        int end = m_stream.find(_T('>'));
        if (end == -1)
        {
            printf("couldn't find end mark! '>'\n");
            return false;
        }
        m_stream.seek(end+1);

        tstring tag ;
        m_stream.getRange(tag, begin+1, end);
        trimString(tag);

        if (tag[0] == _T('/')) //是结束标签
        {
            tag.erase(0, 1);
            trimString(tag);

            if (tag != parent->tag())
            {
                printf("end tag </%s> not match with begin tag <%s>\n", tag.c_str(), parent->tag().c_str());
                return false;
            }
            return true; //匹配成功
        }
        else
        {
            bool isAttr = false;
            int lastIndex = (int)tag.length()-1;
            if (lastIndex > 0 && tag[lastIndex] == _T('/')) //属性
            {
                isAttr = true;
                size_t pos = tag.find_first_of(_T(' '));
                if (pos == tag.npos)
                {
                    tag.erase(lastIndex);
                }
                else
                {
                    tag.erase(pos);
                }
            }

            LZDataPtr node = new xml(tag);
            parent->addChild(node);

            if (!isAttr)
            {
                begin = m_stream.index();
                end = m_stream.find(_T('<')); //找到结束标签的位置
                if (end == -1)
                {
                    printf("couldn't find next mark '<'!\n");
                    return false;
                }
                m_stream.seek(end);

                tstring value_;
                m_stream.getRange(value_, begin, end);
                trimString(value_);

                node->setString(value_);

                //解析子节点
                if (!parseReal(node))
                {
                    return false;
                }
            }
        }
    }
    if(parent != m_root)
    {
        printf("tag not closed : <%s>\n", parent->tag());
        return false;
    }
    return true;
}


}//namespace Lazy