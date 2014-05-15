//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

namespace Lazy
{

#include <process.h>

///多线程锁基类
struct ZLockBase
{
    ///加锁
    virtual void lock() = 0;

    ///解锁
    virtual void unlock() = 0;
};

///假锁。非多线程环境下执行空操作
struct DummyLock : public ZLockBase
{
    virtual void lock() {}
    virtual void unlock() {}
};

///简单锁
class ZSimpleLock : public ZLockBase
{
public:
    ZSimpleLock();

    void lock();
    void unlock();

private:
	LONG m_mark;
};

///临界区
class ZCritical : public ZLockBase
{
public:
    ZCritical();
    ~ZCritical();

    void lock();
    void unlock();

private:
    CRITICAL_SECTION m_cs;
};

///互斥锁
class ZMutex : public ZLockBase
{
public:
    ZMutex(void);

    ~ZMutex(void);

    void lock();

    void unlock();
private:
    HANDLE m_hMutex;
};

///锁保持者。在对象构造时会上锁，析构时会解锁。
class ZLockHolder
{
public:
    ZLockHolder(ZLockBase *locker) 
        : m_locker(locker) 
    {
        m_locker->lock();
    }

    ~ZLockHolder(){ m_locker->unlock(); }
private:
    ZLockBase *m_locker;
};

//////////////////////////////////////////////////////////////////////////
///信号量
class ZSemaphore
{
public:
    ZSemaphore();
    ~ZSemaphore();

    void push();

    void pop();

private:
    HANDLE m_hSemaphore;
};

}//namespace Lazy