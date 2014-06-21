#pragma once
/*  八叉树。基于八分空间的思想，二叉树数据结构实现的八叉树。
 *  用于场景物体碰撞检测。
 *  这里的八叉树也可理解为AABB树。
 */


namespace Lazy
{
    typedef std::vector<AABB> AABBArray;

    class OctreeBase;

    ///八叉树的一些配置参数
    namespace oc
    {
        ///构造八叉树配置参数
        struct Config
        {
            size_t maxDepth;///<最大构造深度
            size_t minCount;///<叶结点最新最小区域数量
            float minSize;///<区域最小分隔尺寸
            bool  better;///<生成较好的四叉树。速度稍慢。
            bool  best;///<生成最优四叉树。最好，意味着消耗更多的时间。
        };

        ///射线碰撞检测接口
        class RayCollider
        {
        public:
            Vector3 origin;
            Vector3 dir;

            ///是否与八叉树叶结点碰撞。
            virtual bool query(OctreeBase* pTree, const IndicesArray & indices) = 0;
        };

        ///立方体相交查询接口
        class CubeCollider
        {
        public:
            AABB aabb;

            ///是否与八叉树叶结点碰撞
            virtual bool query(OctreeBase* pTree, const IndicesArray & indices) = 0;
        };

        ///调试参数
        namespace debug
        {
            ///显示原始的包围盒
            extern bool showOriginBox;

            ///显示八叉树的包围盒。如果此值为false，则忽略showNodeBox，showLeafBox
            extern bool showOcBox;

            ///显示结点的包围盒
            extern bool showNodeBox;

            ///显示叶结点包围盒
            extern bool showLeafBox;
        }
    }

    ///八叉树基类
    class OctreeBase : public IBase
    {
    public:
        OctreeBase();
        OctreeBase(const AABB & aabb);
        ~OctreeBase();

        ///是否是叶结点
        virtual bool isLeaf() const { return false; }

        ///射线拾取
        virtual bool pick(oc::RayCollider* pCollider) const = 0;

        ///碰撞查询
        virtual bool collidQuery(oc::CubeCollider *pCollider) const = 0;

        ///根据索引获取AABB
        virtual const AABB & getAABBById(size_t id) const;

        ///获取配置
        virtual const oc::Config & config() const;

    public://调试

        ///渲染AABB树
        virtual void render(LPDIRECT3DDEVICE9 pDevice) = 0;

        ///最大深度
        virtual size_t numMaxDepth() const { return 1; }

        ///结点个数
        virtual size_t numNode() const { return 0; }

        ///叶结点个数
        virtual size_t numLeaf() const { return 1; }

        ///占用的空间
        virtual size_t numBytes() const = 0;

    protected:
        AABB        m_aabb; ///<结点的包围盒
    };
    typedef RefPtr <OctreeBase> OctreePtr;

    ///八叉树叶子
    class OctreeLeaf : public OctreeBase
    {
    public:
        OctreeLeaf(OctreeBase* pTree, const AABB & aabb, const IndicesArray & indices);
        ~OctreeLeaf();

        ///是否是叶结点
        virtual bool isLeaf() const override { return true; }

        ///射线拾取
        virtual bool pick(oc::RayCollider* pCollider) const override;

        ///碰撞查询
        virtual bool collidQuery(oc::CubeCollider *pCollider) const override;

    public://调试

        ///渲染AABB树
        virtual void render(LPDIRECT3DDEVICE9 pDevice) override;

        ///占用的空间
        virtual size_t numBytes() const override;

    private:
        OctreeBase*     m_pTree;///<所属的八叉树
        IndicesArray    m_indices;
    };


    ///八叉树结点
    class OctreeNode : public OctreeBase
    {
    public:
        OctreeNode(const AABB & aabb, int splitAxis, float splitPos);
        ~OctreeNode();

        ///射线拾取
        virtual bool pick(oc::RayCollider* pCollider) const override;

        ///碰撞查询
        virtual bool collidQuery(oc::CubeCollider *pCollider) const override;

        ///构造八叉树
        static bool build(OctreePtr & child, const AABB & aabb,
                          OctreeBase *pTree, const IndicesArray & indices, size_t depth);

    public://调试

        ///渲染AABB树
        virtual void render(LPDIRECT3DDEVICE9 pDevice) override;

        ///最大深度
        virtual size_t numMaxDepth() const override;

        ///结点个数
        virtual size_t numNode() const override;

        ///叶结点个数
        virtual size_t numLeaf() const override;

        ///占用的空间
        virtual size_t numBytes() const override;

    private:
        OctreePtr       m_front;
        OctreePtr       m_back;
        int             m_splitAxis;///<分割轴
        float           m_splitPos;///<分隔点
    };


    ///八叉树
    class Octree : public OctreeBase
    {
    public:
        Octree(const oc::Config & config);
        ~Octree();

        bool build(const AABBArray & aabbs);

        ///射线拾取
        virtual bool pick(oc::RayCollider* pCollider) const override
        {
            return m_root && m_root->pick(pCollider);
        }

        ///碰撞查询
        virtual bool collidQuery(oc::CubeCollider *pCollider) const override
        {
            return m_root && m_root->collidQuery(pCollider);
        }

        ///根据索引获取AABB
        virtual const AABB & getAABBById(size_t id) const override
        {
            return m_aabbs[id];
        }

        virtual const oc::Config & config() const override
        {
            return m_config;
        }

    public://调试

        ///渲染AABB树
        virtual void render(LPDIRECT3DDEVICE9 pDevice) override;

        ///最大深度
        virtual size_t numMaxDepth() const override;

        ///结点个数
        virtual size_t numNode() const override;

        ///叶结点个数
        virtual size_t numLeaf() const override;

        ///占用的空间
        virtual size_t numBytes() const override;

    protected:

        bool doBuild();

    protected:
        oc::Config m_config; ///<配置
        AABBArray m_aabbs;///<aabb数组
        OctreePtr m_root;///<八叉树根节点
    };

}//end namespace Lazy