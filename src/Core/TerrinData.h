#pragma once

///新版地形。支持多层纹理混合。
#define USE_NEW_CHUNK_STYLE 0

///用于地图编辑器
#define USE_FOR_MAP_EDIT 1


///地图配置参数
namespace MapConfig
{
    ///显示chunk八叉树
    extern bool ShowChunkOctree;

    ///是否可选中场景物体
    extern bool CanSelectItem;

    ///显示物体八叉树。当物体被选中时，是否显示其八叉树。
    extern bool ShowItemOctree;

    ///显示所有物体AABB。
    extern bool ShowAllItemAABB;

    ///使用新格式
    extern bool UseNewFormat;

    ///使用多线程。注意：如果需要存贮地形数据，必须禁用多线程加载。
    extern bool UseMultiThread;

    ///是否显示地形
    extern bool ShowTerrain;

#if USE_NEW_CHUNK_STYLE
    ///最大chunk个数
    const int MaxChunks = 1024*1024;


    ///chunk行（列）顶点个数
    const int ChunkVertices = 128;

    ///chunk一个网格的尺寸
    const float ChunkGridSize = 1.0f;

    ///一个chunk的尺寸
    const float ChunkSize = ChunkVertices * ChunkGridSize;
#endif
}


///自定义顶点格式
struct TerrinVertex
{
    D3DXVECTOR3 pos;	//< 未经过坐标变换后的点
    D3DXVECTOR3 nml;	//< 顶点法向量
    float u,v;		//< 纹理坐标
    static DWORD FVF;//<灵活顶点格式
    static int SIZE;
};


/** 地形数据。高度图。*/
class TerrinData : public IBase
{
public:
    TerrinData(void);
    ~TerrinData(void);

    /** 加载高度图数据。*/
    bool loadHeightMap(
        const std::wstring & name, 
        int vrow, 
        int vcol,
        float squaresize,
        float heightScale,
        bool useOneTex);

    /** 是否在地图范围中。*/
    bool isInBound(float x, float z);

    /** 获得物理高度*/
    float getPhysicalHeight(float x, float z);

    /** 获得高度图高度。*/
    float getHeight(int r, int c);

    /** 得到地图实际宽度。*/
    float width(void){ return m_width; }

    /** 得到地图实际高度。*/
    float height(void){ return m_height; }

    /** 顶点行数。*/
    int vrows(void){ return m_vrows; }

    /** 顶点列数。*/
    int vcols(void){ return m_vcols; }

    float squareSize(void){ return m_squareSize; }

    bool useOneTex(void){ return m_useOneTex; }

    /** 线性插值*/
    float lerp(float a, float b, float t);

protected:
    std::wstring       m_mapName;  ///<地图文件名
    std::vector<BYTE>   m_vecRaw;   ///< 地形高度数据
    int     m_vrows;	    ///< 正方形网格行顶点数
    int     m_vcols;	    ///< 正方形网格列顶点数
    float   m_squareSize;   ///< 每个方格大小
    float   m_width;
    float   m_height;
    float   m_heightScale;   //< 高度缩放系数
    bool    m_useOneTex;
};
