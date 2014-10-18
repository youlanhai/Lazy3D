#pragma once

///新版地形。支持多层纹理混合。
#define USE_NEW_CHUNK_STYLE 0

///用于地图编辑器
#define USE_FOR_MAP_EDIT 1

namespace Lazy
{
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

        ///使用多线程。注意：如果需要存贮地形数据，必须禁用多线程加载。
        extern bool UseMultiThread;

        ///是否显示地形
        extern bool ShowTerrain;

        ///最大chunk个数
        const int MaxNbChunks = 1024 * 1024;

        const int NbChunkGrid = 50;

        ///chunk行（列）顶点个数
        const int NbChunkVertex = NbChunkGrid + 1;

        ///chunk一个网格的尺寸
        const float ChunkGridSize = 1.0f;

        ///一个chunk的尺寸
        const float ChunkSize = NbChunkGrid * ChunkGridSize;

    }

    ///地形顶点格式
    struct TerrinVertex
    {
        Vector3 pos;
        Vector3 nml;
        Vector2 uv1;
        Vector2 uv2;

        static DWORD FVF;
        static int SIZE;
    };

    /// 高度图文件格式：灰度图，每个像素表示了一个点高度。
    class HeightMap : public IBase
    {
    public:
        HeightMap();

        bool load(const tstring & filename);

        size_t rows() const { return m_rows; }
        size_t cols() const { return m_cols; }

        float getHeight(float x, float z) const;

    private:
        tstring                 m_resource;
        std::vector<float>      m_rawdata;

        ///地形原点，默认从(0, 0, 0)开始，向x正轴和z负轴方向增加。
        Vector3                 m_origin;
        float                   m_gridSize;
        size_t                  m_rows;
        size_t                  m_cols;
    };

} // end namespace Lazy
