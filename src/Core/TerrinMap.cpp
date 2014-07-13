
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

        m_rect.zero();
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

        tstring filename = m_mapName + L"map.lzd";
        LZDataPtr ptr = openSection(filename);
        if (!ptr)
        {
            LOG_ERROR(L"Load map config '%s' failed!", m_mapName.c_str());
            return false;
        }

        m_nodeR = ptr->readInt(L"rows");
        m_nodeC = ptr->readInt(L"cols");
        if (m_nodeR < 0 || m_nodeC < 0)
        {
            LOG_ERROR(L"Load map, Invalid size.");
            return false;
        }

        m_nodeSize = MapConfig::ChunkSize;
        float w = m_nodeC * m_nodeSize;
        float h = m_nodeR * m_nodeSize;
        m_rect.set(-w * 0.5f, -h * 0.5f, w * 0.5f, h * 0.5f);

        m_objOnGround = ptr->readBool(L"onGround", true);
        m_textureName = ptr->readString(L"texture");

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
        return true;
    }

    void TerrainMap::saveMap(const tstring & path)
    {
        m_mapName = path;
        formatDirName(m_mapName);

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

    int TerrainMap::position2chunk(float x, float z) const
    {
        int row = int((z - m_rect.top) / MapConfig::ChunkSize);
        int col = int((x - m_rect.left) / MapConfig::ChunkSize);
        return row * m_nodeC + col;
    }

    float TerrainMap::getHeight(float x, float z) const
    {
        size_t index = position2chunk(x, z);
        if (index >= m_mapNodes.size()) return 0.0f;

        return m_mapNodes[index]->getPhysicalHeight(x, z);
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
                rc.left = c * m_nodeSize + m_rect.left;
                rc.top = r * m_nodeSize + m_rect.top;
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
        if (pTex) pDevice->SetTexture(0, pTex);
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
            return getChunkByPos(pos.x, pos.z);
        }
        return NULL;
    }

    ChunkPtr TerrainMap::getChunkByID(uint32 id) const
    {
        uint32 r = (id >> 16) & 0xffff;
        uint32 c = id & 0xffff;
        return m_mapNodes[r * m_nodeC + c];
    }

    ChunkPtr TerrainMap::getChunkByIndex(size_t index) const
    {
        return m_mapNodes[index]; 
    }

    /** 根据坐标取得地图结点。*/
    ChunkPtr TerrainMap::getChunkByPos(float x, float z) const
    {
        size_t index = position2chunk(x, z);
        assert(index < m_mapNodes.size() && "TerrainMap::getNode");

        return m_mapNodes[index];
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

} // end namespace Lazy