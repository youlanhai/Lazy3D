#pragma once

#include "RenderTask.h"
#include "TerrainChunk.h"
#include "../Physics/QuadTree.h"

namespace Lazy
{
    //////////////////////////////////////////////////////////////////////////
    /// TerrainMap
    //////////////////////////////////////////////////////////////////////////
    class LZDLL_API TerrainMap :
        public IBase,
        public IRenderable,
        public Singleton<TerrainMap>
    {
    public:
        typedef std::vector<ChunkPtr>       ChunkPool;
        typedef ChunkPool::iterator         ChunkIterator;

        TerrainMap();
        ~TerrainMap(void);

        void render(IDirect3DDevice9* pDevice);
        void update(float elapse);

        bool loadMap(const tstring & mapName);
        bool saveMap(const tstring & mapName = _T(""));
        /** create a new map*/
        bool createMap(const tstring & nameName, int rows, int cols);

        /** 获得物理高度*/
        float getHeight(float x, float z) const;
        HeightMapPtr getHeightMap() const { return m_heightmap; }

        const tstring & getMapName() const { return m_mapName; }

        ///地图是否可用
        BOOL isUserfull() const { return m_usefull; }

    public:
        /** 地图将以该source进行优化绘制。source往往是玩家。*/
        void setSource(SceneNode* pSource) { m_pSource = pSource; }
        SceneNode * getSource() { return m_pSource; }

        ///获取和设置显示范围
        float getShowRadius() const { return m_showRadius; }
        void setShowRadius(float r) { m_showRadius = r;  }

        ///可见的地形块数量
        size_t numVisibleChunk() const { return m_visibleChunks.size(); }

        /** 是否在地图范围。*/
        bool isInBound(float x, float z);

        const FRect getRect() const { return m_rect; }

        ChunkPtr getFocusChunk();

        void setLoadingProgress(float p) {m_loadingProgress = p; }
        float getLoadingProgress() const { return m_loadingProgress;}

        ChunkIterator begin() { return m_chunks.begin(); }
        ChunkIterator end() { return m_chunks.end(); }

        int getChunkRows() const { return m_chunkRows; }
        int getChunkCols() const { return m_chunkCols; }
        float getChunkSize() const { return m_chunkSize; }

        int position2chunk(float x, float z) const;

        ChunkPtr getChunkByID(uint32 id) const;
        ChunkPtr getChunkByIndex(size_t index) const;
        ChunkPtr getChunkByPos(float x, float z) const;

        ///加载所有的chunk。如果MapConfig::UseMultiThread为true，此函数会立即返回。
        void loadAllChunks(bool load);

        ///是否所有的chunk都已经加载
        bool isAllChunkLoaded();

    public://拾取相关

        bool handeEvent(const SEvent & event);

        ///求鼠标射线与地表交点
        bool intersect(Vector3 & position);

        bool intersectWithCursor();

        void onChunkLoaded(TerrainChunk * chunk);
        void onChunkUnloaded(TerrainChunk* chunk);
        bool ifChunkOutside(TerrainChunk* chunk);

    protected:

        /// 释放当前地图资源
        void clearCurMap(void);

        ///加载chunk
        void initChunks(void);

        ///生成可见的chunk
        void genVisibleChunks(void);

    protected:

        SceneNodePtr        m_root;

        ChunkPool           m_chunks;
        ChunkPool           m_visibleChunks;  ///9格
        SceneNode*          m_pSource;
        tstring             m_mapName;
        HeightMapPtr        m_heightmap;

        float               m_chunkSize;     ///< 每个结点的尺寸。
        int                 m_chunkRows;        ///< 结点行数
        int                 m_chunkCols;        ///< 结点列数
        float               m_showRadius;   ///<以玩家为中心的，显示范围半径
        BOOL                m_usefull;      ///< 是否可使用
        FRect               m_rect;

        BOOL                m_objOnGround;  ///< 物体是否贴地
        BOOL                m_loadAll;

        float               m_loadingProgress;
        QuadTree            m_quadTree;     ///<四叉树
    };

} // end namespace Lazy
