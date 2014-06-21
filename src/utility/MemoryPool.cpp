#include "StdAfx.h"
#include "Misc.h"
#include "MemoryPool.h"
#include "MemoryCheck.h"
#include <sstream>

namespace Lazy
{
    typedef DWORD uptr;

    ///内存分配步长，必须是2的n次方
    const size_t BlockStep = 8;
    const size_t BlockMark = BlockStep - 1;
    const size_t BlockShift = 3;

    const size_t MaxBlockSize = 256;
    const size_t NbAllocator = MaxBlockSize / BlockStep;

    const size_t PoolSize = 4 << 10; //4k
    const size_t PoolSizeMark = PoolSize - 1;

    const size_t AreaSize = 256 << 10; //256k
    const size_t AreaSizeMark = AreaSize - 1;

    ///按BlockStep字节对齐
    size_t roundup(size_t size)
    {
        return (size + BlockMark) & ~BlockMark;
    }

    size_t size2index(size_t size)
    {
        return (size - 1) >> BlockShift;
    }

    size_t index2size(size_t index)
    {
        return (index + 1) << BlockShift;
    }


    void * allocMemory(size_t n)
    {
        return malloc(n ? n : 1);
    }

    void freeMemory(void* ptr)
    {
        free(ptr);
    }

    void * reallocMemory(void *p, size_t size)
    {
        return realloc(p, size);
    }

    /*static*/ MemoryPool * MemoryPool::s_pInstance;

    MemoryPool::MemoryPool()
        : m_arenas(nullptr)
        , m_unusedArena(nullptr)
        , m_arenasize(0)
        , m_pools(NbAllocator)
        , m_nAllocateArena(0)
        , m_usedSize(0)
    {
        for (Pool & p : m_pools) p.make_cycle();

        m_usableArena.make_cycle();
    }

    MemoryPool::~MemoryPool()
    {
        tstring memoryinfo;
        dump(memoryinfo);
        debugMessage(memoryinfo.c_str());

        if (nullptr == m_arenas) return;

        for (size_t i = 0; i < m_arenasize; ++i)
        {
            if (m_arenas[i].addr)
                freeMemory(m_arenas[i].addr);
        }

        freeMemory(m_arenas);
    }

    /*static*/ MemoryPool * MemoryPool::instance()
    {
        if (!s_pInstance)
        {
            throw(std::runtime_error("memory pool doesn't init!"));
        }
        return s_pInstance;
    }

    ///初始化
    /*static*/ void MemoryPool::init()
    {
        if (!s_pInstance)
        {
            s_pInstance = new MemoryPool();
            debugMessage(_T("Memory instance created!"));
        }
    }

    /*static*/ void MemoryPool::fini()
    {
        if (s_pInstance)
        {
            delete s_pInstance;
            s_pInstance = nullptr;

            debugMessage(_T("Memory instance destroy!"));
        }
    }

    //分配内存
    void *MemoryPool::alloc(size_t size)
    {
        size_t index = size2index(size);

        //如果超出最大限度，直接在堆上分配。
        if (index >= NbAllocator)
            return allocMemory(size);

        ZLockHolder holder(&m_lock);

        //找到该尺寸的分配器
        Pool * header = &m_pools[index];
        Pool * allocator = (Pool*) header->next;
        if (allocator == header) //空链表
        {
            allocator = newPool(index);
            if (!allocator) return nullptr;

            header->link_next(allocator);
        }

        //分配一块内存
        Block *p = allocator->freeBlock;
        allocator->freeBlock = p->nextBlock;
        ++allocator->count;

        //用完了。移除掉
        if (nullptr == allocator->freeBlock)
        {
            allocator->unlink();
        }

        m_usedSize += index2size(index);
        return p;
    }

    //释放内存
    void MemoryPool::free(void *ptr)
    {
        ZLockHolder holder(&m_lock);

        Pool *pool = (Pool*) ((uptr) ptr & ~PoolSizeMark);
        size_t arenaIndex = pool->arenaIndex;

        if (arenaIndex >= m_arenasize)
        {
            freeMemory(ptr);
            return;
        }

        //校验是不是内存池分配的内存
        Arena * arena = &m_arenas[arenaIndex];
        uptr n = (uptr) ptr - (uptr) arena->addr;
        if (n > AreaSize)
        {
            freeMemory(ptr);
            return;
        }

        //归还到pool中
        Block * lastblock = pool->freeBlock;
        ((Block*) ptr)->nextBlock = pool->freeBlock;
        pool->freeBlock = (Block*) ptr;
        --pool->count;

        if (nullptr == lastblock)
        {
            //case 1. 之前pool已满，被移走了，现在需要加入到可用表中
            m_pools[pool->sizeIndex].link_next(pool);
        }
        else if (pool->count == 0)
        {
            //case 2. 现在pool已经空了，需要释放掉
            releasePool(pool);
        }
        else
        {
            //case 3. 什么也不做
        }

        m_usedSize -= index2size(pool->sizeIndex);
    }

    Arena * MemoryPool::newArena()
    {
        if (nullptr == m_unusedArena) //empty
        {
            size_t numarena = m_arenasize;
            if (nullptr == m_arenas)
            {
                //初始化arena
                m_arenasize = 16;
                m_arenas = (Arena*) allocMemory(sizeof(Arena) * m_arenasize);
            }
            else
            {
                //重新分配。不用担心Arena.next地址失效问题，因为满的arena不会使用到他的next。
                m_arenasize <<= 1;
                m_arenas = (Arena*) reallocMemory(m_arenas, sizeof(Arena) * m_arenasize);
            }

            //内存分配失败
            if (nullptr == m_arenas) return nullptr;

            for (size_t i = numarena; i < m_arenasize; ++i)
            {
                m_arenas[i].addr = nullptr;
                m_arenas[i].next = i < m_arenasize - 1 ? &m_arenas[i + 1] : nullptr;
            }

            m_unusedArena = &m_arenas[numarena];
        }

        Arena * arena = m_unusedArena;
        if (nullptr == arena->addr)
        {
            arena->addr = allocMemory(AreaSize);

            ++m_nAllocateArena;
            if (nullptr == arena->addr) return nullptr;//内存分配失败
        }
        m_unusedArena = (Arena*) arena->next;


        size_t ntotal = AreaSize / PoolSize;
        arena->freePool = (Pool*) arena->addr;

        //将内存对齐到pool的边界
        size_t excess = size_t(arena->addr) & PoolSizeMark;
        if (excess != 0)
        {
            arena->freePool = (Pool*) ((char*) arena->addr + PoolSize - excess);
            --ntotal;
        }
        arena->nfreepools = arena->ntotalpools = ntotal;

        //填充freepool
        Pool * freePool = arena->freePool;
        while (--ntotal)
        {
            freePool->next = (Pool*) ((char*) freePool + PoolSize);
            freePool = (Pool*) freePool->next;
        }
        freePool->next = nullptr;

        return arena;
    }

    Pool * MemoryPool::newPool(size_t index)
    {
        //当前arena不可用，重新分配一个
        Arena * arena = (Arena*) m_usableArena.next;
        if (arena == &m_usableArena)
        {
            arena = newArena();
            if (!arena) return nullptr;//分配失败

            m_usableArena.link_next(arena);
        }

        //从arena中取一个pool
        Pool *pool = arena->freePool;
        arena->freePool = (Pool*) pool->next;
        if (0 == --arena->nfreepools)//arena被用尽了，删掉
        {
            arena->unlink();
        }

        //初始化pool
        pool->arenaIndex = arena - m_arenas;
        pool->sizeIndex = index;

        size_t blockSize = index2size(index);
        size_t headerSize = roundup(sizeof(Pool));
        size_t totalBlock = (PoolSize - headerSize) / blockSize;
        pool->freeBlock = (Block*) ((char*) pool + headerSize);
        pool->count = 0;

        //填充freeBlock
        Block *freeBlock = pool->freeBlock;
        while (--totalBlock)
        {
            freeBlock->nextBlock = (Block*) ((char*) freeBlock + blockSize);
            freeBlock = freeBlock->nextBlock;
        }
        freeBlock->nextBlock = nullptr;

        return pool;
    }

    void MemoryPool::releasePool(Pool *pool)
    {
        Arena * arena = &m_arenas[pool->arenaIndex];

        //从原有的pool链表中断开
        pool->unlink();

        //添加到arena的freePool中
        pool->next = arena->freePool;
        arena->freePool = pool;
        ++arena->nfreepools;

        //arena也空了，需要释放arena空间
        if (arena->nfreepools == arena->ntotalpools)
        {
            arena->unlink();

            //TODO 内存容量达到一定限度时，进行释放
            //freeMemory(arena->addr);
            //arena->addr = nullptr;
            //--m_nAllocateArena;

            if (m_unusedArena != nullptr)
            {
                arena->next = m_unusedArena;
            }
            m_unusedArena = arena;
        }
    }

    size_t MemoryPool::numUsed() const
    {
        return m_usedSize;
    }

    ///内存容量
    size_t MemoryPool::numCapacity() const
    {
        return m_nAllocateArena * AreaSize;
    }

    ///总共占用的内存
    size_t MemoryPool::numTotalBytes() const
    {
        size_t size = sizeof(MemoryPool);
        size += sizeof(Arena) * m_arenasize;
        size += sizeof(Pool) * m_pools.capacity();
        size += numCapacity();
        return size;
    }

    void MemoryPool::dump(tstring & info) const
    {
        std::wstringstream ss;
        ss << "memory info\n"
           << "total bytes: " << numTotalBytes() << "\n"
           << "capacity bytes: " << numCapacity() << "\n"
           << "used bytes: " << numUsed() << "\n"
           << "num allocated arena: " << m_nAllocateArena << "\n";
        info = ss.str();
    }

}//end namespace Lazy