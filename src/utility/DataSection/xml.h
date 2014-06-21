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

    class LZDATA_API xml : public LZDataBase
    {
    public:
        xml(void);

        explicit xml(const tstring & tag);

        xml(const tstring & tag, const tstring & value);

        virtual ~xml(void);

        virtual bool loadFromBuffer(const tchar* buffer, int length);

        virtual LZDataPtr newOne(const tstring & tag, const tstring & value_);

        virtual void print(tostream & out, int depth = 0) ;

    private:
        xml(const xml&);
        const xml& operator=(const xml&);
    };


}//namespace Lazy