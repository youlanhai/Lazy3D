#pragma once

/**
 *  当地图过大时，将地图分割成若干个小的地图，每次渲染时，仅渲染玩家周围的几个子地图，这样可以得到较高的效率。
 *  MapNode就是这样的子地图。
*/

#include "TerrinData.h"
#include "TerrainItem.h"
#include "../Physics/Octree.h"

#define USE_MESH 1


namespace TerrainConfig
{

    const int MaxMapNodeVertices = 51;
    const int MaxMapNodeVerticesSq = MaxMapNodeVertices * MaxMapNodeVertices;

    const int MaxMapNodeGrid = MaxMapNodeVertices - 1;
    const int MaxMapNodeGridSq = MaxMapNodeGrid * MaxMapNodeGrid;

    const int MapNodeFace = MaxMapNodeGridSq * 2;
    const int MapNodeIndices = MapNodeFace * 3;

    const int MaxMapNodeCacheSize = 64;

    const int NumVertexSharedTex = 4;
}


struct Square2
{
    Square2() : r(0), c(0) {}
    Square2(int r_, int c_) : r(r_), c(c_) { }

    void set(int r_, int c_)
    {
        r = r_; c = c_;
    }
    int r;
    int c;
};

class TerrainChunk;

namespace mapnode
{
    ///用于射线拾取地形obj
    class ObjCollider : public Physics::oc::RayCollider
    {
        TerrainItemPtr m_obj;
        TerrainChunk* m_pMapNode;

    public:
        ObjCollider(TerrainChunk* pMapNode);

        TerrainItemPtr getObj() const { return m_obj; }

        ///是否与八叉树叶结点碰撞。
        virtual bool query(Physics::OctreeBase* pTree, const Math::IndicesArray & indices) override;
    };
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class VIBHolder
{
public:

    VIBHolder() : m_pMesh(NULL) { }

    bool valid() const { return m_pMesh != NULL; }

    bool lockVB(void** pp, DWORD flag=0) const
    {
        return SUCCEEDED(m_pMesh->LockVertexBuffer(flag, pp));
    }

    void unlockVB() const {  m_pMesh->UnlockVertexBuffer(); }

    bool lockIB(void**pp, DWORD flag=0) const
    {
        return SUCCEEDED(m_pMesh->LockIndexBuffer(flag, pp));
    }

    void unlockIB() const { m_pMesh->UnlockIndexBuffer(); }

    bool create(LPDIRECT3DDEVICE9 pDevice, int vertices);

    void optimize();

    void render(LPDIRECT3DDEVICE9 pDevice);

    LPD3DXMESH getMesh() const { return m_pMesh; }
private:

    //释放资源。不允许显示释放。
    void release();
    void clear(){ m_pMesh = NULL; }

    friend class VIBCache;

    LPD3DXMESH m_pMesh; //将高度图生成mesh
};


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class VIBCache
{
    VIBCache() ;

    ~VIBCache() ;

public:

    size_t size() const { return m_pool.size(); }

    void preLoadOne();

    VIBHolder get();

    void release(VIBHolder & holder);

    void clear();

    static VIBCache* instance();

private:
    typedef std::list<VIBHolder> CachePool;

    CachePool   m_pool;
    Lazy::ZCritical m_locker;
};

typedef std::vector<TerrainItemPtr>  ObjectPool;
typedef ObjectPool::iterator    ObjectIter;
typedef ObjectPool::const_iterator    ObjectConstIter;


//////////////////////////////////////////////////////////////////////////
//地图结点
//////////////////////////////////////////////////////////////////////////
class TerrainChunk : public IBase
{
public:
    TerrainChunk(TerrinData* pData, int rID, int cID);

    ~TerrainChunk(void);

    void render(IDirect3DDevice9* pDevice);
    void update(float elapse);
    
    void load();
    void save();

    bool isLoaded() const { return m_loaded; }

#if USE_NEW_CHUNK_STYLE

public:

    TerrainChunk();

    int getId() const { return m_id; }
    void setId(int id){ m_id = id; }

    bool load(const Lazy::tstring & mapPath, int id);
    bool save(const Lazy::tstring & mapPath);

    void create(int id, float x0, float y0);

#endif

public://地图物体相关

    void renderObj(IDirect3DDevice9* pDevice);

    ///情况所有物体
    void clearObj();

    ObjectIter begin(){ return m_objPool.begin(); }
    ObjectIter end(){ return m_objPool.end(); }
    ObjectConstIter begin() const{ return m_objPool.begin(); }
    ObjectConstIter end() const { return m_objPool.end(); }

    TerrainItemPtr getObjByIndex(size_t i) const { return m_objPool[i]; }
    size_t getNumObj() const { return m_objPool.size(); }

    int rowID() const { return m_rID; }
    int colID() const { return m_cID; }

    float size() const { return m_size; }
    LPD3DXMESH getTerrainMesh() const { return m_vibHolder.getMesh(); }

    ///获得结点在世界坐标下的区域
    const Math::FRect & getRect() const{ return m_rect; }

    ///添加一个物体。如果物体的包围盒超出chunk边界，此物体将作为外部链接添加到相交的chunk里。
    void addItem(TerrainItemPtr item);

    ///添加外部引用
    void addExternal(TerrainItemPtr item);

    ///刷新外部引用。当外部引用的坐标、旋转、缩放变化时，调用。
    //void refreshExternal(TerrainItemPtr item);

public://拾取与碰撞检测

    ///鼠标射线是否与当前地表相交。
    bool intersect(Math::Vector3 & position) const;

    ///射线是否与当前地表相交
    bool intersect(const Math::Vector3 & origin, const Math::Vector3 & dir, float & distance) const;

    ///aabb是否与地形相交
    bool intersect(const Math::AABB & aabb) const;

    ///射线拾取
    TerrainItemPtr pickObj(const Math::Vector3 & origin, const Math::Vector3 & dir);

protected:

    ///将结点内的x坐标，转换为世界坐标
    float xToMap(float x) const;
    
    ///将结点内的z坐标，转换为世界坐标
    float zToMap(float z) const;

    float rToMapZ(int r) const;
    float cToMapX(int c) const;
    float getHeight(int r, int c) const;

    float x0() const { return m_x0; }
    float z0() const { return m_z0; }

    Square2 squareToMap(int r, int c);

    void release(void);

    void updateNormal();

    void getPos(int index, D3DXVECTOR3 & pos);

    void updateVertices();

    ///构造八叉树
    void buildOctree();

    ///加载场景
    void doLoading();

    ///场景加载完毕
    void onLoadingFinish();

protected:
    int         m_id;

    bool        m_loaded; ///< 场景是否已经加载完成
    bool        m_loadRunning; ///< 正在加载中

    Lazy::ZCritical m_objLocker; ///< 使用m_objPool和m_externalItems需要加锁
    ObjectPool  m_objPool;
    ObjectPool  m_externalItems;    ///<如果地形还未加载，使用此缓冲池记录外部引用

    VIBHolder   m_vibHolder;

    bool        m_octreeDirty;      ///<八叉树重构标记

    float       m_size;
    float       m_gridSize;

    Math::FRect   m_rect;///<地图x，z坐标区域
    Physics::OctreePtr m_octree;///<地图物件包围盒构成的八叉树，用于快速拾取aabb

    std::vector<float> m_heightMap;


    //旧数据

    TerrinData *m_pTerrin;

    float   m_x0;       //原点坐标
    float   m_z0;
    int     m_rID;
    int     m_cID;

    friend class ChunkLoader;
};

typedef RefPtr<TerrainChunk> ChunkPtr;