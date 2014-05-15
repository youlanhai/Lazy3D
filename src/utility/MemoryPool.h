#pragma once

#include "ZLock.h"

namespace Lazy
{
    ///小块
    struct Block
    {
        Block * nextBlock;
    };

    ///双向链表结点
    struct LinkNode
    {
        LinkNode * prev;
        LinkNode * next;

        ///构成一个自循环的空链表
        void make_cycle()
        {
            prev = next = this;
        }

        ///将next结点链接到当前结点之后
        void link_next(LinkNode * next)
        {
            next->next = this->next;
            next->prev = this;

            this->next->prev = next;
            this->next = next;
        }

        ///将自己从所在链表中移除
        void unlink()
        {
            prev->next = next;
            next->prev = prev;
        }
    };

    ///内存条。block的集合。pool信息在其所管理的内存首部，你可以认为pool是一个变长的对象。
    struct Pool : public LinkNode
    {
        size_t  count;      ///< 使用计数。每分配出去一个freeBlock，coun就加1。相反，每释放一个就减1。
        size_t  arenaIndex; ///< 当前pool所处的arena索引
        size_t  sizeIndex;  ///< 当前pool所处的分配池索引
        Block   *freeBlock; ///< 空闲block
    };

    ///内存区域。记录着大片内存空间，为pool的集合。arena与其所管理的内存是分离的，
    ///addr就是大块内存的地址。只有需要用用到的时候，才会分配大内存，记录在addr。
    struct Arena : public LinkNode
    {
        void    *addr;      ///< 内存地址
        Pool    *freePool;  ///< 可用的pool

        size_t  nfreepools; ///< 可用pool的个数
        size_t  ntotalpools;///< pool的总个数
    };

    ///内存池
    class MemoryPool
    {
    public:

        ///单例
        static MemoryPool * instance();

        ///初始化
        static void init();

        ///释放单例
        static void fini();

        ///分配内存
        void *alloc(size_t size);

        ///释放内存
        void free(void *ptr);

        ///已经分配内存量
        size_t numUsed() const;

        ///总内存容量
        size_t numCapacity() const;

        ///整个内存池体系，所占用的内存
        size_t numTotalBytes() const;

        ///输出内存池信息到字符串
        void dump(tstring & info) const;

    private:

        MemoryPool();
        ~MemoryPool();

        ///如果可用内存不足，分配一块新的area。
        Arena * newArena();

        Pool * newPool(size_t index);

        void releasePool(Pool *p);

    private:
        size_t  m_usedSize;         ///< 已经分配出去的内存量
        size_t  m_nAllocateArena;   ///< 已经分配内存的arena个数
        Arena   m_usableArena;      ///< 当前可用的arena链表头结点，是一个双向链表
        Arena   *m_unusedArena;     ///< 未使用的arena链表头结点，是一个单向链表
        Arena   *m_arenas;          ///< arena数组。这是一个变长的数组。
        size_t  m_arenasize;        ///< arena数量
        std::vector<Pool> m_pools;  ///< 分配不同尺寸的内存。每个元素为一个双向链表的头结点

#ifdef USE_MULTI_THREAD
        ZCritical m_lock;           ///< 多线程同步锁
#else
        DummyLock m_lock;
#endif

        static MemoryPool * s_pInstance;
    };

//内存池分配申明
#define DEC_MEMORY_ALLOCATER()      \
void* operator new (size_t n) { return ::Lazy::MemoryPool::instance()->alloc(n); }      \
void operator delete(void *ptr) { ::Lazy::MemoryPool::instance()->free(ptr); }          \
virtual void destroyThis(void){ delete this; }

}//end namespace Lazy