#pragma once
/*  基于四叉树分割思想，二叉树数据结构实现的四叉树。
 *  四叉树可用于管理分块地形(chunk)。
 *  1.快速判定鼠标射线与哪个chunk相交。
 *  2.查询某矩形区域覆盖了哪些chunk。可用于判定人物可见区域内的chunk，以及确定物体包围盒所覆盖的chunk。
 */


namespace Physics
{

    typedef std::vector<Math::FRect> RectArray;

    ///构造四叉树配置参数
    struct QTConfig
    {
        size_t maxDepth;///<最大构造深度
        size_t minCount;///<叶结点最新最小区域数量
        float minSize;///<区域最小分隔尺寸
        bool  best;///<生成最优四叉树。最好，意味着消耗更多的时间。
    };

    namespace qtree
    {
        extern QTConfig Config;
    }

    class QuadTreeBase;

    ///射线碰撞检测接口
    class RayCollider
    {
    public:
        Vector2 origin;
        Vector2 dir;

        RayCollider(){}
        ~RayCollider(){}

        ///是否与四叉树叶结点碰撞
        virtual bool doQuery(QuadTreeBase* pTree, const IndicesArray & indices) = 0;
    };

    ///区域相交查询接口
    class RectCollider
    {
    public:
        FRect rect;

        ///是否与四叉树叶结点碰撞
        virtual bool doQuery(QuadTreeBase* pTree, const IndicesArray & indices) = 0;
    };

    ///四叉树基类
    class QuadTreeBase : public IBase
    {
    public:
        QuadTreeBase();
        QuadTreeBase(QuadTreeBase* tree, const Math::FRect & rc);

        ///是否是叶结点
        virtual bool isLeaf() const { return false; }

        ///射线拾取
        virtual bool pick(RayCollider *collider) const = 0;

        ///查询相交的区域
        virtual bool queryRect(RectCollider *collider) const = 0;

        ///根据索引获取区域
        virtual const Math::FRect & getRcById(size_t i) const = 0;

    public://debug 接口

        ///调试渲染
        virtual void render(LPDIRECT3DDEVICE9 pDevice) = 0;

        ///输出到文件
        virtual void output(Lazy::LZDataPtr stream) const = 0;

    protected:
        Math::FRect m_rect;///<此结点所占据的区域
        QuadTreeBase* m_pTree;///<所属的树
    };

    typedef RefPtr<QuadTreeBase> QuadTreePtr;



    ///四叉树叶结点
    class QuadTreeLeaf : public QuadTreeBase
    {
    public:
        QuadTreeLeaf(QuadTreeBase* tree, 
            const Math::FRect & rc,
            const IndicesArray & m_indices);

        virtual bool isLeaf() const override { return true; }

        virtual const Math::FRect & getRcById(size_t i) const override 
        { 
            return m_pTree->getRcById(i); 
        }

        virtual bool pick(RayCollider *collider) const override;

        ///查询相交的区域
        virtual bool queryRect(RectCollider *collider) const override;

    public:
        ///调试渲染
        virtual void render(LPDIRECT3DDEVICE9 pDevice) override;

        ///输出到文件
        virtual void output(Lazy::LZDataPtr stream) const override;

    protected:
        IndicesArray    m_indices;///<数据索引
    };

    ///四叉树结点
    class QuadTreeNode : public QuadTreeBase
    {
    public:
        QuadTreeNode(QuadTreeBase* tree, const Math::FRect & rc, int splitAxis, float splitPos);

        ~QuadTreeNode();

        ///构造四叉树
        static bool build(QuadTreeBase* tree, 
            QuadTreePtr & child,
            const Math::FRect & rect, 
            const IndicesArray & indices,
            size_t depth);

        virtual const Math::FRect & getRcById(size_t i) const override 
        { 
            return m_pTree->getRcById(i); 
        }

        virtual bool pick(RayCollider *collider) const override;

        ///查询相交的区域
        virtual bool queryRect(RectCollider *collider) const override;

    public:

        ///调试渲染
        virtual void render(LPDIRECT3DDEVICE9 pDevice) override;

        ///输出到文件
        virtual void output(Lazy::LZDataPtr stream)  const override;

    private:
        int     m_splitAxis;///<分隔轴
        float   m_splitPos;///<分隔点

        QuadTreePtr m_front;///<子结点
        QuadTreePtr m_back;///<子结点
    };

    /////////////////////////////////////////////////////////////////
    //
    /////////////////////////////////////////////////////////////////

    ///通用四叉树。可以继承此类以及RayCollider类，实现自定义的碰撞检测。
    class QuadTree : QuadTreeBase
    {
    public:
        QuadTree(void);

        ~QuadTree(void);

        ///构造四叉树
        bool build(const RectArray &  rects);

        virtual const Math::FRect & getRcById(size_t i) const override
        {
            return m_rects[i];
        }

        virtual bool pick(RayCollider *collider) const override
        {
            assert(collider);
            return m_root && m_root->pick(collider);
        }

        ///查询相交的区域
        virtual bool queryRect(RectCollider *collider) const override
        {
            assert(collider);
            return m_root && m_root->queryRect(collider);
        }

    public:

        ///调试渲染
        virtual void render(LPDIRECT3DDEVICE9 pDevice) override
        {
            if (m_root) m_root->render(pDevice);
        }

        ///输出到文件
        virtual void output(Lazy::LZDataPtr stream) const override
        {
            if (m_root) m_root->output(stream);
        }

        ///保存到文件中
        bool save(const Lazy::tstring & fname, Lazy::DataType type) const;

    private:
        RectArray  m_rects;///<区域
        QuadTreePtr m_root;///<根结点
    };

}//end namespace Physics
