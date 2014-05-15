//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "LZDataBase.h"

namespace Lazy
{

class WrapperStream : public IBase
{
public:
    WrapperStream(const tchar* buffer, int size)
        : m_buffer(buffer)
        , m_size(size)
        , m_index(0)
    {

    }
    int index(){ return m_index; }

    tchar curchar() const
    {
        if (empty())
        {
            return 0;
        }
        return m_buffer[m_index];
    }

    tchar getchar()
    {
        if (empty())
        {
            return 0;
        }
        return m_buffer[m_index++];
    }

    void ignore(int n)
    {
        m_index += n;
    }

    void seek(int index_)
    {
        m_index = index_;
    }

    bool empty() const
    {
        return m_index >= m_size;
    }

    int find(char ch)
    {
        for (int i=m_index; i<m_size; ++i)
        {
            if (m_buffer[i] == ch)
            {
                return i;
            }
        }
        return -1;
    }
    bool getRange(tstring & out, int begin, int end)
    {
        if (begin < 0 || end > m_size)
        {
            return false;
        }
        out.assign(m_buffer+begin, end - begin);
        return true;
    }

private:
    const tchar*    m_buffer;
    int             m_size;
    int             m_index;
};


class xmlParser
{
public:
    xmlParser(const tchar* buffer, int size);
    ~xmlParser(void);

    bool parse(LZDataPtr parent=NULL);

    LZDataPtr root(){ return m_root; }

protected:
    bool parseReal(LZDataPtr parent);

private:
    LZDataPtr m_root;
    WrapperStream m_stream;
};


}//namespace Lazy