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
        typedef std::vector<ChunkPtr>       NodeContainer;
        typedef NodeContainer::iterator             NodeIterator;

        typedef std::map<int, ChunkPtr>   ChunkMap;

        TerrainMap();
        ~TerrainMap(void);

        ///更新
        void render(IDirect3DDevice9* pDevice);

        ///渲染
        void update(float elapse);

        ///加载地图。
        bool loadMap(const tstring & mapName);

        ///保存地图。
        void saveMap(const tstring & mapName);

        bool createMap(const tstring & nameName, int rows, int cols);

        void addTerrainItem(TerrainItemPtr item);
        void delTerrainItem(TerrainItemPtr item);

        /** 获得物理高度*/
        float getHeight(float x, float z) const;

        const tstring & getMapName() const { return m_mapName; }

        ///地图是否可用
        bool isUserfull() const { return m_usefull; }

    public:
        /** 地图将以该source进行优化绘制。source往往是玩家。*/
        void setSource(I3DObject* pSource) { m_pSource = pSource; }

        void setShowLevel(int level);
        int getShowLevel(void) { return m_showLevel; }

        ///获取和设置显示范围
        float getShowRadius() const { return m_showRadius; }
        void setShowRadius(float r) { m_showRadius = r;  }

        ///可见的地形块数量
        size_t numVisibleChunk() const { return m_renderNodes.size(); }

        /** 是否在地图范围。*/
        bool isInBound(float x, float z);

        float xMin() const;
        float xMax() const;

        float zMin() const;
        float zMax() const;

        float width() const;
        float height() const;

        const FRect getRect() const { return m_rect; }

        ///求鼠标射线与地表交点
        bool intersect(Vector3 & position);

        ChunkPtr currentNode();

        void setLoadingProgress(float p) {m_loadingProgress = p; }
        float getLoadingProgress() const { return m_loadingProgress;}

        NodeIterator begin() { return m_mapNodes.begin(); }
        NodeIterator end() { return m_mapNodes.end(); }

        int nodeRows() const { return m_nodeR; }
        int nodeCols() const { return m_nodeC; }
        float nodeSize() const { return m_nodeSize; }

        int position2chunk(float x, float z) const;

        ChunkPtr getChunkByID(uint32 id) const;
        ChunkPtr getNodeByIndex(size_t index) const { return m_mapNodes[index]; }
        ChunkPtr getNodeByPos(float x, float z) const;

        ///加载所有的chunk。如果MapConfig::UseMultiThread为true，此函数会立即返回。
        void loadAllChunks();

        ///是否所有的chunk都已经加载
        bool isAllChunkLoaded();

    protected:

        /// 释放当前地图资源
        void clearCurMap(void);

        ///加载chunk
        void initChunks(void);

        ///生成可见的chunk
        void genVisibleChunks(void);

    public://拾取相关

        void handeMouseEvent(UINT msg, WPARAM wParam, LPARAM lParam );

        bool intersectWithCursor();

        TerrainItemPtr getActiveObj() { return m_pActiveObj; }

        void setActiveObj(TerrainItemPtr pObj);

        TerrainItemPtr getSelectObj() { return m_pSelectObj; }

    protected:
        NodeContainer       m_mapNodes;
        NodeContainer       m_renderNodes;  ///9格
        I3DObject*          m_pSource;
        tstring             m_textureName;
        tstring             m_mapName;

        float               m_nodeSize;     //< 每个结点的尺寸。
        int                 m_nodeR;        //< 结点行数
        int                 m_nodeC;        //< 结点列数
        float               m_showRadius;   ///<以玩家为中心的，显示范围半径
        int                 m_showLevel;    //< 显示级别
        bool                m_usefull;      //< 是否可使用
        FRect               m_rect;

        TerrainItemPtr      m_pActiveObj;   //<活动的OBJ，即拥有鼠标焦点
        TerrainItemPtr      m_pSelectObj;   //<被选择的Obj
        bool                m_objOnGround;  //< 物体是否贴地

        float               m_loadingProgress;
        QuadTree            m_quadTree;     ///<四叉树
    };

} // end namespace Lazy
