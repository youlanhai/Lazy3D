//////////////////////////////////////////////////////////////////////////
/*
* author: youlanhai
* e-mail: you_lan_hai@foxmail.com
* blog: http://blog.csdn.net/you_lan_hai
* data: 2012-2013
*/
//////////////////////////////////////////////////////////////////////////
#ifndef LAZY_UTILITY_CONFIG_H
#define LAZY_UTILITY_CONFIG_H

#define NOMINMAX
#include <Windows.h>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#include <tchar.h>

#include <memory>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cstdlib>

namespace Lazy
{
    typedef unsigned char       uchar;
    typedef signed char         int8;
    typedef unsigned char       uint8;
    typedef short               int16;
    typedef unsigned short      uint16;
    typedef int					int32;
    typedef unsigned int        uint32;
    typedef __int64             int64;
    typedef unsigned __int64    uint64;


    const std::string   EmptyStrA;
    const std::wstring  EmptyStrW;

#ifdef _UNICODE

    typedef wchar_t         tchar;
    typedef std::wstring    tstring;
    typedef std::wostream   tostream;
    typedef std::wifstream  tifstream;
    typedef std::wofstream  tofstream;

#define lzstricmp       _wcsicmp
#define lzstrcmp        wcscmp
#define lzstrncmp       wcsncmp
#define lzstrlen        wcslen
#define lzstrcpy        wcscpy

#else

    typedef char            tchar;
    typedef std::string     tstring;
    typedef std::ostream    tostream;
    typedef std::ifstream   tifstream;
    typedef std::ofstream	tofstream;

#define lzstricmp       stricmp
#define lzstrcmp        strcmp
#define lzstrncmp       strncmp
#define lzstrlen        strlen
#define lzstrcpy        strcpy

#endif

    typedef std::vector<tstring> StringPool;
    const tstring EmptyStr = _T("");

    template<typename T>
    T min(T a, T b)
    {
        return a < b ? a : b;
    }

    template<typename T>
    T max(T a, T b)
    {
        return a > b ? a : b;
    }

}//namespace Lazy


#define SLASH_USE _T('/')
#define SLASH_OLD _T('\\')

#define SAFE_CLOSE_HANDLE(h)    if(h){ CloseHandle(h); h = NULL; }
#define SAFE_RELEASE(p)         if(p){ p->Release(); p = NULL; }

#define SafeRelease(p)      if(p){ p->Release(); p = NULL; }
#define SafeDelete(p)       if(p){ delete p; p = NULL;}
#define SafeDeleteArray(p)  if(p){ delete [] p; p = NULL;}

#define FOR_EACH(pool, it) for(it=pool.begin(); it!=pool.end(); ++it)

#define MAKE_UNCOPY(cl) \
private:                \
    cl(const cl&);      \
    const cl& operator=(const cl&) ;

/*是否需要使用多线程*/
#define USE_MULTI_THREAD

/*打印内存检测的所有信息*/
//#define DEBUG_ALL_MEMORY_CHECK



#include "SmartPtr.h"


#endif //LAZY_UTILITY_CONFIG_H