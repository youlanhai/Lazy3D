#pragma once

#include "Collision.h"
#include "PhysicsUtil.h"

namespace Physics
{
    class OCTreeCollider
    {
    public:

        OCTreeCollider();

        //根据start，end求出最大的包围盒AABB
        void calAABB();

        //根据现有AABB的范围，求出一个最小的AABB。
        void calSmallAABB();

        /*  从pos（分割比例）位置，将start，end分割成两段，并分别求出最小AABB。
         *  pos取值范围（0,1）。
         */
        void split(float pos, OCTreeCollider & font, OCTreeCollider & back) const;

        void render(LPDIRECT3DDEVICE9 pDevice, DWORD color=0xff00ff00);

    public:
        CollisionInfo *pInfo;
        Vector3 start;
        Vector3 end;
        Vector3 extends;
        Vector3 testOld;
        AABB    aabb;
    };

    //////////////////////////////////////////////////////////////////////////

    //八叉树基类
    class OCTreeBase : public IBase
    {
    public:
        OCTreeBase();
        virtual ~OCTreeBase();

        //拾取射线
        virtual bool pick(RayInfo & info) const = 0;

        //碰撞检测
        virtual bool collied(OCTreeCollider & collider, float & minDistance) const = 0;

        //debug渲染
        virtual void render(LPDIRECT3DDEVICE9 pDevice) const = 0;

        //占用空间
        virtual size_t getBytes() const = 0;

        //是否叶结点
        virtual bool isLeaf() const { return false; }
    };
    typedef RefPtr<OCTreeBase> OCTreePtr;

    //////////////////////////////////////////////////////////////////////////

    //八叉树叶结点
    class OCTreeLeaf : public OCTreeBase
    {
    public:

        OCTreeLeaf();

        bool build(const AABB & aabb, 
            const TriangleIndices & triangleIndices, 
            const TriangleSet & triangles);

        virtual void render(LPDIRECT3DDEVICE9 pDevice) const;

        virtual bool pick(RayInfo & info) const;

        virtual bool collied(OCTreeCollider & collider, float & minDistance) const;

        virtual size_t getBytes() const;

        virtual bool isLeaf() const { return true; }
    private:
        AABB                m_aabb;
        TriangleIndices     m_indices;
        const TriangleSet * m_pTriangles;
    };

    //////////////////////////////////////////////////////////////////////////

    //八叉树普通结点
    class OCTreeNode : public OCTreeBase
    {
    public:
        OCTreeNode();

        virtual void render(LPDIRECT3DDEVICE9 pDevice) const;

        virtual bool pick(RayInfo & info) const;

        virtual bool collied(OCTreeCollider & collider, float & minDistance) const;

        virtual size_t getBytes() const;

        static bool build(OCTreePtr & child,
            const AABB & aabb, 
            const TriangleIndices & indices, 
            const TriangleSet & triangles,
            int depth);

    protected:
        Plane           m_plane;//子树分割平面。
        AABB            m_aabb;
        OCTreePtr       m_front;
        OCTreePtr       m_back;
    };

    //////////////////////////////////////////////////////////////////////////

    //八叉树
    class OCTree : public OCTreeBase
    {
    public:
        bool build(LPD3DXMESH pMesh);

        bool collision(CollisionInfo & info) const;

        bool valid() const{ return m_root && m_meshExtractor.valid(); }

        virtual bool pick(RayInfo & info) const;

        virtual bool collied(OCTreeCollider & collider, float & minDistance) const;

        virtual void render(LPDIRECT3DDEVICE9 pDevice) const;

        virtual size_t getBytes() const;

    private:
        MeshExtractor   m_meshExtractor;
        OCTreePtr       m_root;
    };

    //////////////////////////////////////////////////////////////////////////
    class LazyCollision : public ICollision
    {
    public:
        LazyCollision(void);
        virtual ~LazyCollision(void);

        virtual bool preventCollision(CollisionInfo & info) ;

        virtual bool pickRay(RayInfo & info);
    };

    OCTreePtr findOCTree(LPD3DXMESH pMesh);

    //获得OCTree缓存，占用的空间大小
    size_t getOCTreeCacheBytes();
    void clearOCTreeCache();

    namespace DebugFlag
    {
        extern bool renderNode;
        extern bool renderLeafBox;
        extern bool renderLeafTri;
    }
}
