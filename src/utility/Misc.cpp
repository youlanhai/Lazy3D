//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
//misc.cpp 

#include "stdafx.h"
#include "Misc.h"
#include "FileTool.h"
#include <Objbase.h>
#include <MMSystem.h>
#pragma comment(lib, "winmm.lib")

namespace Lazy
{
DWORD defaultCodePage = 0;

const tstring & cpToCode(size_t cp)
{
    if(cp == CP::utf7) return Encode::utf7;
    else if(cp == CP::utf8) return Encode::utf8;
    else if(cp == CP::gbk) return Encode::gbk;
    else if(cp == CP::big5) return Encode::big5;
    else if(cp == CP::ucs_2le) return Encode::ucs_2le;
    else if(cp == CP::ucs_2be) return Encode::ucs_2be;
    else if(cp == CP::ucs2) return Encode::ucs2;
    else return Encode::None;
}

size_t codeToCp(const tstring & code)
{
    if(code == Encode::utf7) return CP::utf7;
    else if(code == Encode::utf8) return CP::utf8;
    else if(code == Encode::gbk) return CP::gbk;
    else if(code == Encode::big5) return CP::big5;
    else if(code == Encode::ucs_2le) return CP::ucs_2le;
    else if(code == Encode::ucs_2be) return CP::ucs_2be;
    else if(code == Encode::ucs2) return CP::ucs2;
    else return CP::None;
}

std::wstring charToWChar(const std::string & str, DWORD code)
{
    std::wstring wstr;
    charToWChar(wstr, str, code);
    return wstr;
}

std::string wcharToChar(const std::wstring & wstr, DWORD code)
{
    std::string str;
    wcharToChar(str, wstr, code);
    return str;
}

bool charToWChar(std::wstring & dest, const std::string & str, DWORD code)
{
    long lLen = MultiByteToWideChar(code, 0, str.c_str(), -1, NULL, 0) + 1; 

    dest.resize(lLen, 0);

    lLen = MultiByteToWideChar(code, 0, str.c_str(), -1, &dest[0], lLen);
    eraseToEnd(dest, 0);

    return true;
}

bool wcharToChar(std::string & dest, const std::wstring & wstr, DWORD code)
{
    long lLen = WideCharToMultiByte(code, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL) + 1;
    dest.resize(lLen);

    lLen = WideCharToMultiByte(code, 0, wstr.c_str(), -1, &dest[0], lLen, NULL, NULL);
    eraseToEnd(dest, 0);

    return true;
}

///改变工作路径
bool changeCurDirectory()
{
    return getfs()->setcwd(EmptyStr);
}

void generateGUID(tstring & guidStr)
{
    GUID guid;
    if (S_OK == ::CoCreateGuid(&guid))
    {
        formatString(guidStr
            , _T("%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X")
            , guid.Data1, guid.Data2, guid.Data3
            , guid.Data4[0], guid.Data4[1]
            , guid.Data4[2], guid.Data4[3]
            , guid.Data4[4], guid.Data4[5]
            , guid.Data4[6], guid.Data4[7]
        );
    }
}

//生成guid
tstring generateGUID()
{
    tstring str;
    generateGUID(str);
    return str;
}

bool generateGUID32(tstring & strGuid)
{
    GUID guid;
    if (S_OK != ::CoCreateGuid(&guid)) return false;

    formatString(strGuid
        , _T("%08x%04x%04x%02x%02x%02x%02x%02x%02x%02x%02x")
        , guid.Data1, guid.Data2, guid.Data3
        , guid.Data4[0], guid.Data4[1]
        , guid.Data4[2], guid.Data4[3]
        , guid.Data4[4], guid.Data4[5]
        , guid.Data4[6], guid.Data4[7]
    );

    return true;
}

float getCurTime()
{
    return timeGetTime() * 0.001f;
}

//4字符构成一个魔法数
size_t makeMagic(char a, char b, char c, char d)
{
    return a | b << 8 | c << 16 | d<< 24;
}

//////////////////////////////////////////////////////////////////////////
//修整字符串
//////////////////////////////////////////////////////////////////////////
template<typename STRING>
void _trimString(STRING & str, const STRING & filter)
{
	int start = 0;
	int end = int(str.size()) - 1;

	for(; start<=end ; ++start)
	{
		if (filter.find(str[start]) == filter.npos)
		{
			break;
		}
	}

	for(; end>=start ; --end)
	{
		if (filter.find(str[end]) == filter.npos)
		{
			break;
		}
	}

	if (start > end)
	{
		str.clear();
	}
	else
	{
		str = str.substr(start, end + 1 - start);
	}
}

//去除string两端空白符。 filter : 要过滤掉的字符集和
void trimStringA(std::string & str, const std::string & filter/*=" \t\r\n"*/)
{
	_trimString(str, filter);
}

//去除string两端空白符。 filter : 要过滤掉的字符集和
void trimStringW(std::wstring & str, const std::wstring & filter/*=L" \t\r\n"*/)
{
	_trimString(str, filter);
}

//////////////////////////////////////////////////////////////////////////
//格式化字符串
//////////////////////////////////////////////////////////////////////////
#pragma warning(push)
#pragma warning(disable: 4996)

bool formatStringVSW(std::wstring & dest, LPCWSTR pFormat, va_list pArgList)
{
	int len = _vscwprintf(pFormat, pArgList) + 1;
	if(len < 0)
	{
		return false;
	}
	dest.resize(len);

	len = vswprintf(&dest[0], pFormat, pArgList);

	if(len < 0)
	{
		return false;
	}

	dest.erase(len);
	return true;
}

bool formatStringVSA(std::string & dest, LPCSTR pFormat, va_list pArgList)
{
	int len = _vscprintf(pFormat, pArgList) + 1;
	if(len < 0)
	{
		return false;
	}
	dest.resize(len);

	len = vsprintf(&dest[0], pFormat, pArgList);

	if(len < 0)
	{
		return false;
	}

	dest.erase(len);
	return true;
}

#pragma warning(pop)

bool formatStringA(std::string & dest, LPCSTR pFormat, ...)
{
	va_list pArgList;
	va_start (pArgList, pFormat);

	bool ret = formatStringVSA(dest, pFormat, pArgList);

	va_end(pArgList);

	return ret;
}

bool formatStringW(std::wstring & dest, LPCWSTR pFormat, ...)
{
	va_list pArgList;
	va_start (pArgList, pFormat);

	bool ret = formatStringVSW(dest, pFormat, pArgList);

	va_end(pArgList);

	return ret;
}

void debugMessageA(LPCSTR pFormat, ...)
{
	std::string dest;

	va_list pArgList;
	va_start (pArgList, pFormat);

	if(formatStringVSA(dest, pFormat, pArgList))
	{
		OutputDebugStringA(dest.c_str());
	}

	va_end(pArgList);
}

void debugMessageW(LPCWSTR pFormat, ...)
{
	std::wstring dest;

	va_list pArgList;
	va_start (pArgList, pFormat);

	if(formatStringVSW(dest, pFormat, pArgList))
	{
		OutputDebugStringW(dest.c_str());
	}

	va_end(pArgList);
}

}//namespace Lazy