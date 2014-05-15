#include "stdafx.h"
#include "Octree.h"
#include "PhysicsDebug.h"

namespace Physics
{
    namespace oc
    {
        namespace debug
        {
            bool showOriginBox = false;
            bool showOcBox = true;
            bool showNodeBox = false;
            bool showLeafBox = true;
        }

        ///将一个aabb，按照最长轴的一半，分割成两个
        void splitAABBHalf(AABB & rcFront, AABB & rcBack, float & splitPos, const AABB & rect, int splitAxis)
        {
            rcFront = rcBack = rect;
            splitPos = (rect.min[splitAxis] + rect.max[splitAxis]) * 0.5f;
            rcFront.max[splitAxis] = rcBack.min[splitAxis] = splitPos;
        }

        ///最优分隔法
        bool splitAABBBetter(AABB & rcFront, AABB & rcBack, float & splitPos, const AABB & rect,
            OctreeBase* tree, const IndicesArray & indices, int splitAxis)
        {
            rcFront = rcBack = rect;

            int bestRate = 0x7fffffff;
            for (size_t x : indices)
            {
                float pos = tree->getAABBById(x).min[splitAxis];

                int a = 0;
                int b = 0;

                for (size_t y : indices)
                {
                    if (tree->getAABBById(y).min[splitAxis] < pos) ++a;
                    if (tree->getAABBById(y).max[splitAxis] > pos) ++b;
                }

                assert(a != 0 && b != 0 && "splitAABBBetter");

                int t = abs(a - b);
                if (bestRate > t)
                {
                    bestRate = t;
                    splitPos = pos;
                }
            }

            if (bestRate >= indices.size())//出现了退化，所有aabb此边坐标都相等
            {
                //splitAABBHalf(rcFront, rcBack, splitPos, rect, splitAxis);
                return false;
            }

            rcFront.max[splitAxis] = rcBack.min[splitAxis] = splitPos;
            return true;
        }

        ///最优分隔法
        bool splitAABBBest(AABB & rcFront, AABB & rcBack, float & splitPos, const AABB & rect,
            OctreeBase* tree, const IndicesArray & indices, int & splitAxis)
        {
            rcFront = rcBack = rect;

            int minGap = 0x7fffffff;
            bool ok = false;

            for (int ax = 0; ax < 3; ++ax)
            {
                for (size_t x : indices)
                {
                    float pos = tree->getAABBById(x).min[ax];

                    int a = 0;
                    int b = 0;

                    for (size_t y : indices)
                    {
                        const AABB & aabb = tree->getAABBById(y);
                        if (aabb.min[ax] < pos) ++a;
                        if (aabb.max[ax] > pos) ++b;
                    }

                    assert((a != 0 || b != 0) && "splitAABBBest");

                    int t = abs(a - b);
                    if (minGap > t)
                    {
                        minGap = t;
                        splitPos = pos;
                        splitAxis = ax;

                        if (minGap == indices.size() % 2) //已经不能再小了。
                        {
                            ok = true;
                            break;
                        }
                    }
                }

                if (ok) break;
            }

            if (minGap >= indices.size())//出现了退化，所有aabb此边坐标都相等
            {
                return false;
            }

            rcFront.max[splitAxis] = rcBack.min[splitAxis] = splitPos;
            return true;
        }

        ///判断区域在分隔点的哪侧
        int wichSide(int splitAxis, float splitPos, const AABB & rc)
        {
            int side = 0;

            if (rc.min[splitAxis] < splitPos) side |= SIDE_FRONT;
            if (rc.max[splitAxis] > splitPos) side |= SIDE_BACK;

            return side;
        }

    }

    /////////////////////////////////////////////////////////////////
    OctreeBase::OctreeBase()
    {

    }

    OctreeBase::OctreeBase(const AABB & aabb)
        : m_aabb(aabb)
    {

    }
    
    OctreeBase::~OctreeBase()
    {

    }

    ///根据索引获取AABB
    const AABB & OctreeBase::getAABBById(size_t) const
    {
        throw(std::logic_error("method 'getAABBById' can't be called!"));
    }

    ///获取配置
    const oc::Config & OctreeBase::config() const
    {
        throw(std::logic_error("method 'config' can't be called!"));
    }

    /////////////////////////////////////////////////////////////////

    OctreeLeaf::OctreeLeaf(OctreeBase* pTree, const AABB & aabb, const IndicesArray & indices)
        : OctreeBase(aabb)
        , m_pTree(pTree)
        , m_indices(indices)
    {
        m_aabb.zero();

        for (size_t i : m_indices) m_aabb.add(m_pTree->getAABBById(i));
    }

    OctreeLeaf::~OctreeLeaf()
    {

    }

    ///射线拾取
    bool OctreeLeaf::pick(oc::RayCollider* pCollider) const
    {
        if (!m_aabb.intersectsRay(pCollider->origin, pCollider->dir)) return false;

        return pCollider->query(m_pTree, m_indices);
    }

    ///碰撞查询
    bool OctreeLeaf::collidQuery(oc::CubeCollider *pCollider) const
    {
        if (!m_aabb.intersect(pCollider->aabb)) return false;

        return pCollider->query(m_pTree, m_indices);
    }

    ///渲染AABB树
    void OctreeLeaf::render(LPDIRECT3DDEVICE9 pDevice)
    {
        if(oc::debug::showLeafBox) drawAABB(pDevice, m_aabb, 0xffff0000);
    }

    ///占用的空间
    size_t OctreeLeaf::numBytes() const
    {
        return sizeof(*this) + m_indices.capacity() * sizeof(size_t);
    }

    /////////////////////////////////////////////////////////////////

    OctreeNode::OctreeNode(const AABB & aabb, int splitAxis, float splitPos)
        : OctreeBase(aabb)
        , m_splitAxis(splitAxis)
        , m_splitPos(splitPos)
    {

    }

    OctreeNode::~OctreeNode()
    {

    }

    ///射线拾取
    bool OctreeNode::pick(oc::RayCollider* pCollider) const
    {
        if (!m_aabb.intersectsRay(pCollider->origin, pCollider->dir)) return false;

        if (pCollider->origin[m_splitAxis] < m_splitPos)
        {
            if (m_front && m_front->pick(pCollider)) return true;
            if (m_back && m_back->pick(pCollider)) return true;
        }
        else
        {
            if (m_back && m_back->pick(pCollider)) return true;
            if (m_front && m_front->pick(pCollider)) return true;
        }

        return false;
    }

    ///碰撞查询
    bool OctreeNode::collidQuery(oc::CubeCollider *pCollider) const
    {
        if (!m_aabb.intersect(pCollider->aabb)) return false;

        if (pCollider->aabb.min[m_splitAxis] < m_splitPos)
        {
            if (m_front && m_front->collidQuery(pCollider)) return true;
            if (m_back && m_back->collidQuery(pCollider)) return true;
        }
        else
        {
            if (m_back && m_back->collidQuery(pCollider)) return true;
            if (m_front && m_front->collidQuery(pCollider)) return true;
        }

        return false;
    }

    
    ///渲染AABB树
    void OctreeNode::render(LPDIRECT3DDEVICE9 pDevice)
    {
        if (m_front) m_front->render(pDevice);
        if (m_back) m_back->render(pDevice);

        if(oc::debug::showNodeBox) drawAABB(pDevice, m_aabb, 0xff00ff00);
    }

    ///构造八叉树
    /*static*/bool OctreeNode::build(OctreePtr & child, const AABB &, 
        OctreeBase *pTree, const IndicesArray & indices, size_t depth)
    {
        assert(pTree && "OctreeNode::build");

        AABB newAABB;
        newAABB.zero();
        for (size_t i : indices)
        {
            newAABB.add(pTree->getAABBById(i));
        }

        if (indices.size() <= pTree->config().minCount || depth >= pTree->config().maxDepth)
        {
            //do something
        }
        else
        {
            int splitAxis;
            float splitPos;
            AABB rcFront, rcBack;
            IndicesArray front, back;

            while (true)
            {
                Vector3 diameter;//直径
                newAABB.getDiameter(diameter);

                splitAxis = diameter.maxAxis();//最大分割轴

                //到达最小分割粒度
                if (fabs(diameter[splitAxis]) <= pTree->config().minSize)
                {
                    break;
                }

                if (pTree->config().best)//最优分隔
                {
                    if (!oc::splitAABBBest(rcFront, rcBack, splitPos,
                        newAABB, pTree, indices, splitAxis)) break;
                }
                else if (pTree->config().better)//最优分隔
                {
                    if (!oc::splitAABBBetter(rcFront, rcBack, splitPos,
                        newAABB, pTree, indices, splitAxis)) break;
                }
                else//快速分隔
                {
                    oc::splitAABBHalf(rcFront, rcBack, splitPos, newAABB, splitAxis);
                }

                front.clear();
                back.clear();

                for (size_t x : indices)
                {
                    int side = oc::wichSide(splitAxis, splitPos, pTree->getAABBById(x));

                    if (side & SIDE_FRONT) front.push_back(x);
                    if (side & SIDE_BACK) back.push_back(x);
                }

                if (front.empty())
                {
                    newAABB = rcBack;
                }
                else if (back.empty())
                {
                    newAABB = rcFront;
                }
                else
                {
                    OctreeNode *pNode = new OctreeNode(newAABB, splitAxis, splitPos);
                    child = pNode;
                    build(pNode->m_front, rcFront, pTree, front, depth + 1);
                    build(pNode->m_back,  rcBack, pTree,back, depth + 1);

                    return true;
                }
            }
        }

        child = new OctreeLeaf(pTree, newAABB, indices);
        return true;
    }

    ///最大深度
    size_t OctreeNode::numMaxDepth() const
    {
        size_t d1 = 0, d2 = 0;
        if (m_front) d1 = m_front->numMaxDepth();
        if (m_back) d2 = m_back->numMaxDepth();

        return 1 + max(d1, d2);
    }

    ///结点个数
    size_t OctreeNode::numNode() const
    {
        size_t n = 1;
        if (m_front) n += m_front->numNode();
        if (m_back) n += m_back->numNode();
        return n;
    }

    ///叶结点个数
    size_t OctreeNode::numLeaf() const
    {
        size_t n = 0;
        if (m_front) n += m_front->numLeaf();
        if (m_back) n += m_back->numLeaf();
        return n;
    }

    ///占用的空间
    size_t OctreeNode::numBytes() const
    {
        size_t size = sizeof(*this);
        if (m_front) size += m_front->numBytes();
        if (m_back) size += m_back->numBytes();
        return size;
    }

    /////////////////////////////////////////////////////////////////
    Octree::Octree(const oc::Config & config)
        : m_config(config)
    {
    }


    Octree::~Octree()
    {
    }

    bool Octree::build(const AABBArray & aabbs)
    {
        m_aabbs = aabbs;

        return doBuild();
    }

    bool Octree::doBuild()
    {
        m_aabb.zero();

        for (AABB & rc : m_aabbs) m_aabb.add(rc);

        size_t n = m_aabbs.size();
        IndicesArray indices(n);
        for (size_t i = 0; i < n; ++i) indices[i] = i;

        return OctreeNode::build(m_root, m_aabb, this, indices, 1);
    }

    ///渲染AABB树
    void Octree::render(LPDIRECT3DDEVICE9 pDevice)
    {
        if (oc::debug::showOcBox && m_root) m_root->render(pDevice);

        if (oc::debug::showOriginBox)
        {
            for (AABB & box : m_aabbs)
            {
                drawAABB(pDevice, box, 0xffffff00);
            }
        }
    }

    ///最大深度
    size_t Octree::numMaxDepth() const 
    {
        if (m_root) return m_root->numMaxDepth();
        return 0;
    }

    ///结点个数
    size_t Octree::numNode() const 
    {
        if (m_root) return m_root->numNode();
        return 0;
    }

    ///叶结点个数
    size_t Octree::numLeaf() const 
    {
        if (m_root) return m_root->numLeaf();
        return 0;
    }

    size_t Octree::numBytes() const
    {
        size_t size = sizeof(*this);
        if (m_root) size += m_root->numBytes();
        return size;
    }

}//namespace Physics