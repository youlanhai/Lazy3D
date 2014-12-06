

#include "stdafx.h"
#include "Entity.h"
#include "TerrainChunk.h"
#include "App.h"
#include "TerrainMap.h"
#include "SceneNodeFactory.h"

#include "../Physics/Physics.h"

#include <sstream>

namespace Lazy
{
    namespace MapConfig
    {
        int MaxNbChunkLayer = 4;

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
            float minDistance = MAX_FLOAT;

            for (size_t i : indices)
            {
                //TODO 完善碰撞
                SceneNodePtr obj /*= m_pMapNode->getItemByIndex(i)*/;
                if (!obj)
                    continue;

                AABB aabb = obj->getWorldBoundingBox();
                if (!aabb.intersectsRay(origin, dir)) continue;

                float dist = origin.distToSq(obj->getPosition());
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
                //TODO 完善碰撞
                m_aabbs.clear();

                for (SceneNodePtr child : (*pMapNode))
                {
                    m_aabbs.push_back(child->getWorldBoundingBox());
                }

                bool ret = doBuild();

                debugMessage(_T("TRACE: success:%d. octree has %u aabbs. maxDepth:%u, numNode:%u, numLeaf:%u, numBytes:%u"),
                             ret ? 1 : 0, m_aabbs.size(), numMaxDepth(), numNode(), numLeaf(), numBytes());

                return ret;
            }

        };
    }

    //////////////////////////////////////////////////////////////////////////
    ///
    //////////////////////////////////////////////////////////////////////////

    template<typename T>
    static bool updateIndices(TerrainMesh * hoder, int nVertices)
    {
        T *pIndex;
        if(!hoder->lockIB((void**)&pIndex)) return false;

        T vertices = (T)nVertices;
        T grid = vertices - 1;
        for (T r = 0; r < grid; ++r)
        {
            for (T c = 0; c < grid; ++c)
            {
                T n = r * vertices + c;
                *pIndex++ = n;
                *pIndex++ = n + vertices + 1;
                *pIndex++ = n + 1;

                *pIndex++ = n;
                *pIndex++ = n + vertices;
                *pIndex++ = n + vertices + 1;
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
    TerrainChunk::TerrainChunk()
        : m_id(0)
        , m_pMap(0)
        , m_isLoaded(false)
        , m_isLoading(false)
        , m_octreeDirty(true)
        , m_uvScale(1.0f)
        , m_gridSize(MapConfig::ChunkGridSize)
    {
        m_rect.zero();
    }

    TerrainChunk::~TerrainChunk(void)
    {
    }

    void TerrainChunk::init(TerrainMap *pMap, uint32 id, const FRect & rect)
    {
        m_pMap = pMap;
        m_id = id;
        m_rect = rect;

        setPosition(Vector3(m_rect.left, 0, m_rect.top));
    }

    bool TerrainChunk::load()
    {
        if (m_isLoaded || m_isLoading)
            return false;

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
        if (m_isLoaded)
            return;

        tstring chunkname;
        formatString(chunkname, _T("%8.8x.lzd"), m_id);
        chunkname = m_pMap->getMapName() + chunkname;

        LOG_DEBUG(_T("Load chunk(%d %d) path='%s'"),
            getRowID(), getColID(), chunkname.c_str());

        LZDataPtr root = openSection(chunkname);
        if (root)
        {
            // load terrain basic information.
            m_uvScale = root->readFloat(_T("uvscale"), 0.1f);

            tstring tempName = root->readString(_T("shader"), L"shader/terrain.fx");
            m_shader = EffectMgr::instance()->get(tempName);

            LZDataPtr textureDatas = root->read(_T("textures"));
            if (textureDatas)
            {
                tstring tagName;
                tstring textureName;
                int i = 0;
                for (; i < MapConfig::MaxNbChunkLayer; ++i)
                {
                    formatString(tagName, _T("layer%d"), i);
                    textureName = textureDatas->readString(tagName);
                    m_textures[i] = TextureMgr::instance()->get(textureName);
                }

                textureName = textureDatas->readString(_T("blend"));
                m_textures[i++] = TextureMgr::instance()->get(textureName);

                textureName = textureDatas->readString(_T("diffuse"));
                m_textures[i++] = TextureMgr::instance()->get(textureName);
            }
            else
            {
                m_textures[0] = TextureMgr::instance()->get(L"map/f.jpg");
                m_textures[MapConfig::MaxNbChunkLayer+1] = TextureMgr::instance()->get(L"map/diffuse.png");
            }

            // load terrain items
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

        updateVertices();
    }

    ///场景加载完毕
    void TerrainChunk::onLoadingFinish()
    {
        ZLockHolder lockHoder(&m_itemLocker);

        m_isLoaded = true;
        m_isLoading = false;
        m_pMap->onChunkLoaded(this);

        LOG_DEBUG(L"Chunk(%d %d) loaded.", getRowID(), getColID());
    }

    void TerrainChunk::loadItem(LZDataPtr ptr)
    {
        int type = ptr->readInt(L"type");
        
        SceneNodePtr item = SceneNodeFactory::create(type);;
        if (!item)
        {
            return;
        }

        if (!item->loadFromStream(ptr))
        {
            LOG_ERROR(L"Failed load Chunk Item. type %d", type);
            return;
        }

        addChild(item);
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

        HeightMapPtr heightmap = m_pMap->getHeightMap();

        //以下是创建网格数据
        const int nVertices = MapConfig::NbChunkVertex;
        TerrinVertex *p;
        for (int r = 0; r < nVertices; ++r)
        {
            for (int c = 0; c < nVertices; ++c)
            {
                int i = r * nVertices + c;//顶点索引

                p = pVertex + i;
                p->pos.x = m_rect.left + c * m_gridSize;
                p->pos.z = m_rect.top + r * m_gridSize;
                p->pos.y = heightmap->getHeight(p->pos.x, p->pos.z);

                p->nml = heightmap->getNormal(p->pos.x, p->pos.z);

                p->uv1.x = p->pos.x * m_uvScale;
                p->uv1.y = p->pos.z * m_uvScale;

                p->uv2.x = float(c) / nVertices;
                p->uv2.y = float(r) / nVertices;
            }
        }

        m_mesh.unlockVB();
        //updateNormal();
    }

    bool TerrainChunk::save()
    {
        if (!m_isLoaded)
        {
            LOG_ERROR(L"The chunk(%d, %d) does't loaded.", getRowID(), getColID());
            return false;
        }

        if (m_isLoading)
        {
            LOG_ERROR(L"The chunk(%d, %d) is loading now.", getRowID(), getColID());
            return false;
        }

        //save chunkdata.
        tstring chunkfile;
        formatString(chunkfile, _T("%8.8x.lzd"), m_id);
        chunkfile = m_pMap->getMapName() + chunkfile;

        LOG_DEBUG(_T("save node(%d %d) path='%s'"), getRowID(), getColID(), chunkfile.c_str());

        LZDataPtr root = openSection(chunkfile, true);
        root->clearChildren();

#if 1
        {//save time.juse for test
            tstring strTime;
            SYSTEMTIME sysTime;
            GetLocalTime(&sysTime);

            formatString(strTime, _T("[%d.%d.%d][%d.%d.%d][%d]"),
                sysTime.wYear, sysTime.wMonth, sysTime.wDay,
                sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
            root->writeString(_T("savetime"), strTime);
        }
#endif

        if (m_shader)
            root->writeString(_T("shader"), m_shader->source());

        {// save textures
            LZDataPtr textures = root->write(_T("textures"));
            int i = 0;
            tstring tagName;
            for (; i < MapConfig::MaxNbChunkLayer; ++i)
            {
                if (m_textures[i])
                {
                    formatString(tagName, _T("layer%d"), i);
                    textures->writeString(tagName, m_textures[i]->source());
                }
            }

            if (m_textures[i])
                textures->writeString(_T("blend"), m_textures[i]->source());

            ++i;
            if (m_textures[i])
                textures->writeString(_T("diffuse"), m_textures[i]->source());
        }

        {// save items
            LZDataPtr items = root->write(_T("items"));
            items->clearChildren();

            for (SceneNodePtr child : m_children)
            {
                LZDataPtr itemData = items->newOne(L"item", L"");
                child->saveToStream(itemData);
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

    void TerrainChunk::render(IDirect3DDevice9* pDevice)
    {
        if (!m_isLoaded)
            return;

        rcDevice()->pushWorld(matIdentity);
        renderTerrain(pDevice);
        rcDevice()->popWorld();

        SceneNode::render(pDevice);

        //调试渲染当前场景的八叉树
        if (MapConfig::ShowChunkOctree && TerrainMap::instance()->getFocusChunk() == this)
        {
            RSHolder rsHolder1(pDevice, D3DRS_LIGHTING, FALSE);

            pDevice->SetTransform(D3DTS_WORLD, &matIdentity);
            if (m_octree) m_octree->render(pDevice);
        }
    }

    void TerrainChunk::renderTerrain(IDirect3DDevice9* pDevice)
    {
        RSHolder holder(pDevice, D3DRS_CULLMODE, D3DCULL_CCW);

        D3DMATERIAL9 material;
        material.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
        material.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
        material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
        material.Specular = material.Diffuse;
        material.Power = 80.f;
        rcDevice()->setMaterial(material);

        if (!m_mesh.valid() || !m_shader) return;

        int maxTexIndex = -1;
        int i = 0;
        char buffer[64];
        for (; i < MapConfig::MaxNbChunkLayer; ++i)
        {
            sprintf_s(buffer, 64, "g_texture%d", i);
            if (m_textures[i])
            {
                m_shader->setTexture(buffer, m_textures[i]->getTexture());
                maxTexIndex = i;
            }
            else m_shader->setTexture(buffer, NULL);
        }

        if (maxTexIndex < 0) return;

        if (m_textures[i])
            m_shader->setTexture("g_textureBlend", m_textures[i]->getTexture());
        else
            m_shader->setTexture("g_textureBlend", NULL);

        ++i;
        if (m_textures[i])
            m_shader->setTexture("g_textureDiffuse", m_textures[i]->getTexture());
        else
            m_shader->setTexture("g_textureDiffuse", NULL);

        sprintf_s(buffer, 64, "tech_%d", maxTexIndex);
        if (!m_shader->setTechnique(buffer))
            return;

        uint32 nPass;
        if (m_shader->begin(nPass))
        {
            for (uint32 i = 0; i < nPass; ++i)
            {
                m_shader->beginPass(i);
                m_mesh.render(pDevice);
                m_shader->endPass();
            }
            m_shader->end();
        }
    }

    void TerrainChunk::update(float elapse)
    {
        if (!m_isLoaded)
            return;

        if (m_pMap->ifChunkOutside(this))
        {
            unload();
            return;
        }

        SceneNode::update(elapse);

        if (m_octreeDirty)
            buildOctree();
    }

    void TerrainChunk::unload(void)
    {
        if (!m_isLoaded)
            return;

        m_pMap->onChunkUnloaded(this);

        m_isLoaded = false;
        m_isLoading = false;

        VIBCache::instance()->release(m_mesh);
        clearChildren();

        LOG_DEBUG(L"Chunk(%d %d) unloaded.", getRowID(), getColID());
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
    SceneNodePtr TerrainChunk::pickItem(const Vector3 & origin, const Vector3 & dir)
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

} // end namespace Lazy
