﻿#pragma once

/**
 *  当地图过大时，将地图分割成若干个小的地图，每次渲染时，仅渲染玩家周围的几个子地图，这样可以得到较高的效率。
 *  MapNode就是这样的子地图。
*/

#include "TerrainData.h"
#include "SceneNode.h"

#include "../Physics/Octree.h"

#include "../Render/Effect.h"
#include "../Render/Texture.h"

#include <hash_map>

#define USE_MESH 1

namespace Lazy
{

    namespace MapConfig
    {
        const int NbChunkGridSq = NbChunkGrid * NbChunkGrid;

        const int NbChunkVertexSq = NbChunkVertex * NbChunkVertex;

        const int NbChunkGridFace = NbChunkGridSq * 2;
        const int NbChunkIndices = NbChunkGridFace * 3;

        const int MaxNbChunkMesh = 64;
        
        extern int MaxNbChunkLayer;
        const int MaxNbChunkTexture = 6;
    }

    class TerrainChunk;
    class TerrainMap;

    namespace mapnode
    {
        ///用于射线拾取地形obj
        class ObjCollider : public oc::RayCollider
        {
            SceneNodePtr    m_obj;
            TerrainChunk*   m_pMapNode;

        public:
            ObjCollider(TerrainChunk* pMapNode);

            SceneNodePtr getObj() const { return m_obj; }

            ///是否与八叉树叶结点碰撞。
            virtual bool query(OctreeBase* pTree, const IndicesArray & indices) override;
        };
    }

    //////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////
    class TerrainMesh
    {
    public:

        TerrainMesh();

        bool create(LPDIRECT3DDEVICE9 pDevice);

        void optimize();

        bool valid() const { return m_pMesh != NULL; }

        bool lockVB(void** pp, DWORD flag = 0) const;
        void unlockVB() const;

        bool lockIB(void**pp, DWORD flag = 0) const;
        void unlockIB() const;

        void render(LPDIRECT3DDEVICE9 pDevice);

        LPD3DXMESH getMesh() const { return m_pMesh; }

    private:

        //释放资源。不允许显示释放。
        void release();
        void clear();

        LPD3DXMESH m_pMesh; //将高度图生成mesh

        friend class VIBCache;
    };

    //////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////
    class VIBCache
    {
        VIBCache();

        ~VIBCache();

    public:

        size_t size() const { return m_pool.size(); }

        void preLoadOne();

        TerrainMesh get();

        void release(TerrainMesh & holder);

        void clear();

        static VIBCache* instance();

    private:
        typedef std::list<TerrainMesh> CachePool;

        CachePool   m_pool;
        ZCritical m_locker;
    };

    //////////////////////////////////////////////////////////////////////////
    ///地图子块
    //////////////////////////////////////////////////////////////////////////
    class TerrainChunk : public SceneNode
    {
    public:
        SCENE_NODE_HEADER(TerrainChunk);

        TerrainChunk(void);
        ~TerrainChunk(void);

        void init(TerrainMap *pMap, uint32 id, const FRect & rect);

        void render(IDirect3DDevice9* pDevice);
        void update(float elapse);

        bool load();
        void unload();
        bool save();

        bool ifLoaded() const { return m_isLoaded; }
        bool ifLoading() const { return m_isLoading; }

        uint32 getID() const { return m_id; }
        int getRowID() const { return (m_id >> 16) & 0xffff; }
        int getColID() const { return m_id & 0xffff; }

        const FRect & getRect() const { return m_rect; }
        float getSize() const { return m_rect.width(); }
        LPD3DXMESH getTerrainMesh() const { return m_mesh.getMesh(); }

    public://拾取与碰撞检测

        ///鼠标射线是否与当前地表相交。
        bool intersect(Vector3 & position) const;

        ///射线是否与当前地表相交
        bool intersect(const Vector3 & origin, const Vector3 & dir, float & distance) const;

        ///aabb是否与地形相交
        bool intersect(const AABB & aabb) const;

        ///射线拾取
        SceneNodePtr pickItem(const Vector3 & origin, const Vector3 & dir);

    protected:

        void renderTerrain(IDirect3DDevice9* pDevice);
        void loadItem(LZDataPtr ptr);

        void updateNormal();
        void updateVertices();

        ///构造八叉树
        void buildOctree();

        ///加载场景
        void doLoading();

        ///场景加载完毕
        void onLoadingFinish();

    protected:
        uint32          m_id;
        TerrainMesh     m_mesh;
        TerrainMap *    m_pMap;
        bool            m_isLoaded;       ///< 场景是否已经加载完成
        bool            m_isLoading;    ///< 正在加载中

        float           m_gridSize;
        FRect           m_rect;         ///<地图x，z坐标区域
        OctreePtr       m_octree;       ///<地图物件包围盒构成的八叉树，用于快速拾取aabb
        bool            m_octreeDirty;  ///<八叉树重构标记

        ZCritical               m_itemLocker;

        float                   m_uvScale;
        TexturePtr              m_textures[MapConfig::MaxNbChunkTexture];
        EffectPtr               m_shader;

        friend class ChunkLoader;
    };

    typedef RefPtr<TerrainChunk> ChunkPtr;

} // end namespace Lazy
