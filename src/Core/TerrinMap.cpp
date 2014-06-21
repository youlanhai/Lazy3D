
#include "stdafx.h"
#include "Pick.h"
#include "Res.h"
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

    const int direct8[][2] = {{ -1, -1}, {1, -1}, {1, 1}, { -1, 1}, {0, -1}, { -1, 0}, {0, 1}, {1, 0}};

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

                m_obj = chunk->pickObj(v3Origin, v3Dir);

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
    /*static*/ TerrainMap * TerrainMap::instance()
    {
        static TerrainMap s_map;
        return &s_map;
    }

    bool TerrainMap::init()
    {
        LOG_INFO(L"TerrainMap::init.");

        return true;
    }

    void TerrainMap::fini()
    {
        clearCurMap();
        VIBCache::instance()->clear();

        m_pTData = NULL;

        LOG_INFO(L"TerrainMap::fini.");
    }


    TerrainMap::TerrainMap()
    {
        m_pTData = new TerrinData();

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
        m_idAllocator = 10000;
    }

    TerrainMap::~TerrainMap(void)
    {
    }


    void TerrainMap::clearCurMap(void)
    {
        m_usefull = false;
        m_loadingProgress = 0.0f;

        m_chunks.clear();
        m_mapNodes.clear();
        m_renderNodes.clear();

        m_mapName.clear();

        m_pActiveObj = NULL;
        m_pSelectObj = NULL;
    }


    /** 根据地图文件名，加载地图。地图文件包含地图的高度图，行列等信息。*/
    bool TerrainMap::loadMap(const tstring & mapName)
    {
        if (m_mapName == mapName) return false;

        LOG_INFO(L"TerrainMap::loadMap %s", mapName.c_str());
        clearCurMap();

        m_mapName = mapName;

        LZDataPtr ptr = openSection(mapName);
        if (!ptr)
        {
            LOG_ERROR(L"Load map config '%s' failed!", mapName.c_str());
            return false;
        }

        if (!m_pTData->loadHeightMap(
                    ptr->readString(L"mapName"),
                    ptr->readInt(L"vrows"),
                    ptr->readInt(L"vcols"),
                    ptr->readFloat(L"squareSize"),
                    ptr->readFloat(L"heightScale"),
                    ptr->readBool(L"useOneTex")))
        {
            LOG_ERROR(L"ERROR: load the height map failed!");
            return false;
        }


        m_objOnGround = ptr->readBool(L"onGround", true);
        m_nodeSize = m_pTData->squareSize() * TerrainConfig::MaxMapNodeGrid;
        m_nodeR = (int)ceil(m_pTData->height() / (float)m_nodeSize);
        m_nodeC = (int)ceil(m_pTData->width() / (float)m_nodeSize);

        debugMessage(_T("INFO: map rows=%d, cols=%d, node size=%f"),
                     m_nodeR, m_nodeC, m_nodeSize);

        m_textureName = ptr->readString(L"texture");

        initChunks();

        //加载路点
        tstring wpPath = ptr->readString(L"searchData");
        if(!WayChunkMgr::instance()->load(wpPath))
        {
            LOG_ERROR(L"Load way point failed!");
        }

        m_loadingProgress = 1.0f;
        m_usefull = true;

        LOG_INFO(L"Load map finished.");
        return true;
    }

    void TerrainMap::saveMap(const tstring &)
    {
        doDataConvert();
    }

//数据类型转换
    void TerrainMap::doDataConvert()
    {
        if (MapConfig::UseMultiThread)
        {
            LOG_ERROR(L"Only single thread mode can do save!");
            return;
        }

        for(ChunkPtr chunk : m_mapNodes)
        {
            chunk->load();
            chunk->save();
        }
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
            if (!chunk->isLoaded()) return false;
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
                m_mapNodes.push_back(new TerrainChunk(m_pTData.get(), r, c));
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


    void TerrainMap::processExternalItem(TerrainItemPtr item)
    {
        AABB aabb;
        item->getWorldAABB(aabb);

        MapRectCollider collider(false);
        collider.rect.fromAABBXZ(aabb);
        m_quadTree.queryRect(&collider);

        for (size_t i : collider.m_chunkIds)
        {
            m_mapNodes[i]->addExternal(item);
        }
    }

    void TerrainMap::render(IDirect3DDevice9* pDevice)
    {
        if (!m_usefull) return ;

        if (!MapConfig::ShowTerrain) return;

        pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
        pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE );
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
            m_renderNodes[i]->render(pDevice);
        }
        pDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

        pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
        pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

        //绘制地图物体
        for (size_t i = 0; i < m_renderNodes.size(); ++i)
        {
            m_renderNodes[i]->renderObj(pDevice);
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
        if (!m_usefull)  return ;

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
        Square2 sq = getNodeIndex(x, z);
        return getNode(sq.r, sq.c);
    }

    /** 根据真实位置得到子结点索引。x:为列，y:为行。*/
    Square2 TerrainMap::getNodeIndex(float x, float z)
    {
        x += m_pTData->width() / 2.0f;
        z = m_pTData->height() / 2.0f - z;
        return Square2(int(z / m_nodeSize), int(x / m_nodeSize));
    }


    Square2 TerrainMap::getSquareIndex(float x, float z)
    {
        x += m_pTData->width() / 2.0f;
        z = m_pTData->height() / 2.0f - z;
        return Square2(int(z / m_pTData->squareSize()), int(x / m_pTData->squareSize()));
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

//////////////////////////////////////////////////////////////////////////
    void TerrainMap::handeMouseEvent(UINT msg, WPARAM, LPARAM )
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


//////////////////////////////////////////////////////////////////////////
    bool TerrainMap::isInBound(float x, float z)
    {
        return m_pTData->isInBound(x, z);
    }

    float TerrainMap::getHeight(float x, float z)
    {
        return m_pTData->getPhysicalHeight(x, z);
    }

    void TerrainMap::setShowLevel(int level)
    {
        m_showLevel = level;
    }


    float TerrainMap::xMin()
    {
        return -width() / 2.0f;
    }

    float TerrainMap::xMax()
    {
        return width() / 2.0f;
    }

    float TerrainMap::zMin()
    {
        return -height() / 2.0f;
    }

    float TerrainMap::zMax()
    {
        return height() / 2.0f;
    }

    float TerrainMap::width()
    {
        return (float)m_pTData->width();
    }

    float TerrainMap::height()
    {
        return (float)m_pTData->height();
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