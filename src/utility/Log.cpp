//////////////////////////////////////////////////////////////////////////
/*
* author: youlanhai
* e-mail: you_lan_hai@foxmail.com
* blog: http://blog.csdn.net/you_lan_hai
* data: 2012-2013
*/
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Log.h"
#include "Misc.h"

#include <cassert>

namespace Lazy
{

    const char * unicodeHeader = "\xFF\xFE";

    const wchar_t * newLine = L"\r\n";
    const size_t sizeNewLine = wcslen(newLine);

    cLog g_globle_log_;

    //////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////

    cLog::cLog(void)
        : m_level(0)
        , m_file(nullptr)
    {
    }

    cLog::~cLog(void)
    {
        unload();
    }

    bool cLog::init(const tstring & fileName)
    {
        assert(m_file == nullptr && "Initialize more than once!");
        
        errno_t err = _wfopen_s(&m_file, fileName.c_str(), L"wb");
        if (err != 0)
        {
            debugMessage(L"ERROR: open log file '%s' failed! code:%d", fileName.c_str(), err);
            return false;
        }

        //以unicode格式存储数据
        fwrite(unicodeHeader, 1, 2, m_file);

        writeW(L"------ LAZY LOG SYSTEM START ------\r\n");

        return true;
    }

    void cLog::unload(void)
    {
        if (!m_file) return;

        writeW(L"****** LAZY LOG SYSTEM STOPED ******\r\n");

        fclose(m_file);
        m_file = nullptr;
    }

    void cLog::generateTimeHead(tstring & header)
    {
        static SYSTEMTIME systm;
        GetLocalTime(&systm);

        formatString(header, _T("[%d-%d-%d %2.2d:%2.2d:%2.2d]"),
            systm.wYear, systm.wMonth, systm.wDay,
            systm.wHour, systm.wMinute, systm.wSecond);
    }

    void cLog::writeA(const std::string & buffer)
    {
        write(nullptr, charToWChar(buffer));
    }

    void cLog::writeW(const std::wstring & buffer)
    {
        write(nullptr, buffer);
    }

    void cLog::write(LPCWSTR pre, const std::wstring & msg)
    {
        if (!m_file) return;
        if (msg.empty()) return;

//#ifdef _DEBUG
        if (pre) debugMessageW(L"%s%s\n", pre, msg.c_str());
        else OutputDebugStringW((msg + newLine).c_str());
//#endif

        ZLockHolder holder(&m_writeMutex);

        tstring header;
        generateTimeHead(header);

        fwrite(header.c_str(), 2, header.length(), m_file);

        if (pre) fwrite(pre, 2, wcslen(pre), m_file);

        fwrite(msg.c_str(), 2, msg.length(), m_file);
        fwrite(newLine, 2, sizeNewLine, m_file);
    }

    void cLog::xwriteA(LPCSTR format, ...)
    {
        std::string buffer;

        va_list vl;
        va_start(vl, format);
        formatStringVSA(buffer, format, vl);
        va_end(vl);

        writeA(buffer);
    }


#define LOG_WRITE_VS(lvl, pre)      \
    if (m_level > lvl) return;      \
    std::wstring buffer;            \
    va_list vl;                     \
    va_start(vl, format);           \
    formatStringVSW(buffer, format, vl);    \
    va_end(vl);                     \
    write(pre, buffer)


    void cLog::xwriteW(LPCWSTR format, ...)
    {
        LOG_WRITE_VS(0, nullptr);
    }

    void cLog::debug(LPCWSTR format, ...)
    {
        LOG_WRITE_VS(LogLvl::debug, L"DEBUG: ");
    }
    
    void cLog::info(LPCWSTR format, ...)
    {
        LOG_WRITE_VS(LogLvl::info, L"INFO: ");
    }
    
    void cLog::warning(LPCWSTR format, ...)
    {
        LOG_WRITE_VS(LogLvl::warning, L"WARNING: ");
    }

    void cLog::error(LPCWSTR format, ...)
    {
        LOG_WRITE_VS(LogLvl::error, L"ERROR: ");
    }

}//namespace Lazy