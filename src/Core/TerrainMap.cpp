
#include "stdafx.h"
#include "Pick.h"
#include "Entity.h"

#include "../Physics/Physics.h"
#include "Material.h"

#include "TerrainMap.h"

#include "../Render/Texture.h"
#include "../Render/RenderDevice.h"
#include "../Render/ShadowMap.h"

namespace Lazy
{

    /**
     *  使用四叉树管理子地图（chunk）。可以很方便查找出位于矩形范围内的chunk。
     *  渲染
     *      以玩家位置为中心，其可见区域构成一个矩形，矩形覆盖到的chunk都需要渲染出来。
     *  碰撞检测
     *      玩家碰撞体构成的aabb所覆盖的chunk。
     *  射线拾取
     */

    const int direct8 [][2] = {
        { -1, -1 }, { 1, -1 }, { 1, 1 },
        { -1, 1 }, { 0, -1 }, 
        { -1, 0 }, { 0, 1 }, { 1, 0 }
    };

    ///地图射线拾取碰撞点
    class MapRayCollider : public RayCollider
    {
        Vector3 v3Origin;
        Vector3 v3Dir;
        float distance;
    public:

        MapRayCollider(const Vector3 & o, const Vector3 & d)
            : v3Origin(o)
            , v3Dir(d)
            , distance(MAX_FLOAT)
        {
            origin.set(o.x, o.z);
            dir.set(d.x, d.z);
        }

        ~MapRayCollider()
        {

        }

        float getCollidDist() const
        {
            return distance;
        }

        ///是否与四叉树叶结点碰撞
        virtual bool doQuery(QuadTreeBase *, const IndicesArray & indices) override
        {
            bool collid = false;
            float dist;
            for (const auto & x : indices)
            {
                ChunkPtr chunk = TerrainMap::instance()->getChunkByIndex(x);
                if (!chunk) continue;

                if (chunk->intersect(v3Origin, v3Dir, dist))
                {
                    distance = min(dist, distance);
                    collid = true;
                }
            }
            return collid;
        }
    };

    ///射线拾取场景物件
    class MapRayAndObjCollider : public RayCollider
    {
        SceneNodePtr m_obj;
        Vector3 v3Origin;
        Vector3 v3Dir;
    public:

        MapRayAndObjCollider(const Vector3 & o, const Vector3 & d)
            : v3Origin(o)
            , v3Dir(d)
        {
            origin.set(o.x, o.z);
            dir.set(d.x, d.z);
        }

        ~MapRayAndObjCollider()
        {

        }

        SceneNodePtr getObj() const
        {
            return m_obj;
        }

        ///是否与四叉树叶结点碰撞
        virtual bool doQuery(QuadTreeBase *, const IndicesArray & indices) override
        {
            for (const auto & x : indices)
            {
                ChunkPtr chunk = TerrainMap::instance()->getChunkByIndex(x);
                if (!chunk) continue;

                m_obj = chunk->pickItem(v3Origin, v3Dir);

                if (m_obj) return true;
            }
            return false;
        }
    };

    ///区域查询。查询rect所覆盖的区域。
    class MapRectCollider : public RectCollider
    {
        bool m_querayFirst;
    public:

        MapRectCollider(bool querayFirst)
            : m_querayFirst(querayFirst)
        {
        }

        ///是否与四叉树叶结点碰撞
        virtual bool doQuery(QuadTreeBase *, const IndicesArray & indices) override
        {
            m_chunkIds.insert(m_chunkIds.end(), indices.begin(), indices.end());

            return m_querayFirst; //返回false，可以查询到所有相交的区域。
        }

        bool empty() const { return m_chunkIds.empty(); }

        IndicesArray m_chunkIds;
    };


    //////////////////////////////////////////////////////////////////////////
    /// TerrainMap
    //////////////////////////////////////////////////////////////////////////
    IMPLEMENT_SINGLETON(TerrainMap);

    TerrainMap::TerrainMap()
        : m_root(new SceneNode())
        , m_loadAll(FALSE)
    {
        m_chunkSize = 0;         //< 每个格子的尺寸。
        m_chunkRows = 0;        //< 结点行数
        m_chunkCols = 0;        //< 结点列数

        m_usefull = FALSE;

        m_pSource = NULL;
        m_loadingProgress = 0.0f;

        m_showRadius = 50.0f;
        m_rect.zero();
    }

    TerrainMap::~TerrainMap(void)
    {
        clearCurMap();
        VIBCache::instance()->clear();
    }

    void TerrainMap::clearCurMap(void)
    {
        m_usefull = false;
        m_loadingProgress = 0.0f;

        m_chunks.clear();
        m_visibleChunks.clear();

        m_mapName.clear();

        m_rect.zero();

        m_root->clearChildren();
    }

    bool TerrainMap::createMap(const tstring & nameName, int rows, int cols)
    {
        tstring filename = nameName;
        formatDirName(filename);
        filename += L"map.lzd";

        LZDataPtr root = openSection(filename, true, DataType::Lzd);
        if (!root)
        {
            LOG_ERROR(L"Can't openSection '%s'", filename.c_str());
            return false;
        }

        root->writeInt(L"rows", rows);
        root->writeInt(L"cols", cols);

        if (!saveSection(root, filename))
        {
            LOG_ERROR(L"Can't saveSection '%s'", filename.c_str());
            return false;
        }

        return loadMap(nameName);
    }

    /** 根据地图文件名，加载地图。地图文件包含地图的高度图，行列等信息。*/
    bool TerrainMap::loadMap(const tstring & path)
    {
        if (m_mapName == path) return false;

        LOG_INFO(L"TerrainMap::loadMap %s", path.c_str());
        clearCurMap();

        m_mapName = path;
        formatDirName(m_mapName);

        // loa map config file
        tstring filename = m_mapName + L"map.lzd";
        LZDataPtr ptr = openSection(filename);
        if (!ptr)
        {
            LOG_ERROR(L"Load map config '%s' failed!", m_mapName.c_str());
            return false;
        }

        // load height map
        tstring heightmap = ptr->readString(L"heightMap");
        m_heightmap = new HeightMap();
        if (heightmap.empty() || !m_heightmap->load(heightmap))
        {
            LOG_ERROR(L"Failed load map '%s', the height map load failed!", m_mapName.c_str());
            return false;
        }

        m_chunkRows = ptr->readInt(L"rows");
        m_chunkCols = ptr->readInt(L"cols");
        if (m_chunkRows < 0 || m_chunkCols < 0)
        {
            LOG_ERROR(L"Load map, Invalid size.");
            return false;
        }

        m_chunkSize = MapConfig::ChunkSize;
        float w = m_chunkCols * m_chunkSize;
        float h = m_chunkRows * m_chunkSize;
        m_rect.set(-w * 0.5f, -h * 0.5f, w * 0.5f, h * 0.5f);

        m_objOnGround = ptr->readBool(L"onGround", true);

        // loa chunks
        initChunks();

        //加载路点
        tstring wpPath = ptr->readString(L"searchData");
        if (!WayChunkMgr::instance()->load(wpPath))
        {
            LOG_ERROR(L"Load way point failed!");
        }

        LOG_INFO(L"Load map finished.");

        m_loadingProgress = 1.0f;
        m_usefull = true;

        m_root->onEnterWorld();
        return true;
    }

    bool TerrainMap::saveMap(const tstring & path)
    {
        if (MapConfig::UseMultiThread && !isAllChunkLoaded())
        {
            LOG_ERROR(L"In muti thread mode, you must load all the chunk first!");
            return false;
        }

        if (!path.empty())
        {
            m_mapName = path;
            formatDirName(m_mapName);
        }

        if (m_mapName.empty())
        {
            LOG_ERROR(L"Need mapPath.");
            return false;
        }

        tstring filename = m_mapName + L"map.lzd";
        LZDataPtr root = openSection(filename, true);
        if (!root)
        {
            LOG_ERROR(L"Load map config '%s' failed!", m_mapName.c_str());
            return false;
        }

        if (m_heightmap)
            root->writeString(L"heightMap", m_heightmap->getResource());

        root->writeInt(L"rows", m_chunkRows);
        root->writeInt(L"cols", m_chunkCols);
        root->writeBool(L"onGround", m_objOnGround != FALSE);

        if (!saveSection(root, filename))
        {
            LOG_ERROR(L"Save map config '%s' failed.", filename.c_str());
            return false;
        }

        for (ChunkPtr chunk : m_chunks)
        {
            chunk->load();
            chunk->save();
        }

        return true;
    }

    int TerrainMap::position2chunk(float x, float z) const
    {
        int row = int((z - m_rect.top) / MapConfig::ChunkSize);
        int col = int((x - m_rect.left) / MapConfig::ChunkSize);
        return row * m_chunkCols + col;
    }

    float TerrainMap::getHeight(float x, float z) const
    {
        return m_heightmap ? m_heightmap->getHeight(x, z) : 0.0f;
    }

    void TerrainMap::loadAllChunks(bool load)
    {
        m_loadAll = load;

        if (m_loadAll)
        {
            for (ChunkPtr chunk : m_chunks)
            {
                chunk->load();
            }
        }
    }

    bool TerrainMap::isAllChunkLoaded()
    {
        for (ChunkPtr chunk : m_chunks)
        {
            if (!chunk->ifLoaded()) return false;
        }

        return true;
    }

    void TerrainMap::initChunks(void)
    {
        LOG_INFO(L"TerrainMap::initChunks...");

        //预先构造几个顶点缓存
        LOG_INFO(L"Preload vib cache...");
        for (size_t i = VIBCache::instance()->size(); i < 8; ++i)
        {
            VIBCache::instance()->preLoadOne();
        }

        float totalNode = float(m_chunkRows * m_chunkCols);

        //将地图分成m_chunkRows*m_chunkCols块
        for (int r = 0; r < m_chunkRows; ++r)
        {
            for (int c = 0; c < m_chunkCols; ++c)
            {
                m_loadingProgress = (r * m_chunkCols + c) / totalNode * 0.5f;

                FRect rc;
                rc.left = c * m_chunkSize + m_rect.left;
                rc.top = r * m_chunkSize + m_rect.top;
                rc.right = rc.left + m_chunkSize;
                rc.bottom = rc.top + m_chunkSize;

                TerrainChunk *p = new TerrainChunk();
                p->init(this, r << 16 | c, rc);
                m_chunks.push_back(p);
            }
        }

        //构造四叉树
        LOG_INFO(L"Build quad tree.");

        qtree::Config = { 14, 1, m_chunkSize, true };
        RectArray rects;
        for (auto it = m_chunks.begin(); it != m_chunks.end(); ++it)
        {
            rects.push_back((*it)->getRect());
        }
        m_quadTree.build(rects);

#if 0
        //将四叉树数据保存到文件中
        m_quadTree.save(_T("quadTree.lzd"), LZType::lzd);
        m_quadTree.save(_T("quadTree.xml"), LZType::xml);
#endif

    }

    void TerrainMap::render(IDirect3DDevice9* pDevice)
    {
        if (!m_usefull || !MapConfig::ShowTerrain || !m_root)
            return;

#if 0
        if (ShadowMap::instance()->begin())
        {
            rcDevice()->clear(D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER,
                D3DXCOLOR(0, 0, 0, 1), 1, 0);

            m_root->render(pDevice);
            ShadowMap::instance()->end();

            static bool isFirst = true;
            if (isFirst)
            {
                isFirst = false;
                D3DXSaveTextureToFile(L"test.bmp", D3DXIFF_BMP,
                    ShadowMap::instance()->getTexture(), NULL);
            }
        }
#else
        m_root->render(pDevice);
#endif

#if 0

        Matrix matWorld;
        MatrixIdentity(&matWorld);
        pDevice->SetTransform(D3DTS_WORLD, &matWorld);
        m_quadTree.render(pDevice);

#endif
    }

    void TerrainMap::update(float elapse)
    {
        if (!m_usefull) 
            return;

        genVisibleChunks();

        m_root->update(elapse);
    }

    ChunkPtr TerrainMap::getFocusChunk()
    {
        if (m_pSource)
        {
            const Vector3 & pos = m_pSource->getPosition();
            if (!isInBound(pos.x, pos.z))
            {
                return NULL;
            }
            return getChunkByPos(pos.x, pos.z);
        }
        return NULL;
    }

    ChunkPtr TerrainMap::getChunkByID(uint32 id) const
    {
        uint32 r = (id >> 16) & 0xffff;
        uint32 c = id & 0xffff;
        return m_chunks[r * m_chunkCols + c];
    }

    ChunkPtr TerrainMap::getChunkByIndex(size_t index) const
    {
        return m_chunks[index]; 
    }

    /** 根据坐标取得地图结点。*/
    ChunkPtr TerrainMap::getChunkByPos(float x, float z) const
    {
        size_t index = position2chunk(x, z);
        if (index < m_chunks.size())
            return NULL;

        return m_chunks[index];
    }

    /** 生成邻接的8个结点。*/
    void TerrainMap::genVisibleChunks(void)
    {
        Vector3 pos(0, 0, 0);
        if (m_pSource)
        {
            pos = m_pSource->getPosition();
            if (!isInBound(pos.x, pos.z))
            {
                return;
            }
        }
        m_visibleChunks.clear();

        if (m_showRadius < 0)
            m_showRadius = 10.0f;

        MapRectCollider collider(false);
        collider.rect.left = pos.x - m_showRadius;
        collider.rect.top = pos.z - m_showRadius;
        collider.rect.right = pos.x + m_showRadius;
        collider.rect.bottom = pos.z + m_showRadius;
        m_quadTree.queryRect(&collider);

        if (collider.empty()) return;

        for (size_t i : collider.m_chunkIds)
        {
            m_chunks[i]->load();
            m_visibleChunks.push_back(m_chunks[i]);
        }
    }


    bool TerrainMap::intersect(Vector3 & position)
    {
        Pick *pick = Pick::instance();

        MapRayCollider collider(pick->rayPos(), pick->rayDir());
        if (m_quadTree.pick(&collider))
        {
            position = pick->rayDir();
            position *= collider.getCollidDist();
            position += pick->rayPos();

            //debugMessage(_T("TRACE: click (%f, %f, %f)"), position.x, position.y, position.z);
            return true;
        }

        return false;
    }

    bool TerrainMap::handeEvent(const SEvent & /*event*/)
    {
        return false;
    }

    bool TerrainMap::intersectWithCursor()
    {
        SceneNodePtr pActiveObj;

        //八叉树拾取
        MapRayAndObjCollider collider(Pick::instance()->rayPos(), Pick::instance()->rayDir());
        m_quadTree.pick(&collider);

        pActiveObj = collider.getObj();

        return pActiveObj != NULL;
    }

    bool TerrainMap::isInBound(float x, float z)
    {
        return m_rect.isIn(x, z);
    }

    void TerrainMap::onChunkLoaded(TerrainChunk* chunk)
    {
        m_root->addChild(chunk);
    }

    void TerrainMap::onChunkUnloaded(TerrainChunk* chunk)
    {
        m_root->delChild(chunk);
    }

    bool TerrainMap::ifChunkOutside(TerrainChunk* chunk)
    {
        if (m_loadAll)
            return false;

        if (!m_pSource)
            return true;

        const Vector3 & center = m_pSource->getPosition();
        FRect rc = chunk->getRect();

        if (rc.isIn(center.x, center.z))
            return false;

        float VisibleDistance = m_showRadius + 10.0f;

        if (abs(rc.left - center.x) < VisibleDistance ||
            abs(rc.right - center.x) < VisibleDistance ||
            abs(rc.top - center.y) < VisibleDistance ||
            abs(rc.bottom - center.y) < VisibleDistance
            )
            return false;

        return true;
    }

} // end namespace Lazy
