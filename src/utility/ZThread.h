//////////////////////////////////////////////////////////////////////////
/*
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "UtilConfig.h"
#include "SmartPtr.h"
#include "ZLock.h"

namespace Lazy
{
///线程基类
    class ZThreadBase : public IBase
    {
    public:
        explicit ZThreadBase(bool start = true);
        virtual ~ZThreadBase(void);

        ///启动线程
        void start();

        ///暂停线程
        void pause();

        ///执行任务。执行完毕，线程就退出了。
        virtual void run() = 0;

    private:
        HANDLE m_hThread; ///<线程句柄

        static uint32 __stdcall _threadCall(void* param);
    };

    typedef RefPtr<ZThreadBase> ZThreadPtr;
    typedef void(*ZThreadFunc)(void*);


///扩展线程类
    template<typename F>
    class ZThread : public ZThreadBase
    {
        F m_f;
    public:

        ZThread(F f)
            : m_f(f)
        {}

        virtual void run()
        {
            m_f();
        }
    };


///普通线程类
    template<>
    class ZThread<ZThreadFunc> : public ZThreadBase
    {
    public:
        ZThread(ZThreadFunc pf, void *param)
            : m_pFunc(pf)
            , m_pParam(param)
        {
            assert(pf && "ZThread constructor");
        }

        virtual void run()
        {
            m_pFunc(m_pParam);
        }

    private:
        ZThreadFunc m_pFunc;
        void*       m_pParam;
    };


///扩展线程类.用于调用类成员函数。
    template<typename T>
    class ZThreadEx : public ZThreadBase
    {
    public:
        typedef void(T::*ZFuncPtr)(void);

        ZThreadEx(T *pobj, ZFuncPtr func)
            : m_obj(pobj)
            , m_func(func)
        {
            assert(pobj && func && "ZThreadEx constructor");
        }

        virtual void run()
        {
            (m_obj->*m_func)();
        }

    private:
        T           *m_obj;
        ZFuncPtr     m_func;
    };

}//namespace Lazy