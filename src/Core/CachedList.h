///mlist.h
#pragma once


#ifndef LISTCOMPARE
#define LISTCOMPARE

namespace Lazy
{

    //元素比较
    template<typename Type>
    struct ListCompare
    {
        bool operator ()(const Type & t1, const Type & t2)
        {
            if(t1 == t2)
            {
                return true;
            }
        }
    };

    /** 链表结点*/
    template<typename Type>
    struct ListNode
    {
        Type data;
        struct ListNode *next;

        ListNode(void): next(NULL) { }
    };

    /**
    * 链表类。单向链表。
    */
    template<typename Type>
    class CachedList
    {
    public:
        typedef typename ListNode<Type> ListNode;///链表结点
        typedef ListNode *LPListNode;///链表结点指针
    public:

        ///构造函数
        CachedList()
        {
            m_nPoolSize = 0;
        }

        ///析构函数
        ~CachedList() { destroy(); }

        ///链表是否为空
        bool empty(void) { return (NULL == m_head.next); }

        ///统计链表元素个数
        int size( void )
        {
            int count = 0;
            ListNode *p = m_head.next;
            while (p != NULL)
            {
                ++count;
                p = p->next;
            }
            return count;
        }

        ///向链头添加元素
        void push_front(const Type & t)
        {
            ListNode *p = newNode(t);
            push_front(p);
        }

        ///向链头添加元素
        void push_front(ListNode *p)
        {
            if(NULL != p)
            {
                p->next = m_head.next;
                m_head.next = p;
            }
        }

        ////查找元素
        ListNode* find(Type t, ListCompare<Type> cmp = ListCompare<Type>())
        {
            ListNode *p = m_head.next;
            while (p != NULL)
            {
                if ( cmp(p->data, t))
                {
                    return p;
                }
                p = p->next;
            }
            return NULL;
        }

        ///清空链表。仅将结点移到内存池。
        void clear()
        {
            ListNode *p = m_head.next;
            while (p != NULL)
            {
                p = deleteNode(p);
            }
            m_head.next = NULL;
        }

        ///销毁所有资源，包括内存池
        void destroy()
        {
            clear();
            ListNode *p = m_memoryPool.next;
            ListNode *pTemp = NULL;
            while (p != NULL)
            {
                pTemp = p;
                p = p->next;
                delete pTemp;
            }
        }

        ///分配新节点
        ListNode *newNode(void)
        {
            if(NULL == m_memoryPool.next)
            {
                return new ListNode();
            }
            else
            {
                ListNode *p = m_memoryPool.next;
                m_memoryPool.next = p->next;
                p->next = NULL;
                --m_nPoolSize;
                return p;
            }
        }

        ///分配新节点
        ListNode *newNode(const Type & t)
        {
            ListNode *p = newNode();
            p->data = t;
            return p;
        }

        ///删除结点。返回pNode的后继结点地址。
        ListNode *deleteNode(ListNode *pNode)
        {
            if (pNode != NULL)
            {
                ListNode *p = pNode->next;
                pNode->next = m_memoryPool.next;
                m_memoryPool.next = pNode;
                ++m_nPoolSize;
                return p;
            }
            else
            {
                return NULL;
            }
        }

        ///预留空间。本质上是增加内存池容量。
        void reseve(int size)
        {
            for(int i = m_nPoolSize; i < size; ++i)
            {
                deleteNode(new ListNode());
            }
        }

        ///内存池元素个数
        int sizePool( void ) { return m_nPoolSize; }

        ///返回链表头结点
        ListNode * head(void) { return &m_head; }

        ///链表第一个元素
        ListNode * begin(void) { return m_head.next; }

        ///链表结束标识
        ListNode * end(void) { return NULL; }

    private:
        ListNode	m_head;		    //头结点
        ListNode	m_memoryPool;   //内存池
        int         m_nPoolSize;    //内存池元素个数
    };


} // end namespace Lazy

#endif
