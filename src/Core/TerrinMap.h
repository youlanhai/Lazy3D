#pragma once

#include "TerrinData.h"
#include "RenderTask.h"
#include "TerrainItem.h"
#include "TerrainChunk.h"
#include "../Physics/QuadTree.h"

//////////////////////////////////////////////////////////////////////////
class LZDLL_API TerrainMap : public IBase, public IRenderObj
{
public:
    typedef std::vector<ChunkPtr>       NodeContainer;
    typedef NodeContainer::iterator             NodeIterator;

    typedef std::map<int, ChunkPtr>   ChunkMap;

    TerrainMap();
    ~TerrainMap(void);

public:
    static TerrainMap * instance();

    bool init();
    void fini();

    ///更新
    void render(IDirect3DDevice9* pDevice);

    ///渲染
    void update(float elapse);

    ///加载地图。
    bool loadMap(const Lazy::tstring & mapName);

    ///保存地图。
    void saveMap(const Lazy::tstring & mapName);

#if USE_NEW_CHUNK_STYLE
    ///创建新地图。默认会创建一个长方形地图，可以动态添加和删除chunk达到任意形状。
    bool createMap(const Lazy::tstring & mapName, int rows, int cols);

    bool createMap(float widht, float height, const Lazy::tstring & mapName);

    ///导入高度图
    bool importHeightMap(const Lazy::tstring & config);

    ///导出高度图
    bool exportHeightMap(const Lazy::tstring & config);

    ///创建一个新的地形块
    bool createChunk(float x, float y);

    ///导入一个地形块
    bool importChunk(const Lazy::tstring & file);

    ///删除一个地形块
    bool removeChunk(int id);

#endif

public:

    /** 地图将以该source进行优化绘制。source往往是玩家。*/
    void setSource(I3DObject* pSource){ m_pSource = pSource; }

    /** 是否在地图范围。*/
    bool isInBound(float x, float z);

    /** 获得物理高度*/
    float getHeight(float x, float z);

    ///获得地形数据
    TerrinData* getTerrinData(void){ return m_pTData.get(); }

    const Lazy::tstring & getMapName() const { return m_mapName; }

    //数据类型转换
    void doDataConvert();

    ///地图是否可用
    bool isUserfull() const { return m_usefull; }

public:
    void setShowLevel(int level);

    int getShowLevel(void){ return m_showLevel; }

    ///获取和设置显示范围
    float getShowRadius() const { return m_showRadius; }
    void setShowRadius(float r){ m_showRadius = r;  }

    ///可见的地形块数量
    size_t numVisibleChunk() const { return m_renderNodes.size(); }

    float xMin();

    float xMax();

    float zMin();

    float zMax();

    float width();

    float height();

    ///求鼠标射线与地表交点
    bool intersect(Math::Vector3 & position);

    ChunkPtr currentNode();

    void setLoadingProgress(float p){m_loadingProgress = p; }
    float getLoadingProgress() const { return m_loadingProgress;}

    NodeIterator begin(){ return m_mapNodes.begin(); }
    NodeIterator end(){ return m_mapNodes.end(); }

    int nodeRows() const { return m_nodeR; }
    int nodeCols() const { return m_nodeC; }
    float nodeSize() const { return m_nodeSize; }

    ChunkPtr getNode(size_t index) const { return m_mapNodes[index]; }

    /** 根据索引获得地图结点。*/
    TerrainChunk* getNode(int r, int c);

    /** 根据坐标取得地图结点。*/
    TerrainChunk* getNode(float x, float z);

    /** 根据真实位置得到子结点索引。x:为列，y:为行。*/
    Square2 getNodeIndex(float x, float z);

    /** 获得地图索引。*/
    Square2 getSquareIndex(float x, float z);

    void processExternalItem(TerrainItemPtr item);

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

    TerrainItemPtr getActiveObj(){ return m_pActiveObj; }

    void setActiveObj(TerrainItemPtr pObj);

    TerrainItemPtr getSelectObj(){ return m_pSelectObj; }

protected://顶点无关
    ChunkMap   m_chunks;
    int        m_idAllocator;

    NodeContainer       m_mapNodes;
    NodeContainer       m_renderNodes;//9格
    I3DObject*          m_pSource;
    RefPtr<TerrinData>  m_pTData;
    Lazy::tstring         m_textureName;
    Lazy::tstring         m_mapName;

    float   m_nodeSize;     //< 每个结点的尺寸。
    int     m_nodeR;        //< 结点行数
    int     m_nodeC;        //< 结点列数
    
    float   m_showRadius;   ///<以玩家为中心的，显示范围半径
    int     m_showLevel;    //< 显示级别
    bool    m_usefull;      //< 是否可使用

    Lazy::tstring  m_objFileName;
    TerrainItemPtr m_pActiveObj;//<活动的OBJ，即拥有鼠标焦点
    TerrainItemPtr m_pSelectObj;//<被选择的Obj
    bool         m_objOnGround;//< 物体是否贴地
    
    float       m_loadingProgress;

    Physics::QuadTree m_quadTree;///<四叉树
};
