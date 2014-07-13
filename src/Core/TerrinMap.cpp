
#include "stdafx.h"
#include "Pick.h"
#include "Entity.h"

#include "../Physics/Physics.h"
#include "Material.h"

#include "TerrinMap.h"

#include "../Render/Texture.h"

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
                ChunkPtr chunk = TerrainMap::instance()->getNode(x);
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
        TerrainItemPtr m_obj;
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

        TerrainItemPtr getObj() const
        {
            return m_obj;
        }

        ///是否与四叉树叶结点碰撞
        virtual bool doQuery(QuadTreeBase *, const IndicesArray & indices) override
        {
            for (const auto & x : indices)
            {
                ChunkPtr chunk = TerrainMap::instance()->getNode(x);
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
    {
        m_nodeSize = 0;         //< 每个格子的尺寸。
        m_nodeR = 0;        //< 结点行数
        m_nodeC = 0;        //< 结点列数

        m_showLevel = 1;
        m_usefull = false;

        m_pSource = NULL;
        m_pActiveObj = NULL;
        m_pSelectObj = NULL;
        m_loadingProgress = 0.0f;

        m_showRadius = 5000.0f;
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

        m_mapNodes.clear();
        m_renderNodes.clear();

        m_mapName.clear();

        m_pActiveObj = NULL;
        m_pSelectObj = NULL;
    }

    void TerrainMap::createMap(int rows, int cols)
    {
        m_nodeR = rows;
        m_nodeC = cols;
        m_nodeSize = MapConfig::ChunkSize;

        float w = m_nodeC * m_nodeSize;
        float h = m_nodeR * m_nodeSize;
        m_rect.set(-w * 0.5f, -h * 0.5f, w * 0.5f, h * 0.5f);

        LOG_DEBUG(_T("CreateMap rows=%d, cols=%d, node size=%f"),
            m_nodeR, m_nodeC, m_nodeSize);

        initChunks();
    }

    /** 根据地图文件名，加载地图。地图文件包含地图的高度图，行列等信息。*/
    bool TerrainMap::loadMap(const tstring & path)
    {
        if (m_mapName == path) return false;

        LOG_INFO(L"TerrainMap::loadMap %s", path.c_str());
        clearCurMap();

        m_mapName = path;
        formatDirName(m_mapName);

        tstring filename = m_mapName + L"map.lzd";
        LZDataPtr ptr = openSection(filename);
        if (!ptr)
        {
            LOG_ERROR(L"Load map config '%s' failed!", m_mapName.c_str());
            return false;
        }

        m_objOnGround = ptr->readBool(L"onGround", true);
        m_textureName = ptr->readString(L"texture");

        createMap(ptr->readInt(L"rows"), ptr->readInt(L"cols"));

        //加载路点
        tstring wpPath = ptr->readString(L"searchData");
        if (!WayChunkMgr::instance()->load(wpPath))
        {
            LOG_ERROR(L"Load way point failed!");
        }

        m_loadingProgress = 1.0f;
        m_usefull = true;

        LOG_INFO(L"Load map finished.");
        return true;
    }

    void TerrainMap::saveMap(const tstring & path)
    {
        m_mapName = path;
        if (MapConfig::UseMultiThread && !isAllChunkLoaded())
        {
            LOG_ERROR(L"In muti thread mode, you must load all the chunk first!");
            return;
        }

        for (ChunkPtr chunk : m_mapNodes)
        {
            chunk->load();
            chunk->save();
        }
    }

    void TerrainMap::addTerrainItem(TerrainItemPtr item)
    {
        AABB aabb;
        item->getWorldAABB(aabb);

        MapRectCollider collider(false);
        collider.rect.fromAABBXZ(aabb);
        m_quadTree.queryRect(&collider);

        for (size_t i : collider.m_chunkIds)
        {
            m_mapNodes[i]->addItem(item);
        }
    }

    void TerrainMap::delTerrainItem(TerrainItemPtr item)
    {
        item->removeFromChunks();
    }

    float TerrainMap::getHeight(float x, float z) const
    {
        int row = int(z / MapConfig::ChunkSize);
        int col = int(x / MapConfig::ChunkGridSize);
        return m_mapNodes[row * m_nodeC + col]->getPhysicalHeight(x, z);
    }

    void TerrainMap::loadAllChunks()
    {
        for (ChunkPtr chunk : m_mapNodes)
        {
            chunk->load();
        }
    }

    bool TerrainMap::isAllChunkLoaded()
    {
        for (ChunkPtr chunk : m_mapNodes)
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

        float totalNode = float(m_nodeR * m_nodeC);

        //将地图分成m_nodeR*m_nodeC块
        for (int r = 0; r < m_nodeR; ++r)
        {
            for (int c = 0; c < m_nodeC; ++c)
            {
                m_loadingProgress = (r * m_nodeC + c) / totalNode * 0.5f;

                FRect rc;
                rc.left = c * m_nodeSize - m_rect.left;
                rc.top = r * m_nodeSize - m_rect.top;
                rc.right = rc.left + m_nodeSize;
                rc.bottom = rc.top + m_nodeSize;
                m_mapNodes.push_back(new TerrainChunk(this, r << 16 || c, rc));
            }
        }

        //构造四叉树
        LOG_INFO(L"Build quad tree.");

        qtree::Config = { 14, 1, 1000.0f, true };
        RectArray rects;
        for (auto it = m_mapNodes.begin(); it != m_mapNodes.end(); ++it)
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
        if (!m_usefull) return;

        if (!MapConfig::ShowTerrain) return;

        pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
        pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

        pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);

        pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

        CMaterial::setMaterial(pDevice,
            D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
            D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f),
            D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f),
            D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f),
            1.0f);

        pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

        dx::Texture *pTex = TextureMgr::instance()->getTexture(m_textureName);
        pDevice->SetTexture(0, pTex);
        for (size_t i = 0; i < m_renderNodes.size(); ++i)
        {
            m_renderNodes[i]->renderTerrain(pDevice);
        }
        pDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

        pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

        //绘制地图物体
        for (size_t i = 0; i < m_renderNodes.size(); ++i)
        {
            m_renderNodes[i]->renderItems(pDevice);
        }

#if 0

        Matrix matWorld;
        MatrixIdentity(&matWorld);
        pDevice->SetTransform(D3DTS_WORLD, &matWorld);
        m_quadTree.render(pDevice);

#endif
    }

    void TerrainMap::update(float elapse)
    {
        if (!m_usefull)  return;

        genVisibleChunks();

        for (ChunkPtr chunk : m_renderNodes)
        {
            chunk->update(elapse);
        }
    }

    ChunkPtr TerrainMap::currentNode()
    {
        if (m_pSource)
        {
            const Vector3 & pos = m_pSource->getPos();
            if (!isInBound(pos.x, pos.z))
            {
                return NULL;
            }
            return getNode(pos.x, pos.z);
        }
        return NULL;
    }

    ChunkPtr TerrainMap::getChunkByID(uint32 id)
    {
        uint32 r = (id >> 16) & 0xffff;
        uint32 c = id & 0xffff;
        return m_mapNodes[r * m_nodeC + c];
    }

    /** 根据索引获得地图结点。*/
    TerrainChunk* TerrainMap::getNode(int r, int c)
    {
        assert(r >= 0 && r < m_nodeR && c >= 0 && c < m_nodeC && "TerrainMap::getNode: index out of range!");

        int index = r * m_nodeC + c;
        return m_mapNodes[index].get();
    }

    /** 根据坐标取得地图结点。*/
    TerrainChunk* TerrainMap::getNode(float x, float z)
    {
        int c = int(x / m_nodeSize);
        int r = int(z / m_nodeSize);
        return getNode(r, c);
    }

    /** 生成邻接的8个结点。*/
    void TerrainMap::genVisibleChunks(void)
    {
        Vector3 pos(0, 0, 0);
        if (m_pSource)
        {
            pos = m_pSource->getPos();
            if (!isInBound(pos.x, pos.z))
            {
                return;
            }
        }
        m_renderNodes.clear();

        if (m_showRadius < 0) m_showRadius = 10.0f;

        MapRectCollider collider(false);
        collider.rect.left = pos.x - m_showRadius;
        collider.rect.top = pos.z - m_showRadius;
        collider.rect.right = pos.x + m_showRadius;
        collider.rect.bottom = pos.z + m_showRadius;
        m_quadTree.queryRect(&collider);

        if (collider.empty()) return;
        for (size_t i : collider.m_chunkIds)
        {
            m_renderNodes.push_back(m_mapNodes[i]);
        }
    }


    bool TerrainMap::intersect(Vector3 & position)
    {
        Pick *pick = getPick();
        if (NULL == pick) return false;

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

    void TerrainMap::handeMouseEvent(UINT msg, WPARAM, LPARAM)
    {
        if (!m_usefull) return;

        if (!Pick::instance()->isTerrainObjEnable())
        {
            return;
        }

        if (getActiveObj())
        {
            if (msg == WM_LBUTTONDOWN)
            {
                getActiveObj()->focusCursor(CM_LDOWN);
            }
            else if (msg == WM_LBUTTONUP)
            {
                getActiveObj()->focusCursor(CM_LUP);
            }
            else if (msg == WM_RBUTTONDOWN)
            {
                getActiveObj()->focusCursor(CM_RDOWN);
            }
            else if (msg == WM_RBUTTONUP)
            {
                getActiveObj()->focusCursor(CM_RUP);
                m_pSelectObj = getActiveObj();
            }
        }
    }

    bool TerrainMap::intersectWithCursor()
    {
        TerrainItemPtr pActiveObj = NULL;

        //八叉树拾取
        MapRayAndObjCollider collider(Pick::instance()->rayPos(), Pick::instance()->rayDir());
        m_quadTree.pick(&collider);

        pActiveObj = collider.getObj();

        setActiveObj(pActiveObj);
        return pActiveObj != NULL;
    }

    void TerrainMap::setActiveObj(TerrainItemPtr pObj)
    {
        if (m_pActiveObj)
        {
            m_pActiveObj->focusCursor(CM_LEAVE);
        }
        m_pActiveObj = pObj;
        if (m_pActiveObj)
        {
            m_pActiveObj->focusCursor(CM_ENTER);
        }
    }


    bool TerrainMap::isInBound(float x, float z)
    {
        return m_rect.isIn(x, z);
    }

    void TerrainMap::setShowLevel(int level)
    {
        m_showLevel = level;
    }

    float TerrainMap::xMin() const
    {
        return m_rect.left;
    }

    float TerrainMap::xMax() const
    {
        return m_rect.right;
    }

    float TerrainMap::zMin() const
    {
        return m_rect.top;
    }

    float TerrainMap::zMax() const
    {
        return m_rect.bottom;
    }

    float TerrainMap::width() const
    {
        return m_rect.width();
    }

    float TerrainMap::height() const
    {
        return m_rect.height();
    }

#if USE_NEW_CHUNK_STYLE

    bool TerrainMap::createMap(float width, float height, const tstring & mapName)
    {
        int rows = int(std::ceil(width / MapConfig::ChunkSize));
        int cols = int(std::ceil(height / MapConfig::ChunkSize));

        return createMap(mapName, rows, cols);
    }

    bool TerrainMap::createMap(const tstring & mapName, int rows, int cols)
    {
        if (rows <= 0) rows = 1;
        if (cols <= 0) cols = 1;

        if (rows * cols > MapConfig::MaxChunks)
        {
            LOG_ERROR(L"Map size is too large! rows=%d, cols=%d", rows, cols);
            return false;
        }

        release();

        m_mapName = mapName;


        //生成一个长方形的地图

        float halfWidth = rows * MapConfig::ChunkSize * 0.5f;
        float halfHeight = cols * MapConfig::ChunkSize * 0.5f;

        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                int id = m_idAllocator++;

                float x = c * MapConfig::ChunkSize - halfWidth;
                float y = halfHeight - r * MapConfig::ChunkSize;

                ChunkPtr chunk = new TerrainChunk();
                chunk->create(id, x, y);

                m_chunks[id] = chunk;
            }
        }

        //生成4叉树



        return true;
    }



#endif

} // end namespace Lazy