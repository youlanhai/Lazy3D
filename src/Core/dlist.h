//dlist.h
#pragma once

#ifndef LISTCOMPARE
#define LISTCOMPARE
//元素比较
template<typename Type>
struct ListCompare
{
    bool operator ()(const Type & t1, const Type & t2)
    {
        return t1 == t2;
    }
};
#endif

//链表结点基类
struct List_node_base 
{
    List_node_base* next;
    List_node_base* prev;

    List_node_base():next(0),prev(0){}
};
//链表结点
template <class Type>
struct List_node : public List_node_base
{
    Type data;
};
//链表迭代器基类
struct List_iterator_base 
{
    List_node_base* pNode;

    List_iterator_base() : pNode(0) {}
    List_iterator_base(List_node_base* p) : pNode(p) {}

    void incr() { pNode = pNode->next; }
    void decr() { pNode = pNode->prev; }

    bool operator==(const List_iterator_base& n) const 
    {
        return pNode == n.pNode;
    }
    bool operator!=(const List_iterator_base& n) const 
    {
        return pNode != n.pNode;
    }
}; 
//链表迭代器
template<class Type, class _Ref, class _Ptr>
struct List_iterator : 
    public List_iterator_base 
{
    typedef List_iterator<Type,Type&,Type*>             iterator;
    typedef List_iterator<Type,const Type&,const Type*> const_iterator;
    typedef List_iterator<Type,_Ref,_Ptr>               _Self;

    typedef Type value_type;
    typedef _Ptr pointer;
    typedef _Ref reference;

    typedef List_node<Type> ListNode;

    List_iterator() {}
    List_iterator(ListNode* p) : List_iterator_base(p) {}
    List_iterator(const iterator& n) : List_iterator_base(n.pNode) {}

    reference operator*() const { return ((ListNode*) pNode)->data; }

    pointer operator->() const { return &(operator*()); }

    _Self& operator++()
    { 
        this->incr();
        return *this;
    }
    _Self operator++(int) 
    { 
        _Self __tmp = *this;
        this->incr();
        return __tmp;
    }
    _Self& operator--() 
    { 
        this->decr();
        return *this;
    }
    _Self operator--(int) 
    { 
        _Self __tmp = *this;
        this->decr();
        return __tmp;
    }
};

//链表类。双向链表。
template<typename Type>
class CDList
{
public:
    //typedef typename ListNode<Type> ListNode;

    typedef Type&                               reference;
    typedef const Type&                         const_reference;
    typedef List_node<Type>                     ListNode;
    typedef List_iterator<Type,Type&,Type*>     iterator;
public:
    iterator begin( void ){ return (ListNode*)m_head.next; }
    iterator end( void ){ return (ListNode*)(&m_head);}
    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }
    reference back() { return *(--end()); }
    const_reference back() const { return *(--end()); }

    bool empty( void ){ return m_head.next == &m_head; }

    //在position之前插入
    iterator insert(iterator position, const Type& t)
    {
//         ListNode *tmp = newNode(t);
//         tmp->next = position.pNode;
//         tmp->prev = position.pNode->prev;
//         position.pNode->prev->next = tmp;
//         position.pNode->prev = tmp;
//         return tmp;
        return insert(position,newNode(t));
    }
    //在position之前插入
    iterator insert(iterator position,iterator it)
    {
        it.pNode->next = position.pNode;
        it.pNode->prev = position.pNode->prev;
        position.pNode->prev->next = it.pNode;
        position.pNode->prev = it.pNode;
        return it;
    }
    iterator insert(iterator position) { return insert(position, Type()); }
    void push_front(const Type& __x) { insert(begin(), __x); }
    void push_front() {insert(begin());}
    void push_back(const Type& __x) { insert(end(), __x); }
    void push_back() {insert(end());}

    iterator erase(iterator position)
    {
        List_node_base* next_node = position.pNode->next;
        List_node_base* prev_node = position.pNode->prev;
        ListNode* p = (ListNode*) position.pNode;
        prev_node->next = next_node;
        next_node->prev = prev_node;
        deleteNode(p);
        return iterator((ListNode*) next_node);
    }
public:

	CDList()
    {
        m_nPoolSize = 0;
        m_head.next = &m_head;
        m_head.prev = &m_head;
    }

	~CDList()
    {
        destroy();
    }

    //统计链表元素个数
    int size( void )
    {
        int count=0;
        for (iterator it=begin(); it!=end(); ++it)
        {
            ++count;
        }
        return count;
    }

	iterator find(Type t,ListCompare<Type> cmp=ListCompare<Type>())
    {
        for (iterator it=begin(); it!=end(); ++it)
        {
            if ( cmp(*it, t))
            {
                return it;
            }
        }
        return end();
    }

    //清空链表。仅将结点移到内存池。
    void clear()
    {
        for (iterator it=begin(); it!=end(); )
        {
            iterator temp=it;
            ++it;
            deleteNode(temp);
        }
        m_head.next = &m_head;
        m_head.prev = &m_head;
    }  

    //销毁所有资源，包括内存池
    void destroy()
    {
        clear();
        ListNode *p=(ListNode*)m_memoryPool.next;
        ListNode *pTemp=NULL;
        while (p != NULL)
        {
            pTemp=p;
            p=(ListNode*)p->next;
            delete pTemp;
            --m_nPoolSize;
        }
        m_memoryPool.next=NULL;
    }

    //分配新节点
    ListNode *newNode(void)
    {
        if(NULL == m_memoryPool.next)
        {
            return new ListNode();
        }
        else
        {
            ListNode *p = (ListNode *)m_memoryPool.next;
            m_memoryPool.next = p->next;
            p->next = NULL;
            --m_nPoolSize;
            return p;
        }
    }
    //分配新节点
    ListNode *newNode(const Type &t)
    {
        ListNode *p = newNode();
        p->data = t;
        return p;
    }


    //删除结点。返回pNode的后继结点地址。
    void deleteNode(iterator it)
    {
        it.pNode->next = m_memoryPool.next;
        m_memoryPool.next = it.pNode;
        ++m_nPoolSize;
    }

    //预留空间。本质上是增加内存池容量。
    void reseve(int size)
    {
        for(int i=m_nPoolSize; i < size; ++i)
        {
            deleteNode(new ListNode());
        }
    }

    int sizePool( void ){ return m_nPoolSize; }

private:
	ListNode	m_head;		    //头结点
	ListNode	m_memoryPool;   //内存池
    int         m_nPoolSize;    //内存池元素个数
};
