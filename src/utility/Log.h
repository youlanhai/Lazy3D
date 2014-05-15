//////////////////////////////////////////////////////////////////////////
/*
* author: youlanhai
* e-mail: you_lan_hai@foxmail.com
* blog: http://blog.csdn.net/you_lan_hai
* data: 2012-2013
*/
//////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef LAZY_LOG_H
#define LAZY_LOG_H

#include "ZLock.h"

namespace Lazy
{
    ///日志等级
    namespace LogLvl
    {
        const int debug = 1;
        const int info = 2; 
        const int warning = 5;
        const int error = 8;
        const int disable = 100;
    }

    ///日志系统
    class cLog
    {
    public:

        cLog(void);
        ~cLog(void);

        ///启动日志
        bool init(const tstring & fileName);

        ///停止日志
        void unload(void);

        ///日志系统是否可用
        bool isUsefull(void) const { return m_file != nullptr; }

    public:

        void writeA(const std::string & buffer);

        void writeW(const std::wstring & buffer);

        void xwriteA(LPCSTR format, ...);

        void xwriteW(LPCWSTR format, ...);

    public:

        void debug(LPCWSTR format, ...);
        void info(LPCWSTR format, ...);
        void warning(LPCWSTR format, ...);
        void error(LPCWSTR format, ...);

    protected:

        void generateTimeHead(tstring & header);

        void write(LPCWSTR pre, const std::wstring & msg);

    private:
        int     m_level;    ///<输出等级
        FILE    *m_file;    ///<输出文件流

#ifdef USE_MULTI_THREAD
        ZCritical       m_writeMutex;
#else
        DummyLock       m_writeMutex;
#endif
    };

    extern cLog g_globle_log_;

}//namespace Lazy


/**初始日志系统。*/
#define  INIT_LOG(fileName)			Lazy::g_globle_log_.init(fileName)

/**卸载日志系统*/
#define  UNLOAD_LOG()				Lazy:: g_globle_log_.unload()

#define LOG_WRITE(format, ...)      Lazy::g_globle_log_.xwriteW(format, __VA_ARGS__)
#define LOG_DEBUG(format, ...)      Lazy::g_globle_log_.debug(format, __VA_ARGS__)
#define LOG_INFO(format, ...)       Lazy::g_globle_log_.info(format, __VA_ARGS__)
#define LOG_WARNING(format, ...)    Lazy::g_globle_log_.warning(format, __VA_ARGS__)
#define LOG_ERROR(format, ...)      Lazy::g_globle_log_.error(format, __VA_ARGS__)




//以下为兼容旧接口

#define  WRITE_LOGA(buffer)			Lazy::g_globle_log_.writeA(buffer)
#define  WRITE_LOGW(buffer)			Lazy::g_globle_log_.writeW(buffer)
#define  XWRITE_LOGA(format, ...)   Lazy::g_globle_log_.xwriteA(format, __VA_ARGS__)
#define  XWRITE_LOGW(format, ...)   Lazy::g_globle_log_.xwriteW(format, __VA_ARGS__)

#ifdef _UNICODE
#   define XWRITE_LOG   XWRITE_LOGW
#   define WRITE_LOG    WRITE_LOGW
#else
#   define XWRITE_LOG   XWRITE_LOGA
#   define WRITE_LOG    WRITE_LOGA
#endif

#endif