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

class LZDATA_API lzd : public LZDataBase
{
public:

    lzd(void);

    lzd(const tstring & tag);

    lzd(const tstring & tag, const tstring & value_);

    ~lzd(void);

    virtual void print(tostream & out, int depth=0) ;

    virtual LZDataPtr newOne(const tstring & tag, const tstring & value_);

    virtual bool loadFromBuffer(const tchar* buffer, int length);

    virtual bool load(const tstring & fname);

    virtual bool save(const tstring & fileName) ;
private:
    lzd(const lzd&);
    const lzd& operator=(const lzd&);
};


}//namespace Lazy