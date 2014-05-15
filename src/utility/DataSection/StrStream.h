#pragma once

#include "../UtilConfig.h"
#include "../SmartPtr.h"

namespace Lazy
{

    class StrStream : public IBase
    {
    public:
        StrStream(const tchar* buffer, int size)
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

    typedef RefPtr<StrStream> StrStreamPtr;

}