//////////////////////////////////////////////////////////////////////////
/*
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

//#define  LZDATA_DYNAMIC

#ifdef LZDATA_DYNAMIC
#   ifdef LZDATA_EXPORTS
#       define LZDATA_API __declspec(dllexport)
#   else
#       define LZDATA_API __declspec(dllimport)
#   endif
#else
#   define LZDATA_API
#endif

#include <wtypes.h>
#include <tchar.h>
#include <cassert>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <ostream>
#include <algorithm>
