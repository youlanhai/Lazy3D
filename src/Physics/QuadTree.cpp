
#include "stdafx.h"
#include "QuadTree.h"

#include "PhysicsDebug.h"

namespace Lazy
{
    namespace qtree
    {
        QTConfig Config = {20, 1, 100.0f, false};
    }

    void rectToString(Lazy::tstring & str, const FRect & rc)
    {
        formatString(str, _T("%f %f %f %f %f %f"), rc.left, rc.top, rc.right, rc.bottom, rc.width(), rc.height());
    }

    ///将一个rect，按照最长轴的一半，分割成两个
    int splitRect(FRect & rcFront, FRect & rcBack, float & splitPos, const FRect & rect)
    {
        rcFront = rcBack = rect;
        if (rect.width() > rect.height())
        {
            splitPos = (rect.left + rect.right) * 0.5f;
            rcFront.right = rcBack.left = splitPos;
            return AXIS_X;
        }
        else
        {
            splitPos = (rect.top + rect.bottom) * 0.5f;
            rcFront.bottom = rcBack.top = splitPos;
            return AXIS_Y;
        }
    }

    ///最优分隔法
    int splitRectBest(FRect & rcFront, FRect & rcBack, float & splitPos, const FRect & rect,
                      QuadTreeBase* tree, const IndicesArray & indices)
    {
        rcFront = rcBack = rect;

        int axis = AXIS_X;
        if (rect.width() < rect.height())
        {
            axis = AXIS_Y;
        }

        float bestRate = 0.0f;
        for (size_t x : indices)
        {
            float pos = tree->getRcById(x)[axis];

            size_t a = 0;
            size_t b = 0;

            for (size_t y : indices)
            {
                if (tree->getRcById(y)[axis] < pos) ++a;
                else ++b;
            }

            if (a > b) std::swap(a, b);
            float t = float(a) / float(b);
            if (bestRate < t)
            {
                bestRate = t;
                splitPos = pos;
            }
        }

        rcFront[axis + 2] = rcBack[axis] = splitPos;
        return axis;
    }

    ///判断区域在分隔点的哪侧
    int wichSide(int splitAxis, float splitPos, const FRect & rc)
    {
        int side = 0;
        if (splitAxis == AXIS_X)
        {
            if (rc.left < splitPos) side |= SIDE_FRONT;
            if (rc.right > splitPos) side |= SIDE_BACK;
        }
        else
        {
            if (rc.top < splitPos) side |= SIDE_FRONT;
            if (rc.bottom > splitPos) side |= SIDE_BACK;
        }
        return side;
    }

    /////////////////////////////////////////////////////////////////
    QuadTreeBase::QuadTreeBase()
    {}

    QuadTreeBase::QuadTreeBase(QuadTreeBase* tree, const FRect & rc)
        : m_rect(rc)
        , m_pTree(tree)
    {
    }
    /////////////////////////////////////////////////////////////////
    QuadTreeLeaf::QuadTreeLeaf(QuadTreeBase* tree, const FRect & rc,
                               const IndicesArray & indices)
        : QuadTreeBase(tree, rc)
        , m_indices(indices)
    {
    }

    bool QuadTreeLeaf::pick(RayCollider *collider) const
    {
        if (!m_rect.isIntersect(collider->origin, collider->dir)) return false;

        return collider->doQuery(m_pTree, m_indices);
    }

    bool QuadTreeLeaf::queryRect(RectCollider *collider) const
    {
        if (!m_rect.isIntersect(collider->rect)) return false;

        return collider->doQuery(m_pTree, m_indices);
    }

    ///调试渲染
    void QuadTreeLeaf::render(LPDIRECT3DDEVICE9 pDevice)
    {

    }

    void QuadTreeLeaf::output(Lazy::LZDataPtr stream) const
    {
        Lazy::tstring temp;
        rectToString(temp, m_rect);
        stream->writeString(_T("rect"), temp);

        for (size_t i : m_indices)
        {
            Lazy::LZDataPtr ptr = stream->newOne(_T("index"), Lazy::EmptyStr);
#if 1
            ptr->setUint(i);
#else
            ptr->writeUint(_T("id"), i);

            rectToString(temp, getRcById(i));
            ptr->writeString(_T("rc"), temp);
#endif
            stream->addChild(ptr);
        }
    }

    //////////////////////////////////////////////////////////////////
    QuadTreeNode::QuadTreeNode(QuadTreeBase* tree, const FRect & rc, int splitAxis, float splitPos)
        : QuadTreeBase(tree, rc)
        , m_splitPos(splitPos)
        , m_splitAxis(splitAxis)
    {}

    QuadTreeNode::~QuadTreeNode()
    {}

    bool QuadTreeNode::pick(RayCollider *collider) const
    {
        if (!m_rect.isIntersect(collider->origin, collider->dir)) return false;

        if (collider->origin[m_splitAxis] < m_splitPos)
        {
            if (m_front && m_front->pick(collider)) return true;
            if (m_back && m_back->pick(collider)) return true;
        }
        else
        {
            if (m_back && m_back->pick(collider)) return true;
            if (m_front && m_front->pick(collider)) return true;
        }

        return false;
    }

    bool QuadTreeNode::queryRect(RectCollider *collider) const
    {
        if (!m_rect.isIntersect(collider->rect)) return false;

        if (collider->rect[m_splitAxis] < m_splitPos)
        {
            if (m_front && m_front->queryRect(collider)) return true;
            if (m_back && m_back->queryRect(collider)) return true;
        }
        else
        {
            if (m_back && m_back->queryRect(collider)) return true;
            if (m_front && m_front->queryRect(collider)) return true;
        }

        return false;
    }

    bool QuadTreeNode::build(
        QuadTreeBase* tree,
        QuadTreePtr & child,
        const FRect & rect,
        const IndicesArray & indices,
        size_t depth)
    {
        assert(tree && "QuadTreeNode::build");

        FRect newRect = rect;
        if (indices.size() <= qtree::Config.minCount || depth > qtree::Config.maxDepth)
        {
            //do something
        }
        else
        {
            int splitAxis;
            float splitPos;
            FRect rcFront, rcBack;
            IndicesArray front, back;

            while (true)
            {
                //最小分隔粒度
                if (newRect.width() <= qtree::Config.minSize && newRect.height() <= qtree::Config.minSize)
                {
                    break;
                }

                if (qtree::Config.best)//最优分隔
                {
                    splitAxis = splitRectBest(rcFront, rcBack, splitPos, newRect, tree, indices);
                }
                else//快速分隔
                {
                    splitAxis = splitRect(rcFront, rcBack, splitPos, newRect);
                }

                front.clear();
                back.clear();

                for (auto & x : indices)
                {
                    int side = wichSide(splitAxis, splitPos, tree->getRcById(x));

                    if (side & SIDE_FRONT) front.push_back(x);
                    if (side & SIDE_BACK) back.push_back(x);
                }

                if (front.empty())
                {
                    newRect = rcBack;
                }
                else if (back.empty())
                {
                    newRect = rcFront;
                }
                else
                {
                    QuadTreeNode *pNode = new QuadTreeNode(tree, newRect, splitAxis, splitPos);
                    child = pNode;
                    build(tree, pNode->m_front, rcFront, front, depth + 1);
                    build(tree, pNode->m_back, rcBack, back, depth + 1);

                    return true;
                }
            }
        }

        child = new QuadTreeLeaf(tree, newRect, indices);
        return true;
    }

    ///调试渲染
    void QuadTreeNode::render(LPDIRECT3DDEVICE9 pDevice)
    {
        FRect rc(m_rect);
        rc.delta(-10.0f, -10.0f);
        //drawFRectSolid(pDevice, rc, 0.0f, 0xff00ffff);
        drawFRect(pDevice, rc, 10.0f, 0xffff0000);

        if (m_front) m_front->render(pDevice);
        if (m_back) m_back->render(pDevice);
    }

    void QuadTreeNode::output(Lazy::LZDataPtr stream) const
    {
        Lazy::tstring temp;
        rectToString(temp, m_rect);
        stream->writeString(_T("rect"), temp);
        stream->writeInt(_T("axis"), m_splitAxis);
        stream->writeFloat(_T("pos"), m_splitPos);

        if (m_front)
        {
            Lazy::LZDataPtr ptr = stream->newOne(_T("front"), Lazy::EmptyStr);
            stream->addChild(ptr);
            m_front->output(ptr);
        }
        if (m_back)
        {
            Lazy::LZDataPtr ptr = stream->newOne(_T("back"), Lazy::EmptyStr);
            stream->addChild(ptr);
            m_back->output(ptr);
        }
    }

    /////////////////////////////////////////////////////////////////

    QuadTree::QuadTree(void)
    {
    }

    QuadTree::~QuadTree(void)
    {
    }

    bool QuadTree::build(const RectArray &  rects)
    {
        m_rects = rects;
        m_rect.zero();

        for (FRect & rc : m_rects) m_rect.expand(rc);

        size_t n = m_rects.size();
        IndicesArray indices(n);
        for (size_t i = 0; i < n; ++i) indices[i] = i;

        return QuadTreeNode::build(this, m_root, m_rect, indices, 1);
    }

    bool QuadTree::save(const Lazy::tstring & fname, Lazy::DataType type) const
    {
        Lazy::LZDataPtr stream = Lazy::openSection(fname, true, type);
        if (!stream) return false;

        output(stream);

        return Lazy::saveSection(stream, fname);
    }


}//end namespace Lazy
