

#include "stdafx.h"
#include "Res.h"
#include "Entity.h"
#include "TerrainChunk.h"
#include "App.h"
#include "TerrinMap.h"

#include "../Physics/Physics.h"

namespace MapConfig
{
    const size_t MagicNumber = Lazy::makeMagic('m', 'd', 'a', 't');
    const size_t Version = 0;

    struct ChunkHeader
    {
        size_t magic;
        size_t version;
        size_t sizeInfo;
    };
}

typedef WORD IndexType;    //< 索引顶点类型
DWORD INDEX_TYPE = D3DFMT_INDEX16;

namespace mapnode
{
    ObjCollider::ObjCollider(TerrainChunk* pMapNode)
        : m_pMapNode(pMapNode)
        , m_obj(nullptr)
    {
    }

    ///是否与八叉树叶结点碰撞。
    bool ObjCollider::query(Physics::OctreeBase *, const Physics::IndicesArray & indices)
    {
        //size_t numObj = m_pMapNode->getNumObj();

        float minDistance = Physics::MAX_FLOAT;

        for (size_t i : indices)
        {
            TerrainItemPtr obj = m_pMapNode->getObjByIndex(i);
            if (!obj) continue;

            Physics::AABB aabb;
            obj->getWorldAABB(aabb);
            if (!aabb.intersectsRay(origin, dir)) continue;

            float dist = origin.distToSq(obj->m_vPos);
            if (minDistance > dist)
            {
                minDistance = dist;
                m_obj = obj;
            }
        }

        return bool(m_obj);
    }


    class Octree : public Physics::Octree
    {
    public:
        Octree(const Physics::oc::Config & config)
            : Physics::Octree(config)
        {

        }

        bool build(TerrainChunk* pMapNode)
        {
            size_t n = pMapNode->getNumObj();
            m_aabbs.resize(n);

            for (size_t i = 0; i < n; ++i)
            {
                pMapNode->getObjByIndex(i)->getWorldAABB(m_aabbs[i]);
            }

            bool ret = doBuild();

            debugMessage(_T("TRACE: success:%d. octree has %u aabbs. maxDepth:%u, numNode:%u, numLeaf:%u, numBytes:%u"), 
                ret ? 1 : 0, m_aabbs.size(), numMaxDepth(), numNode(), numLeaf(), numBytes());

            return ret;
        }

    };
}

//////////////////////////////////////////////////////////////////////////
template<typename T>
bool updateIndices(VIBHolder * hoder, int nVertices)
{
    T *pIndex;
    if(!hoder->lockIB((void**)&pIndex)) return false;

    T vertices = (T)nVertices;
    T grid = vertices - 1;
    T i = 0;
    for (T r =0; r<grid; ++r)
    {
        for (T c=0; c<grid; ++c)
        {
            T n = r * vertices + c;
            pIndex[i] = n;
            pIndex[i+1] = n + 1;
            pIndex[i+2] = n + vertices;

            pIndex[i+3] = n + 1;
            pIndex[i+4] = n + vertices + 1;
            pIndex[i+5] = n + vertices;

            i += 6;
        }
    }

    hoder->unlockIB();
    return true;
}

bool VIBHolder::create(LPDIRECT3DDEVICE9 pDevice, int vertices)
{
    //const int grid = vertices - 1;
    const int numFace = TerrainConfig::MapNodeFace;
    //const int numIndices = numFace * 3;

    if(FAILED(D3DXCreateMeshFVF(numFace, 
        TerrainConfig::MaxMapNodeVerticesSq, 
        D3DXMESH_MANAGED, 
        TerrinVertex::FVF, pDevice, &m_pMesh)))
    {
        LOG_ERROR(L"Create VIBHolder Mesh failed");
        return false;
    }

    bool ret;
    if (m_pMesh->GetOptions() & D3DXMESH_32BIT)
        ret = updateIndices<DWORD>(this, vertices);
    else
        ret = updateIndices<WORD>(this, vertices);

    if(!ret)
    {
        SafeRelease(m_pMesh);
        return false;
    }

    MEMORY_CHECK_CONS(this);
    return ret;
}

void VIBHolder::optimize()
{
    if(!m_pMesh) return;

    D3DXComputeNormals(m_pMesh, NULL);
}


void VIBHolder::release()
{
    MEMORY_CHECK_DEST(this);
    SafeRelease(m_pMesh);
}

void VIBHolder::render(LPDIRECT3DDEVICE9)
{
    m_pMesh->DrawSubset(0);
}

//////////////////////////////////////////////////////////////////////////
/*static*/ VIBCache* VIBCache::instance()
{
    static VIBCache s_instance;
    return &s_instance;
}

VIBCache::VIBCache() 
{
}

VIBCache::~VIBCache() 
{
    clear();
}

void VIBCache::preLoadOne()
{
    Lazy::ZLockHolder lockHolder(&m_locker);

    m_pool.push_back(VIBHolder());
    m_pool.back().create(getApp()->getDevice(), TerrainConfig::MaxMapNodeVertices);
}

VIBHolder VIBCache::get()
{
    Lazy::ZLockHolder lockHolder(&m_locker);

    if (m_pool.empty())
    {
        VIBHolder holder;
        holder.create(getApp()->getDevice(), TerrainConfig::MaxMapNodeVertices);
        return holder;
    }
    else
    {
        VIBHolder holder = m_pool.front();
        m_pool.pop_front();
        return holder;
    }
}

void VIBCache::release(VIBHolder & holder)
{
    Lazy::ZLockHolder lockHolder(&m_locker);

    if(!holder.valid()) return ;

    if (m_pool.size() < TerrainConfig::MaxMapNodeCacheSize)
    {
        m_pool.push_back(holder);
        holder.clear();
    }
    else
    {
        holder.release();
    }
}

void VIBCache::clear()
{
    Lazy::ZLockHolder lockHolder(&m_locker);

    for (CachePool::iterator it = m_pool.begin();
        it != m_pool.end(); ++it)
    {
        it->release();
    }

    m_pool.clear();
}

//////////////////////////////////////////////////////////////////////////

///后台加载任务
class ChunkLoader : public Lazy::TaskInterface
{
    TerrainChunk * m_pChunk;

public:
    ChunkLoader(TerrainChunk * pChunk)
        : m_pChunk(pChunk)
    {
    }

    ~ChunkLoader()
    {
    }

    virtual void doTask(void) override
    {
        m_pChunk->doLoading();
    }

    virtual void onTaskFinish(void) override
    {
        m_pChunk->onLoadingFinish();
    }
};


//////////////////////////////////////////////////////////////////////////

TerrainChunk::TerrainChunk(TerrinData* pData, int rID, int cID)
    : m_pTerrin(pData)
    , m_loaded(false)
    , m_rID(rID)
    , m_cID(cID)
    , m_octreeDirty(true)
    , m_loadRunning(false)
{

    m_id = TerrainMap::instance()->nodeCols() * rID + cID;

    m_gridSize = pData->squareSize();
    m_size = m_gridSize * TerrainConfig::MaxMapNodeGrid;
    m_z0 = m_rID * m_size;
    m_x0 = m_cID * m_size;

    m_rect.left = xToMap(0);
    m_rect.top = zToMap(0);
    m_rect.right = xToMap(m_size);
    m_rect.bottom = zToMap(m_size);
    m_rect.normalization();

}

TerrainChunk::~TerrainChunk(void)
{
    release();

}

float TerrainChunk::xToMap(float x) const
{
    return x + m_x0 - m_pTerrin->width()/2.0f;
}

float TerrainChunk::zToMap(float z) const
{
    return m_pTerrin->height()/2.0f - (z + m_z0);
}


float TerrainChunk::rToMapZ(int r) const
{
#if USE_NEW_CHUNK_STYLE
    return m_rect.bottom - r * m_gridSize;
#else
    return m_pTerrin->height() / 2.0f - (r * m_gridSize + m_z0);
#endif
}

float TerrainChunk::cToMapX(int c) const
{
#if USE_NEW_CHUNK_STYLE
    return m_rect.left + c * m_gridSize;
#else
    return c * m_gridSize + m_x0 - m_pTerrin->width() / 2.0f;
#endif
}

float TerrainChunk::getHeight(int r, int c) const
{
#if USE_NEW_CHUNK_STYLE
    return m_heightMap[r * MapConfig::ChunkVertices + c];
#else
    return m_pTerrin->getHeight(r + int(m_z0 / m_gridSize), c + int(m_x0 / m_gridSize));
#endif
}

Square2 TerrainChunk::squareToMap(int r, int c)
{
    return Square2(r + int(m_z0/m_gridSize), c + int(m_x0/m_gridSize));
}

void TerrainChunk::getPos(int index, D3DXVECTOR3 & pos)
{
    int c = index % TerrainConfig::MaxMapNodeGrid;
    int r = index / TerrainConfig::MaxMapNodeGrid;

    Square2 sq = squareToMap(r, c);

    if (sq.r < 0 ||
        sq.r >= m_pTerrin->vrows() ||
        sq.c < 0 ||
        sq.c >= m_pTerrin->vcols())
    {
        ZeroMemory(&pos, sizeof(pos));
        return ;
    }

    pos.x = xToMap(c * m_gridSize);
    pos.y = m_pTerrin->getHeight(r, c);
    pos.z = zToMap(r * m_gridSize);
}

void TerrainChunk::load()
{
    if(m_loaded) return;
    if(m_loadRunning) return;

    m_loadRunning = true;
    if (MapConfig::UseMultiThread)
    {
        Lazy::LoadingMgr::instance()->addTask(new ChunkLoader(this));
    }
    else
    {
        doLoading();
        onLoadingFinish();
    }
}

///加载场景
void TerrainChunk::doLoading()
{
    if (m_loaded) return;

    updateVertices();

    if (MapConfig::UseNewFormat)
    {
        Lazy::tstring path = Lazy::getFilePath(TerrainMap::instance()->getMapName());

        Lazy::tstring name;
        formatString(name, _T("%4.4d%4.4d.lzd"), m_rID, m_cID);
        path += name;

        debugMessage(_T("TRACE: load node(%d %d) path='%s'"), m_rID, m_cID, path.c_str());

        Lazy::LZDataPtr root = Lazy::openSection(path);
        if (!root)
        {
            LOG_ERROR(L"Load failed! node(%d %d) path='%s'", m_rID, m_cID, path.c_str());
            return;
        }

        m_objPool.clear();
        Lazy::LZDataPtr itemsDatas = root->read(_T("items"));
        if (itemsDatas)
        {
            for (Lazy::LZDataPtr ptr : (*itemsDatas))
            {
                if (!m_loadRunning) return;

                if (ptr->tag() == L"item" && ptr->countChildren() > 0)
                {
                    TerrainItemPtr item = new TerrainItem();
                    item->load(ptr);
                    addItem(item);
                }
            }
        }

        debugMessage(_T("TRACE: finish load node(%d %d) path='%s'"), m_rID, m_cID, path.c_str());
    }

}

///场景加载完毕
void TerrainChunk::onLoadingFinish()
{
    Lazy::ZLockHolder lockHoder(&m_objLocker);

    m_loaded = true;
    m_loadRunning = false;

    //合并外部引用
    if (!m_externalItems.empty())
    {
        m_objPool.insert(m_objPool.end(), m_externalItems.begin(), m_externalItems.end());
        m_externalItems.clear();
    }

    LOG_DEBUG(L"Chunk(%d %d) load finish.", m_rID, m_cID);
}



void TerrainChunk::updateVertices()
{
    if (!m_vibHolder.valid())
    {
        m_vibHolder = VIBCache::instance()->get();
        if (!m_vibHolder.valid())
        {
            LOG_ERROR(_T("TerrainChunk::load get vib failed!"));
            return;
        }
    }

    float su = 1.0f / m_size;
    float sv = 1.0f / m_size;
    bool onTex = m_pTerrin->useOneTex();

    TerrinVertex* pVertex = NULL;
    if (!m_vibHolder.lockVB((void**) &pVertex, 0))
    {
        LOG_ERROR(_T("TerrainChunk::load lock vertexbuffer faild"));
        return;
    }

    //以下是创建网格数据
#if USE_NEW_CHUNK_STYLE
    const int vertices = MapConfig::ChunkVertices;
#else
    const int vertices = TerrainConfig::MaxMapNodeVertices;
#endif
    
    for (int r = 0; r < vertices; ++r)
    {
        for (int c = 0; c < vertices; ++c)
        {
            int i = r * vertices + c;//顶点索引
            
#if 1// USE_NEW_CHUNK_STYLE
            pVertex[i].pos.x = cToMapX(c);
            pVertex[i].pos.z = rToMapZ(r);
            pVertex[i].pos.y = getHeight(r, c);
#else
            float mx = xToMap(c * m_gridSize);
            float mz = zToMap(r * m_gridSize);
            pVertex[i].pos.x = mx;
            pVertex[i].pos.z = mz;
            pVertex[i].pos.y = m_pTerrin->getPhysicalHeight(mx, mz);
#endif

            if (onTex)
            {
                pVertex[i].u = c * m_gridSize * su * 4;
                pVertex[i].v = r * m_gridSize * sv * 4;
            }
            else
            {
                pVertex[i].u = float(r % 2);
                pVertex[i].v = float(c % 2);
            }
        }
    }

    m_vibHolder.unlockVB();

    updateNormal();
}

void TerrainChunk::save()
{
    if(!m_loaded)
    {
        return;
    }

    Lazy::tstring path = Lazy::getFilePath(TerrainMap::instance()->getMapName());

    Lazy::tstring name;
    formatString(name, _T("%4.4d%4.4d.lzd"), m_rID, m_cID);
    path += name;

    debugMessage(_T("TRACE: save node(%d %d) path='%s'"), m_rID, m_cID, path.c_str());

    Lazy::LZDataPtr root = Lazy::openSection(path, true);
    root->clearChildren();
    
#if 1
    {//save time.juse for test
        Lazy::tstring strTime;
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);
    
        formatString(strTime, _T("[%d.%d.%d][%d.%d.%d][%d]"), 
            sysTime.wYear, sysTime.wMonth, sysTime.wDay,
            sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
        root->writeString(_T("savetime"), strTime);
    }
#endif

    Lazy::LZDataPtr items = root->write(_T("items"));
    items->clearChildren();

    for(TerrainItemPtr item : m_objPool)
    {
        if (item->isReference()) continue;

        Lazy::LZDataPtr itemData = items->newOne(L"item", L"");
        if (item->save(itemData))
        {
            items->addChild(itemData);
        }
    }

    if(!Lazy::saveSection(root, path))
    {
        LOG_ERROR(L"Save failed! node(%d %d) path='%s'", m_rID, m_cID, path.c_str());
    }
}

void TerrainChunk::render(IDirect3DDevice9* pDevice)
{
    if (!m_loaded)
    {
        load();
        return;
    }

    if (!m_vibHolder.valid()) return ;

    Matrix4x4 matWord;
    matWord.makeIdentity();
    pDevice->SetTransform(D3DTS_WORLD, &matWord);
    m_vibHolder.render(pDevice);
}

void TerrainChunk::update(float elapse)
{
    if (!m_loaded) return;

    if (m_octreeDirty) buildOctree();

    for (TerrainItemPtr item : m_objPool)
    {
        item->update(elapse);
    }
}

void TerrainChunk::renderObj(IDirect3DDevice9* pDevice)
{
    if (!m_loaded) return;

    for (TerrainItemPtr item : m_objPool)
    {
        item->render(pDevice);
    }

    //调试渲染当前场景的八叉树
    if (MapConfig::ShowChunkOctree && TerrainMap::instance()->currentNode() == this)
    {
        pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

        Matrix4x4 matWord;
        D3DXMatrixIdentity(&matWord);
        pDevice->SetTransform(D3DTS_WORLD, &matWord);
        if (m_octree) m_octree->render(pDevice);

        pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    }
}

void TerrainChunk::release(void)
{
    clearObj();

    if (m_loaded)
    {
        VIBCache::instance()->release(m_vibHolder);
        m_loaded = false;
    }

    m_loadRunning = false;
}

void TerrainChunk::clearObj()
{
    m_objPool.clear();
    m_externalItems.clear();

    m_octree = nullptr;
    m_octreeDirty = true;
}

//鼠标是否与当前地形相交。
bool TerrainChunk::intersect(Physics::Vector3 & position) const
{
    if(!m_loaded) return false;
    if (!m_vibHolder.valid()) return false;

    float distance;
    if (!intersect(cPick::instance()->rayPos(), cPick::instance()->rayDir(), distance)) return false;

    position = cPick::instance()->rayDir();
    position *= distance;
    position += cPick::instance()->rayPos();

    return true;
}

///射线是否与当前地表相交
bool TerrainChunk::intersect(const Physics::Vector3 & origin, const Physics::Vector3 & dir, float & distance) const
{
    if (!m_loaded) return false;
    if (!m_vibHolder.valid()) return false;

    Physics::Matrix4x4 mat;
    D3DXMatrixIdentity(&mat);
    Physics::RayCollision collider(origin, dir);

    if (!collider.pick(m_vibHolder.getMesh(), mat)) return false;

    distance = collider.m_hitDistance;
    return true;
}

bool TerrainChunk::intersect(const Physics::AABB & aabb) const
{
    if(m_x0 > aabb.max.x) return false;
    if(m_x0 + m_size < aabb.min.x) return false;
    if(m_z0 > aabb.max.z) return false;
    if(m_z0 + m_size < aabb.min.z) return false;

    return true;
}

///射线拾取
TerrainItemPtr TerrainChunk::pickObj(const Physics::Vector3 & origin, const Physics::Vector3 & dir)
{
    if (!m_octree) return nullptr;

    mapnode::ObjCollider collider(this);
    collider.origin = origin;
    collider.dir = dir;
    if (!m_octree->pick(&collider)) return nullptr;

    return collider.getObj();
}

void TerrainChunk::updateNormal(void)
{
    m_vibHolder.optimize();
}

void TerrainChunk::addItem(TerrainItemPtr item)
{
    item->setChunkId(m_id);
    m_objPool.push_back(item);

    Physics::AABB aabb;
    item->getWorldAABB(aabb);

    if (aabb.min.x >= m_rect.left &&
        aabb.max.x <= m_rect.right &&
        aabb.min.z >= m_rect.top &&
        aabb.max.z <= m_rect.bottom)
    {
        return;
    }
  
    TerrainMap::instance()->processExternalItem(item);
}

void TerrainChunk::addExternal(TerrainItemPtr item)
{
    if (item->getChunkId() == m_id) return;

    Lazy::ZLockHolder lockHoder(&m_objLocker);

    TerrainItemPtr clone = item->clone();
    clone->setChunkId(m_id);
    clone->setReference(true);
    clone->setRefChunk(item->getChunkId());

    if (m_loaded) m_objPool.push_back(clone);
    else m_externalItems.push_back(clone);

    m_octreeDirty = true;
}

void TerrainChunk::buildOctree()
{
    if (!m_loaded) return;
    if (!m_octreeDirty) return;
    m_octreeDirty = false;

    //构建物件八叉树
    LOG_INFO(L"build chunk %d octree.", m_id);

    Physics::oc::Config config;
    config.best = true;
    config.maxDepth = 16;
    config.minCount = 1;
    config.minSize = 100.0f;

    m_octree = nullptr;

    mapnode::Octree *pTree = new mapnode::Octree(config);
    if (!pTree->build(this))
    {
        LOG_ERROR(L"build chunk %d octree failed!", m_id);
    }
    else
    {
        m_octree = pTree;
    }
}

#if USE_NEW_CHUNK_STYLE

TerrainChunk::TerrainChunk()
    : m_id(0)
{

}


bool TerrainChunk::load(const Lazy::tstring & mapPath, int id)
{
    Lazy::tstring path = mapPath;
    Lazy::formatDirName(path);

    Lazy::tstring name;
    formatString(name, L"%d.mdat", id);
    path += name;

    Lazy::FileHoder file = Lazy::getfs()->openFile(path, L"r");
    if (!file) return false;

    MapConfig::ChunkHeader header;
    fread(&header, sizeof(header), 1, file.ptr());
    if (header.magic != MapConfig::MagicNumber) return false;

    Lazy::DataStream stream;
    if (!stream.readFromFile(file.ptr(), header.sizeInfo)) return false;

    stream.loadStruct(m_id);
    stream.loadStruct(m_gridSize);
    stream.loadStruct(m_rect);

    size_t n;
    stream.loadStruct(n);
    m_heightMap.resize(n);
    fread(&m_heightMap[0], n * sizeof(float) , 1, file.ptr());
    return true;
}

bool TerrainChunk::save(const Lazy::tstring & mapPath)
{
    Lazy::tstring path = Lazy::getfs()->defaultPath();
    path += mapPath;
    Lazy::formatDirName(path);
    
    Lazy::tstring name;
    formatString(name, L"%d.mdat", m_id);
    path += name;

    Lazy::FileHoder file = Lazy::getfs()->openFile(path, L"wb");
    if (!file) return false;

    Lazy::DataStream stream;
    stream.saveStruct(m_id);
    stream.saveStruct(m_gridSize);
    stream.saveStruct(m_rect);

    size_t n = m_heightMap.size();
    stream.saveStruct(n);

    MapConfig::ChunkHeader header = {
        MapConfig::MagicNumber,
        MapConfig::Version,
        stream.pos(),
    };

    fwrite(&header, sizeof(header), 1, file.ptr());
    stream.writeToFile(file.ptr());
    fwrite(&m_heightMap[0], n * sizeof(float), 1, file.ptr());
    return true;
}

void TerrainChunk::create(int id, float x0, float y0)
{
    release();

    m_id = id;

    m_size = MapConfig::ChunkSize;
    m_gridSize = MapConfig::ChunkGridSize;

    m_rect.left = x0;
    m_rect.right = x0 + m_size;
    m_rect.bottom = y0;
    m_rect.top = y0 - m_size;

    int n = MapConfig::ChunkVertices * MapConfig::ChunkVertices;
    m_heightMap.resize(n);
    for (int i = 0; i < n; ++i)
    {
        m_heightMap[i] = 1.0f;
    }

    updateVertices();
}

#endif