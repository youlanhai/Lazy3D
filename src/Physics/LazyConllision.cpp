
#include "stdafx.h"
#include "LazyConllision.h"
#include "PhysicsDebug.h"

/*

collision info : start->end, externd.

1.构造碰撞体的包围盒AABB。
2.将碰撞体AABB与OCTree的AABB进行碰撞检测，如果不相交，则碰撞体与OCTree不相交，碰撞检测结束。
3.如果OCTree结点不可再分，此时，该OCTree结点就是最近结点之一（是最优解的可能性最大），停止分割，并向上层返回结果。
4.判断AABB在分割面的那一侧，如果分别在两侧，则将碰撞体分割成两部分，在OCTree的两个子树中分别进行碰撞检测，即转到2.
5.判断直线（start, end）的两端点分别在平面的那一侧，如果分别位于两端，则沿着分割点，将碰撞体分割为
两个新的子碰撞体，重新计算子分割体的AABB，然后优先检测正向的碰撞体，即转到2.
6.如果，AABB与平面近似平行，则直接分割此AABB，包含直线的一边，求出最小AABB。然后分别检测两子碰撞体
与OCTree的两个结点，求出最近的碰撞距离。
7.此时，AABB与平面相交为一个角，从碰撞体末端分割成两个AABB，转到2。


*/

namespace Physics
{
    namespace DebugFlag
    {
        bool renderNode = true;
        bool renderLeafBox = true;
        bool renderLeafTri = false;
    }

    namespace OCTreeConfig
    {
        //是否在每次分割时，重新计算包围盒，保证每次分割时的aabb是最小的。
        const bool  bestAABB = false;

        //树的最大深度，超过此深度后，分割停止。
        const int   maxBuildDepth = 10;

        //叶结点最大三角形个数。当三角形数小于此值时，分割停止。
        const int   maxLeafSize = 2;

        //各轴上的最小粒度，当aabb直径最大长度小于此值时，分割停止。
        const float minExtend = 20.0f;

        //每个结点包含多个三角形，拾取最近的三角形。
        const bool  pickBestLine = true;
    }


    //////////////////////////////////////////////////////////////////////////

    OCTreeBase::OCTreeBase()
    {
    }

    OCTreeBase::~OCTreeBase()
    {
    }

    //////////////////////////////////////////////////////////////////////////

    OCTreeLeaf::OCTreeLeaf()
        : m_pTriangles(NULL)
    {}

    bool OCTreeLeaf::build(const AABB & aabb, 
        const TriangleIndices & triangleIndices, 
        const TriangleSet & triangles)
    {
        m_aabb = aabb;
        m_indices = triangleIndices;
        m_pTriangles = &triangles;

        return true;
    }

    void OCTreeLeaf::render(LPDIRECT3DDEVICE9 pDevice) const
    {
        if(DebugFlag::renderLeafTri)
        {
            for (TriangleIndices::const_iterator it = m_indices.begin();
                it != m_indices.end(); ++it)
            {
                drawTriangle(pDevice, m_pTriangles->at(*it), 0x7f7f7f7f);
            }
        }

        if (DebugFlag::renderLeafBox)
        {
            drawAABB(pDevice, m_aabb, 0xffffffff);
        }
    }


    bool OCTreeLeaf::pick(RayInfo & info) const
    {
        bool collid = false;
        for (TriangleIndices::const_iterator it = m_indices.begin(); it != m_indices.end(); ++it)
        {
            const Triangle & srcTri = (*m_pTriangles)[*it];
            if (srcTri.intersectRay(info.start, info.dir, &info.hitDistance))
            {
                collid = true;
                info.hitTriangle = srcTri;

                if(!OCTreeConfig::pickBestLine)
                    break;
            }
        }
        return collid;
    }

    bool OCTreeLeaf::collied(OCTreeCollider & collider, float & minDistance) const
    {
        m_aabb.clipLine(collider.start, collider.end);
        minDistance = collider.start.distTo(collider.testOld);

        if (collider.pInfo)
        {
            for (TriangleIndices::const_iterator it = m_indices.begin(); it != m_indices.end(); ++it)
            {
                const Triangle & srcTri = (*m_pTriangles)[*it];
                collider.pInfo->hitTriangles.push_back(srcTri);
            }
        }

        return true;
    }

    size_t OCTreeLeaf::getBytes() const
    {
        return sizeof(*this) + m_indices.capacity()*sizeof(int);
    }

    //////////////////////////////////////////////////////////////////////////
    OCTreeNode::OCTreeNode()
    {

    }

    /*static */bool OCTreeNode::build(OCTreePtr & child, 
        const AABB & aabb, 
        const TriangleIndices & indices, 
        const TriangleSet & triangles,
        int depth)
    {
        AABB newAabb = aabb;

        bool stopDivide = (indices.size() <= OCTreeConfig::maxLeafSize || depth>=OCTreeConfig::maxBuildDepth);
        if (depth > 1 && (OCTreeConfig::bestAABB || stopDivide))
        {
            AABB ab;
            calTrianglesAABB(ab, indices, triangles);

            //两个aabb求交，获得最小范围。
            newAabb.sub(ab);
        }

        //三角形数到达最小粒度
        if (!stopDivide)
        {
            Plane splitPlane;
            AABB front, back;
            TriangleIndices triFront, triBack;

            while(true)
            {
                triFront.clear();
                triBack.clear();

                Vector3 diameter;//直径
                newAabb.getDiameter(diameter);

                int splitAxsis = diameter.maxAxis();//最大分割轴

                //到达最小分割粒度
                if (fabs(diameter[splitAxsis]) <= OCTreeConfig::minExtend)
                {
                    break;
                }

                //在最大轴上分割
                splitAABB(splitPlane, front, back, newAabb, splitAxsis);

                for (TriangleIndices::const_iterator it = indices.begin();
                    it != indices.end(); ++it)
                {
                    int side = splitPlane.witchSide(triangles[*it]);

                    if (side == SIDE_ON)
                    {
                        triFront.push_back(*it);
                        //triBack.push_back(*it);
                    }
                    else
                    {
                        if (side & SIDE_FRONT)
                        {
                            triFront.push_back(*it);
                        }

                        if (side & SIDE_BACK)
                        {
                            triBack.push_back(*it);
                        }
                    }
                }

                if (triFront.empty())//如果正面没有三角形，则忽略正面的空间。
                {
                    newAabb = back;//缩小包围盒
                }
                else if (triBack.empty())//同理
                {
                    newAabb = front;
                }
                else//都不为空
                {
                    OCTreeNode *pNode = new OCTreeNode();
                    child = pNode;

                    pNode->m_aabb = newAabb;
                    pNode->m_plane = splitPlane;
                    pNode->build(pNode->m_front, front, triFront, triangles, depth+1);
                    pNode->build(pNode->m_back, back, triBack, triangles, depth+1);

                    return true;
                }
            }

        }


        OCTreeLeaf* pLeaf = new OCTreeLeaf();
        child = pLeaf;
        return pLeaf->build(newAabb, indices, triangles);
    }

    void OCTreeNode::render(LPDIRECT3DDEVICE9 pDevice) const
    {
        if(m_front) m_front->render(pDevice);
        if(m_back) m_back->render(pDevice);
        if (DebugFlag::renderNode)
        {
            drawAABB(pDevice, m_aabb, 0xffffffff);
        }
    }

    bool OCTreeNode::pick(RayInfo & info) const
    {
        if (!m_aabb.intersectsRay(info.start, info.dir))
        {
            return false;
        }
        if( m_plane.witchSide(info.start) & SIDE_FRONT )
        {
            if (m_front && m_front->pick(info))
            {
                return true;
            }
            if (m_back && m_back->pick(info))
            {
                return true;
            }
        }
        else
        {
            if (m_back && m_back->pick(info))
            {
                return true;
            }
            if (m_front && m_front->pick(info))
            {
                return true;
            }
        }
        return false;
    }

    bool OCTreeNode::collied(OCTreeCollider & collider, float & minDistance) const
    {
        //m_aabb.clipLine(collider.start, collider.end);
        //collider.calAABB();

        if (!m_aabb.intersect(collider.aabb))
        {
            return false;
        }

        float distS = m_plane.distToPoint(collider.start);
        float distE = m_plane.distToPoint(collider.end);

        if (distS >= 0.0f && distE >= 0.0f)
        {
            if (m_front)
                return m_front->collied(collider, minDistance);
        }
        else if (distS < 0.0f && distE < 0.0f)
        {
            if (m_back)
                return m_back->collied(collider, minDistance);
        }
        else
        {
            float pos = fabs(distS)/(fabs(distS) + fabs(distE));
            OCTreeCollider front, back;
            collider.split(pos, front, back);

            float dist1 = MAX_FLOAT;
            float dist2 = MAX_FLOAT;
            bool collid = false;
            
            if (distS > 0)
            {
                collid |= (m_front && m_front->collied(front, dist1));
                collid |= (m_back && m_back->collied(back, dist2));
            }
            else
            {
                collid |= (m_back && m_back->collied(front, dist1));
                collid |= (m_front && m_front->collied(back, dist2));
            }
            if (collid)
            {
                minDistance = min(dist1, dist2);
                return true;
            }
        }

        return false;
    }

    size_t OCTreeNode::getBytes() const
    {
        size_t size = sizeof(*this);
        if (m_front) size += m_front->getBytes();
        if (m_back) size += m_back->getBytes();
        return size;
    }

    //////////////////////////////////////////////////////////////////////////


    bool OCTree::build(LPD3DXMESH pMesh)
    {
        if(!m_meshExtractor.build(pMesh))
            return false;

        const TriangleSet & triangles = m_meshExtractor.triangles();

        TriangleIndices indices(triangles.size());
        for (size_t i=0; i<indices.size(); ++i)
        {
            indices[i] = i;
        }

        AABB aabb;
        calTrianglesAABB(aabb, indices, triangles);
        return OCTreeNode::build(m_root, aabb, indices, m_meshExtractor.triangles(), 1);
    }

    bool OCTree::collision(CollisionInfo & info) const
    {
        return false;
    }


    //拾取射线 将射线截取到包围盒范围。
    bool OCTree::pick(RayInfo & info) const
    {
        return m_root->pick(info);
    }

    bool OCTree::collied(OCTreeCollider & collider, float & minDistance) const
    {
        return m_root->collied(collider, minDistance);
    }

    void OCTree::render(LPDIRECT3DDEVICE9 pDevice) const
    {
        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

        pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

        m_root->render(pDevice);

        pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

    }

    size_t OCTree::getBytes() const
    {
        size_t size = sizeof(*this) + m_meshExtractor.getBytes() - sizeof(MeshExtractor);
        if (m_root)
        {
            size += m_root->getBytes();
        }
        return size;
    }

    //////////////////////////////////////////////////////////////////////////
    OCTreeCollider::OCTreeCollider()
        : pInfo(NULL)
    {

    }

    //根据start，end求出最大的包围盒AABB
    void OCTreeCollider::calAABB()
    {
        aabb.zero();

        Vector3 look = end - start;
        look.normalize();

        //选择一个合适的up，使得right = up x look.
        //right = up x look
        //x = y x z 如果look为z轴，则up选y轴，right就是x轴
        //y = z x x
        //z = x x y
        Vector3 up;
        int axis = look.maxAxis();
        if (axis = AXIS_X) up.set(0, 0, 1);
        else if(axis == AXIS_Y) up.set(1, 0, 0);
        else up.set(0, 1, 0);

        Vector3 right = up.cross(look);
        up = look.cross(right);

        Vector3 halfRight = right * extends.x;
        Vector3 halfUp = up * extends.y;

        Vector3 rightUp = halfRight + halfUp;
        Vector3 leftUp = halfUp - halfRight;

        aabb.addPoint(start + rightUp);
        aabb.addPoint(start - rightUp);
        aabb.addPoint(start + leftUp);
        aabb.addPoint(start - leftUp);
        aabb.addPoint(end + rightUp);
        aabb.addPoint(end - rightUp);
        aabb.addPoint(end + leftUp);
        aabb.addPoint(end - leftUp);
    }

    void OCTreeCollider::render(LPDIRECT3DDEVICE9 pDevice, DWORD color/*=0xff00ff00*/)
    {
        Vector3 look = end - start;
        look.normalize();
        Vector3 up(0, 1, 0);
        Vector3 right = up.cross(look);
        up = look.cross(right);

        Vector3 halfRight = right * extends.x;
        Vector3 halfUp = up * extends.y;

        Vector3 rightUp = halfRight + halfUp;
        Vector3 leftUp = halfUp - halfRight;

        Vector3 pt[8];
        pt[0] = start + leftUp;
        pt[1] = start + rightUp;
        pt[2] = start - leftUp;
        pt[3] = start - rightUp;

        pt[4] = end + leftUp;
        pt[5] = end + rightUp;
        pt[6] = end - leftUp;
        pt[7] = end - rightUp;

        for (int i=0; i<4; ++i)
        {
            drawLine(pDevice, pt[i], pt[(i+1) % 4], color);
        }

        for (int i=0; i<4; ++i)
        {
            drawLine(pDevice, pt[i+4], pt[(i+1) % 4 + 4], color);
        }

        for (int i=0; i<4; ++i)
        {
            drawLine(pDevice, pt[i], pt[i + 4], color);
        }
    }

    //根据现有AABB的范围，求出一个最小的AABB。
    void OCTreeCollider::calSmallAABB()
    {

    }


    /*  从pos（分割比例）位置，将start，end分割成两段，并分别求出最小AABB。
        pos取值范围（0,1）。
        */
    void OCTreeCollider::split(float pos, OCTreeCollider & front, OCTreeCollider & back) const
    {
        if (pos < 0.0f)
        {
            pos = 0.0f;
        }
        else if (pos > 1.0f)
        {
            pos = 1.0f;
        }

        front = *this;
        front.end = start + (end - start) * pos;
        front.calAABB();

        back = *this;
        back.start = back.end;
        back.calAABB();
        
    }

    //////////////////////////////////////////////////////////////////////////
    struct OCTreeCacheNode
    {
        DWORD id;
        OCTreePtr tree;

        size_t getBytes() const
        {
            size_t size = sizeof(*this);
            if (tree)
            {
                size += tree->getBytes();
            }
            return size;
        }
    };
    typedef std::list<OCTreeCacheNode> OCTreeCache;

    OCTreeCache g_cache;
    const int MaxCacheNum = 100;

#if 0
    struct _Finder
    {
        DWORD m_id;
        _Finder(DWORD id) : m_id(id) {}

        bool operator()(const OCTreeCacheNode & node) const
        {
            return node.id == m_id;
        }

    };
#endif

    OCTreePtr findOCTree(LPD3DXMESH pMesh)
    {
        DWORD id = (DWORD)pMesh;
        auto pred = [id](const OCTreeCacheNode & node){ return node.id == id; } ;
        auto it = std::find_if(g_cache.begin(), g_cache.end(), pred); //_Finder(id)
        if (it != g_cache.end()) return it->tree;
        
        return NULL;
    }

    
    OCTreePtr getOCTree(LPD3DXMESH pMesh)
    {
        DWORD id = (DWORD)pMesh;
        auto pred = [id](const OCTreeCacheNode & node){ return node.id == id; };
        OCTreeCache::iterator it = std::find_if(g_cache.begin(), g_cache.end(),  pred);

        OCTreeCacheNode node;
        if (it != g_cache.end())
        {
            node = *it;
            g_cache.erase(it);
        }
        else
        {
            if (g_cache.size() >= MaxCacheNum)
            {
                g_cache.pop_front();//删除头部结点

                debugMessage(_T("INFO: Cache is full, remove octree 0x%8.8x"), g_cache.front().id);
            }

            debugMessage(_T("INFO: build octree for mesh 0x%8.8x"), id);
            OCTree * pTree = new OCTree();
            if (!pTree->build(pMesh))
            {
                delete pTree;
                pTree = NULL;
            }
            node.id = id;
            node.tree = pTree;
        }

        g_cache.push_back(node);//最近访问的结点会放在链表尾部
        return node.tree;
    }

    size_t getOCTreeCacheBytes()
    {
        size_t size = 0;
        for (OCTreeCache::iterator it=g_cache.begin(); it!=g_cache.end(); ++it)
        {
            size += it->getBytes();
        }
        return size;
    }

    void clearOCTreeCache()
    {
        g_cache.clear();
    }

    //////////////////////////////////////////////////////////////////////////
   
    LazyCollision::LazyCollision(void)
    {

    }
    LazyCollision::~LazyCollision(void)
    {

    }

    bool LazyCollision::preventCollision(CollisionInfo & info)
    {
        OCTreePtr tree = getOCTree(info.pMesh);
        if (!tree)
        {
            return false;
        }

        OCTreeCollider collider;
        collider.start = info.oldPos;
        collider.end = info.newPos;
        collider.extends = info.extends;
        collider.pInfo = &info;
        collider.testOld = collider.start;
        collider.calAABB();

        float collidDist;
        return tree->collied(collider, collidDist);
    }

    bool LazyCollision::pickRay(RayInfo & info)
    {
        OCTreePtr tree = getOCTree(info.pMesh);
        if (!tree)
        {
            return false;
        }

        return tree->pick(info);
    }

    
}