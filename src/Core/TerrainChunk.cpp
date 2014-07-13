

#include "stdafx.h"
#include "Entity.h"
#include "TerrainChunk.h"
#include "App.h"
#include "TerrinMap.h"

#include "../Physics/Physics.h"
#include <sstream>

namespace Lazy
{
    namespace MapConfig
    {
        const size_t MagicNumber = makeMagic('m', 'd', 'a', 't');
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
        bool ObjCollider::query(OctreeBase *, const IndicesArray & indices)
        {
            //size_t numObj = m_pMapNode->getNumObj();

            float minDistance = MAX_FLOAT;

            for (size_t i : indices)
            {
                TerrainItemPtr obj = m_pMapNode->getItemByIndex(i);
                if (!obj) continue;

                AABB aabb;
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


        class MyOctree : public Octree
        {
        public:
            MyOctree(const oc::Config & config)
                : Octree(config)
            {

            }

            bool build(TerrainChunk* pMapNode)
            {
                size_t n = pMapNode->getNbItems();
                m_aabbs.resize(n);

                for (size_t i = 0; i < n; ++i)
                {
                    pMapNode->getItemByIndex(i)->getWorldAABB(m_aabbs[i]);
                }

                bool ret = doBuild();

                debugMessage(_T("TRACE: success:%d. octree has %u aabbs. maxDepth:%u, numNode:%u, numLeaf:%u, numBytes:%u"),
                             ret ? 1 : 0, m_aabbs.size(), numMaxDepth(), numNode(), numLeaf(), numBytes());

                return ret;
            }

        };
    }

    //////////////////////////////////////////////////////////////////////////

    /** 线性插值*/
    static float lerp(float a, float b, float t)
    {
        return a - (a * t) + (b * t);
    }

    template<typename T>
    static bool updateIndices(TerrainMesh * hoder, int nVertices)
    {
        T *pIndex;
        if(!hoder->lockIB((void**)&pIndex)) return false;

        T vertices = (T)nVertices;
        T grid = vertices - 1;
        T i = 0;
        for (T r = 0; r < grid; ++r)
        {
            for (T c = 0; c < grid; ++c)
            {
                T n = r * vertices + c;
                pIndex[i] = n;
                pIndex[i + 1] = n + 1;
                pIndex[i + 2] = n + vertices;

                pIndex[i + 3] = n + 1;
                pIndex[i + 4] = n + vertices + 1;
                pIndex[i + 5] = n + vertices;

                i += 6;
            }
        }

        hoder->unlockIB();
        return true;
    }


    //////////////////////////////////////////////////////////////////////////
    /// TerrainMesh
    //////////////////////////////////////////////////////////////////////////

    TerrainMesh::TerrainMesh()
        : m_pMesh(NULL)
    {}

    bool TerrainMesh::lockVB(void** pp, DWORD flag) const
    {
        return SUCCEEDED(m_pMesh->LockVertexBuffer(flag, pp));
    }

    void TerrainMesh::unlockVB() const
    { 
        m_pMesh->UnlockVertexBuffer();
    }

    bool TerrainMesh::lockIB(void**pp, DWORD flag) const
    {
        return SUCCEEDED(m_pMesh->LockIndexBuffer(flag, pp));
    }

    void TerrainMesh::unlockIB() const
    {
        m_pMesh->UnlockIndexBuffer();
    }

    bool TerrainMesh::create(LPDIRECT3DDEVICE9 pDevice)
    {

        if (FAILED(D3DXCreateMeshFVF(MapConfig::NbChunkGridFace,
                                    MapConfig::NbChunkVertexSq,
                                    D3DXMESH_MANAGED,
                                    TerrinVertex::FVF, pDevice, &m_pMesh)))
        {
            LOG_ERROR(L"Create TerrainMesh Mesh failed");
            return false;
        }

        bool ret;
        if (m_pMesh->GetOptions() & D3DXMESH_32BIT)
            ret = updateIndices<DWORD>(this, MapConfig::NbChunkVertex);
        else
            ret = updateIndices<WORD>(this, MapConfig::NbChunkVertex);

        if(!ret)
        {
            SafeRelease(m_pMesh);
            return false;
        }

        MEMORY_CHECK_CONS(this);
        return ret;
    }

    void TerrainMesh::optimize()
    {
        if(!m_pMesh) return;

        D3DXComputeNormals(m_pMesh, NULL);
    }


    void TerrainMesh::release()
    {
        MEMORY_CHECK_DEST(this);
        SafeRelease(m_pMesh);
    }

    void TerrainMesh::clear()
    {
        m_pMesh = NULL;
    }

    void TerrainMesh::render(LPDIRECT3DDEVICE9)
    {
        m_pMesh->DrawSubset(0);
    }

    //////////////////////////////////////////////////////////////////////////
    /// TerrainMesh cache
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
        ZLockHolder lockHolder(&m_locker);

        m_pool.push_back(TerrainMesh());
        m_pool.back().create(getApp()->getDevice());
    }

    TerrainMesh VIBCache::get()
    {
        ZLockHolder lockHolder(&m_locker);

        if (m_pool.empty())
        {
            TerrainMesh holder;
            holder.create(getApp()->getDevice());
            return holder;
        }
        else
        {
            TerrainMesh holder = m_pool.front();
            m_pool.pop_front();
            return holder;
        }
    }

    void VIBCache::release(TerrainMesh & holder)
    {
        ZLockHolder lockHolder(&m_locker);

        if(!holder.valid()) return ;

        if (m_pool.size() < MapConfig::MaxNbChunkMesh)
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
        ZLockHolder lockHolder(&m_locker);

        for (CachePool::iterator it = m_pool.begin();
                it != m_pool.end(); ++it)
        {
            it->release();
        }

        m_pool.clear();
    }

    //////////////////////////////////////////////////////////////////////////
    /// 后台加载任务
    //////////////////////////////////////////////////////////////////////////
    class ChunkLoader : public TaskInterface
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
    /// TerrainChunk
    //////////////////////////////////////////////////////////////////////////
    TerrainChunk::TerrainChunk(TerrainMap *pMap, uint32 id, const FRect & rect)
        : m_id(id)
        , m_pMap(pMap)
        , m_isLoaded(false)
        , m_isLoading(false)
        , m_octreeDirty(true)
        , m_rect(rect)
    {
        m_gridSize = MapConfig::ChunkGridSize;
    }

    TerrainChunk::~TerrainChunk(void)
    {
        release();
    }

    bool TerrainChunk::load()
    {
        if (m_isLoaded || m_isLoading) return false;

        m_isLoading = true;
        if (MapConfig::UseMultiThread)
        {
            LoadingMgr::instance()->addTask(new ChunkLoader(this));
        }
        else
        {
            doLoading();
            onLoadingFinish();
        }

        return true;
    }

    void TerrainChunk::doLoading()
    {
        if (m_isLoaded) return;

        tstring heightmap;
        formatString(heightmap, _T("%8.8x.raw"), m_id);
        heightmap = m_pMap->getMapName() + heightmap;
        if (!loadHeightMap(heightmap))
        {
            LOG_ERROR(L"Load heightmap '%s' failed! node(%d %d)",
                heightmap.c_str(), getRowID(), getColID());
        }

        tstring chunkname;
        formatString(chunkname, _T("%8.8x.lzd"), m_id);
        chunkname = m_pMap->getMapName() + chunkname;

        LOG_DEBUG(_T("Load chunk(%d %d) path='%s'"),
            getRowID(), getColID(), chunkname.c_str());

        LZDataPtr root = openSection(chunkname);
        if (root)
        {
            LZDataPtr itemsDatas = root->read(_T("items"));
            if (itemsDatas)
            {
                for (LZDataPtr ptr : (*itemsDatas))
                {
                    if (!m_isLoading)//加载结束
                        return;

                    if (ptr->tag() == L"item" && ptr->countChildren() > 0)
                    {
                        loadItem(ptr);
                    }
                }
            }
        }
        else
        {
            LOG_ERROR(L"Load failed! node(%d %d) path='%s'",
                getRowID(), getColID(), chunkname.c_str());
        }

        LOG_DEBUG(_T("Finish load node(%d %d) path='%s'"),
            getRowID(), getColID(), chunkname.c_str());
    }

    ///场景加载完毕
    void TerrainChunk::onLoadingFinish()
    {
        ZLockHolder lockHoder(&m_itemLocker);

        m_isLoaded = true;
        m_isLoading = false;

        LOG_DEBUG(L"Chunk(%d %d) load finish.", getRowID(), getColID());
    }

    void TerrainChunk::loadItem(LZDataPtr ptr)
    {
        uint32 itemID = ptr->readUint(L"id");
        if (findItem(itemID))
            return;

        TerrainItemPtr item = m_pMap->createTerrainItem();
        item->load(ptr);
        
        bool isAdded = false;
        tstring chunkIDs = ptr->readString(L"chunks");
        std::wistringstream ss(chunkIDs);
        while (!item && ss.good())
        {
            uint32 id = 0xffffffff;
            ss >> id;
            if (id == 0xffffffff)
                break;

            isAdded = true;
            ChunkPtr chunk = m_pMap->getChunkByID(id);
            chunk->addItem(item);
        }

        if (!isAdded)
        {
            m_pMap->addTerrainItem(item);
        }
    }

    float TerrainChunk::getHeight(int r, int c) const
    {
        size_t index = r * MapConfig::NbChunkVertex + c;
        if (index >= m_heightMap.size()) return 0.0f;

        return m_heightMap.at(index);
    }

    bool TerrainChunk::loadHeightMap(const tstring & filename)
    {
        m_heightMap.resize(MapConfig::NbChunkVertexSq, 0.0f);

        FILE *pFile = getfs()->openFile(filename, L"rb");
        if (pFile)
        {
            fread(&m_heightMap[0], 1, m_heightMap.size(), pFile);
            fclose(pFile);
        }

        updateVertices();
        return true;
    }

    bool TerrainChunk::saveHeightMap(const tstring & filename)
    {
        FILE *pFile = getfs()->openFile(filename, L"wb");
        if (!pFile) return false;

        fwrite(&m_heightMap[0], 1, m_heightMap.size(), pFile);
        fclose(pFile);        
        return true;
    }

    void TerrainChunk::updateVertices()
    {
        if (!m_mesh.valid())
        {
            m_mesh = VIBCache::instance()->get();
            if (!m_mesh.valid())
            {
                LOG_ERROR(_T("TerrainChunk::load get vib failed!"));
                return;
            }
        }

        TerrinVertex* pVertex = NULL;
        if (!m_mesh.lockVB((void**) &pVertex, 0))
        {
            LOG_ERROR(_T("TerrainChunk::load lock vertexbuffer faild"));
            return;
        }

        float uvScale = m_pMap->getUVScale();

        //以下是创建网格数据
        const int vertices = MapConfig::NbChunkVertex;
        for (int r = 0; r < vertices; ++r)
        {
            for (int c = 0; c < vertices; ++c)
            {
                int i = r * vertices + c;//顶点索引

                pVertex[i].pos.x = m_rect.left + c * m_gridSize;
                pVertex[i].pos.z = m_rect.top + r * m_gridSize;
                pVertex[i].pos.y = getHeight(r, c);

                pVertex[i].u = pVertex[i].pos.z * uvScale;
                pVertex[i].v = pVertex[i].pos.x * uvScale;
            }
        }

        m_mesh.unlockVB();
        updateNormal();
    }

    bool TerrainChunk::save()
    {
        if(!m_isLoaded)
        {
            LOG_ERROR(L"The chunk(%d, %d) does't loaded.", getRowID(), getColID());
            return false;
        }

        if (m_isLoading)
        {
            LOG_ERROR(L"The chunk(%d, %d) is loading now.", getRowID(), getColID());
            return false;
        }

        //save height map
        tstring heightmap;
        formatString(heightmap, _T("%8.8x.raw"), m_id);
        heightmap = m_pMap->getMapName() + heightmap;
        if (!saveHeightMap(heightmap))
        {
            LOG_ERROR(L"Failed to save heightmap '%s'", heightmap.c_str());
            //return false;
        }

        //save chunkdata.
        tstring chunkfile;
        formatString(chunkfile, _T("%8.8x.lzd"), m_id);
        chunkfile = m_pMap->getMapName() + chunkfile;

        LOG_DEBUG(_T("save node(%d %d) path='%s'"), getRowID(), getColID(), chunkfile.c_str());

        LZDataPtr root = openSection(chunkfile, true);
        root->clearChildren();

#if 1
        {
            //save time.juse for test
            tstring strTime;
            SYSTEMTIME sysTime;
            GetLocalTime(&sysTime);

            formatString(strTime, _T("[%d.%d.%d][%d.%d.%d][%d]"),
                         sysTime.wYear, sysTime.wMonth, sysTime.wDay,
                         sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
            root->writeString(_T("savetime"), strTime);
        }
#endif

        LZDataPtr items = root->write(_T("items"));
        items->clearChildren();

        for(TerrainItemPtr item : m_items)
        {
            LZDataPtr itemData = items->newOne(L"item", L"");
            if (item->save(itemData))
            {
                items->addChild(itemData);
            }
        }

        if (!saveSection(root, chunkfile))
        {
            LOG_ERROR(L"Save failed! node(%d %d) path='%s'", getRowID(), getColID(), chunkfile.c_str());
            return false;
        }

        return true;
    }

    void TerrainChunk::renderTerrain(IDirect3DDevice9* pDevice)
    {
        if (!m_isLoaded)
        {
            load();
            return;
        }

        if (!m_mesh.valid()) return ;

        pDevice->SetTransform(D3DTS_WORLD, &matIdentity);
        m_mesh.render(pDevice);
    }

    void TerrainChunk::update(float elapse)
    {
        if (!m_isLoaded) return;

        if (m_octreeDirty) buildOctree();

        for (TerrainItemPtr item : m_items)
        {
            item->update(elapse);
        }
    }

    void TerrainChunk::renderItems(IDirect3DDevice9* pDevice)
    {
        if (!m_isLoaded) return;

        for (TerrainItemPtr item : m_items)
        {
            //一个item可以横跨多个chunk，只有中心坐标所在的chunk才负责渲染。
            if (m_rect.isIn(item->getPos().x, item->getPos().z))
            {
                item->render(pDevice);
            }
        }

        //调试渲染当前场景的八叉树
        if (MapConfig::ShowChunkOctree && TerrainMap::instance()->currentNode() == this)
        {
            pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

            pDevice->SetTransform(D3DTS_WORLD, &matIdentity);
            if (m_octree) m_octree->render(pDevice);

            pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
        }
    }

    void TerrainChunk::release(void)
    {
        clearItems();

        if (m_isLoaded)
        {
            VIBCache::instance()->release(m_mesh);
            m_isLoaded = false;
        }

        m_isLoading = false;
    }

    //鼠标是否与当前地形相交。
    bool TerrainChunk::intersect(Vector3 & position) const
    {
        if(!m_isLoaded || m_isLoading) return false;
        if (!m_mesh.valid()) return false;

        float distance;
        if (!intersect(Pick::instance()->rayPos(), Pick::instance()->rayDir(), distance)) return false;

        position = Pick::instance()->rayDir();
        position *= distance;
        position += Pick::instance()->rayPos();

        return true;
    }

    ///射线是否与当前地表相交
    bool TerrainChunk::intersect(const Vector3 & origin, const Vector3 & dir, float & distance) const
    {
        if (!m_isLoaded || m_isLoading) return false;
        if (!m_mesh.valid()) return false;

        RayCollision collider(origin, dir);

        if (!collider.pick(m_mesh.getMesh(), matIdentity)) return false;

        distance = collider.m_hitDistance;
        return true;
    }

    bool TerrainChunk::intersect(const AABB & aabb) const
    {
        if(m_rect.right > aabb.max.x) return false;
        if(m_rect.left < aabb.min.x) return false;
        if(m_rect.bottom > aabb.max.z) return false;
        if(m_rect.top < aabb.min.z) return false;

        return true;
    }

    ///射线拾取
    TerrainItemPtr TerrainChunk::pickItem(const Vector3 & origin, const Vector3 & dir)
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
        m_mesh.optimize();
    }

    TerrainItemPtr TerrainChunk::findItem(uint32 id)
    {
        ZLockHolder hodler(&m_itemLocker);

        ItemCatalogue::iterator it = m_itemCatalogue.find(id);
        if (it == m_itemCatalogue.end())
            return nullptr;
        return it->second;
    }

    void TerrainChunk::delItem(TerrainItemPtr item)
    {
        ZLockHolder hodler(&m_itemLocker);

        ItemCatalogue::iterator it = m_itemCatalogue.find(item->getID());
        if (it == m_itemCatalogue.end())
            return; //doesn't found.

        m_itemCatalogue.erase(item->getID());
        m_items.erase(std::find(m_items.begin(), m_items.end(), item));
        m_octreeDirty = true;
    }

    void TerrainChunk::addItem(TerrainItemPtr item)
    {
        ZLockHolder hodler(&m_itemLocker);

        auto it = m_itemCatalogue.insert(std::make_pair(item->getID(), item));
        if (!it.second) //has been added.
            return;

        m_items.push_back(item);
        item->addChunk(this);
        m_octreeDirty = true;
    }

    void TerrainChunk::clearItems()
    {
        ZLockHolder hodler(&m_itemLocker);

        m_items.clear();
        m_itemCatalogue.clear();

        m_octree = nullptr;
        m_octreeDirty = true;
    }

    void TerrainChunk::buildOctree()
    {
        if (!m_isLoaded || m_isLoading) return;
        if (!m_octreeDirty) return;
        m_octreeDirty = false;

        //构建物件八叉树
        LOG_INFO(L"build chunk %d octree.", m_id);

        oc::Config config;
        config.best = true;
        config.maxDepth = 16;
        config.minCount = 1;
        config.minSize = 100.0f;

        m_octree = nullptr;

        mapnode::MyOctree *pTree = new mapnode::MyOctree(config);
        if (!pTree->build(this))
        {
            LOG_ERROR(L"build chunk %d octree failed!", m_id);
        }
        else
        {
            m_octree = pTree;
        }
    }

    /** 获得物理高度*/
    float TerrainChunk::getPhysicalHeight(float x, float z) const
    {
        if (!m_rect.isIn(x, z)) return getHeight(0, 0);

        //将地图移动到原点,方便计算
        x = (x - m_rect.left) / m_gridSize;
        z = (z - m_rect.top) / m_gridSize;

        //计算x,z坐标所在的行列值
        int col = int(x);//向下取整
        int row = int(z);

        // 获取如下图4个顶点的高度
        //
        //  A   B
        //  *---*
        //  | / |
        //  *---*
        //  C   D

        float A = getHeight(row, col);
        float B = getHeight(row, col + 1);
        float C = getHeight(row + 1, col);
        float D = getHeight(row + 1, col + 1);

        float dx = x - col;
        float dz = z - row;

        float height;
        if (dz < 1.0f - dx)//(x,z)点在ABC三角形上
        {
            float uy = B - A;
            float vy = C - A;

            height = A + lerp(0.0f, uy, dx) + lerp(0.0f, vy, dz);//线形插值得到高度
        }
        else//(x,z)点在BCD三角形上
        {
            float uy = C - D;
            float vy = B - D;

            height = D + lerp(0.0f, uy, 1.0f - dx) + lerp(0.0f, vy, 1.0f - dz);
        }

        return height;
    }

} // end namespace Lazy
