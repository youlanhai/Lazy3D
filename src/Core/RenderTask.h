#pragma once

#include "Base.h"
#include "RenderObj.h"
#include <vector>

///访问状态
enum VisitState
{
    RS_ACTIVE = 0,      //< 活动状态。
    RS_DEAD = 1,        //< 死亡状态。
    RS_SUSPEND = 2,     //< 延迟状态。
};


///安全访问数据。注意：该类不支持多线程。
template<typename VisitType>
struct VisitPool : IRenderObj, IBase
{
    typedef VisitType*   PVisitType;

    //链表结点
    struct VisitNode
    {
        VisitNode(PVisitType pObj, VisitState state) 
            : m_state(state)
            , m_pObj(pObj)
        { 
        }

        PVisitType obj(void) const { return m_pObj; }
        void setObj(PVisitType obj){ m_pObj = obj; }

        VisitState state(void) const { return m_state; }
        void setState(VisitState a){ m_state = a; }

        /// 是否可对该指针进行操作。
        bool canOperate(void) {  return m_state==RS_ACTIVE && NULL!=m_pObj; }

    private:
        PVisitType  m_pObj;
        VisitState  m_state;
    };

    //相等判断
    struct PredIfEqual
    {
        PredIfEqual(VisitType* pObj) : m_pObj(pObj) { }

        bool operator()(const VisitNode & node)
        {
            return node.obj() == m_pObj;
        }

    protected:
        VisitType* m_pObj;
    };

    //死亡判断
    struct PredIfDead
    {
        PredIfDead() { }
        bool operator()(const VisitNode & node)
        {
            if (node.state() == RS_DEAD)
            {
                return true;
            }
            if (node.state() == RS_SUSPEND)//从延迟状态恢复正常
            {
                const_cast<VisitNode &>(node).setState(RS_ACTIVE);
            }
            return false;
        }
    };

    typedef std::list<VisitNode>                VisitContainer;
    typedef typename VisitContainer::iterator            iterator, VisitIterator;
    typedef typename VisitContainer::reverse_iterator    reverse_iterator,VisitRIterator;

    VisitPool() : m_lockIndex(0)
    {
    }

    ///查找
    VisitIterator find(PVisitType pobj)
    {
        return find_if(PredIfEqual(pobj));
    }

    ///根据条件查找
    template<typename Pred>
    VisitIterator find_if(Pred pred)
    {
        return std::find_if(m_pool.begin(), m_pool.end(), pred);
    }

    ///添加一个元素
    void add(PVisitType obj)
    {
        if (NULL == obj)
        {
            return;
        }
        VisitIterator it = find(obj);
        if (it!=m_pool.end() && it->state()!=RS_DEAD)
        {
            return;
        }

        if (!locking())
        {
            m_pool.push_back(VisitNode(obj, RS_ACTIVE));
        }
        else
        {
            m_pool.push_back(VisitNode(obj, RS_SUSPEND));
        }
    }

    ///移除一个元素
    void remove(PVisitType obj)
    {
        VisitIterator it = find(obj);
        if (it != m_pool.end())
        {
            if(locking())
            {
                it->setState(RS_DEAD);
            }
            else
            {
                m_pool.erase(it);
            }
        }
    }

    ///安全删除已死亡的控件。并恢复正常状态。
    void removeDead(void)
    {
        if (locking())
        {
            return ;
        }
        m_pool.remove_if(PredIfDead());
    }

    ///进行锁定
    void lock(bool l) 
    { 
        if (l)
        {
            ++m_lockIndex;
        }
        else if(m_lockIndex>0)
        {
            --m_lockIndex;
        }
    }

    ///是否锁定
    bool locking(void){ return m_lockIndex>0; }

    VisitContainer* getPool(void){ return &m_pool; }

    VisitIterator begin(void){ return m_pool.begin(); }

    VisitIterator end(void){ return m_pool.end(); }

    VisitRIterator rbegin(void){ return m_pool.rbegin(); }

    VisitRIterator rend(void){ return m_pool.rend(); }

    void clear(){ m_pool.clear(); }

    bool empty(){ return m_pool.empty(); }

    void update(float fElapse)
    {
        lock(true);
        for (VisitIterator it = begin();  it != end();  ++it )
        {
            if (it->canOperate())
            {
                (it->obj())->update(fElapse);
            }
        }
        lock(false);
        removeDead();
    }

    void render(IDirect3DDevice9 * pDevice)
    {
        lock(true);
        for (VisitIterator it = begin();  it != end();  ++it )
        {
            if (it->canOperate())
            {
                (it->obj())->render(pDevice);
            }
        }
        lock(false);
        removeDead();
    }
private:
    VisitContainer  m_pool;
    int             m_lockIndex;//锁定计数。防止在遍历的时候进行删除和添加操作而造成错误。
};

typedef VisitPool<IRenderObj> CRenderTask;
typedef RefPtr<CRenderTask> RenderTaskPtr;
