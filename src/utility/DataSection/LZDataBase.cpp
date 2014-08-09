//////////////////////////////////////////////////////////////////////////
/*
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LZDataBase.h"
#include <sstream>

#include "../FileTool.h"
#include "../MemoryCheck.h"

namespace Lazy
{

    void printTable(tostream & cout, int n)
    {
        for (int i = 0; i < n; ++i)
        {
            cout << _T('\t');
        }
    }

//////////////////////////////////////////////////////////////////////////

    LZDataBase::LZDataBase(void)
    {
    }

    LZDataBase::~LZDataBase(void)
    {
    }

    LZDataBase::LZDataBase(const tstring & tag)
        : m_tag(tag)
    {
    }

    LZDataBase::LZDataBase(const tstring & tag, const tstring & value_)
        : m_tag(tag)
        , StringConvTool(value_)
    {
    }

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

//获得子孙结点属性。支持a.b.c的形式。
    LZDataPtr LZDataBase::read(const tstring & tag)
    {
        size_t pos = tag.find_first_of('/');
        int i = findChild(tag.substr(0, pos));
        if (i < 0)
        {
            return NULL;
        }

        if (pos == tag.npos)
        {
            return getChild(i);
        }
        else
        {
            return getChild(i)->read(tag.substr(pos + 1));
        }
    }
    LZDataPtr LZDataBase::write(const tstring & tag, tstring value_/*=""*/)
    {
        size_t pos = tag.find_first_of('/');
        if (tag.npos == pos)
        {
            int i = findChild(tag);
            if (i >= 0)
            {
                getChild(i)->setString(value_);
            }
            else
            {
                i = addChild(newOne(tag, value_));
            }
            return getChild(i);
        }
        else
        {
            tstring lName = tag.substr(0, pos);
            tstring rName = tag.substr(pos + 1);
            int i = findChild(lName);
            if (i < 0)
            {
                i = addChild(newOne(lName, _T("")));
            }
            return getChild(i)->write(rName, value_);
        }
    }

    LZDataPtr LZDataBase::newChild(const tstring & tag, const tstring & value_)
    {
        LZDataPtr child = newOne(tag, value_);
        addChild(child);
        return child;
    }

    int LZDataBase::findNextChild(int index, const tstring & tag)
    {
        index += 1;
        if (index < 0 || index >= countChildren())
        {
            return -1;
        }
        for (int i = index; (size_t)i < m_children.size(); ++i)
        {
            if (m_children[i]->tag() == tag)
            {
                return i;
            }
        }
        return -1;
    }

    bool LZDataBase::load(const tstring & fname)
    {
        std::string buffer;
        if(!getfs()->readString(fname, buffer)) return false;

        std::wstring fileBuffer;
        if(!charToWChar(fileBuffer, buffer)) return false;

        return loadFromBuffer(&fileBuffer[0], fileBuffer.size());
    }

    bool LZDataBase::save(const tstring & fname)
    {
        std::wostringstream stream;
        print(stream, 0);

        std::string buffer;
        if(!wcharToChar(buffer, stream.str(), CP::utf8)) return false;

        return getfs()->writeString(fname, buffer);
    }

}//namespace Lazy