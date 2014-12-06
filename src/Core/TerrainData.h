#pragma once

#include "Base.h"

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

    
    /** 高度图文件格式：灰度图(支持8,16,32位深度)，每个像素表示了一个点高度。*/
    class HeightMap : public IBase
    {
    public:
        HeightMap();

        /** 从配置文件中读取配置，并加载高度图。*/
        bool load(const tstring & filename);

        int rows() const { return m_rows; }
        int cols() const { return m_cols; }

        const tstring & getResource() const { return m_resource; }

        /** 根据(x, z)坐标返回插值之后的y坐标。如果(x, z)超出边界，则返回m_origin.y。*/
        float getHeight(float x, float z) const;
        float getAbsHeight(int row, int col) const;

        Vector3 getNormal(float x, float z) const;
        Vector3 getNormal(int x, int z) const;
        
    private:
        tstring                 m_resource;
        std::vector<float>      m_rawdata;

        /** 地形原点，默认从(0, 0, 0)开始，向x正轴和z负轴方向增加。*/
        Vector3                 m_origin;
        float                   m_gridSize;
        int                     m_rows;
        int                     m_cols;
        float                   m_diagonalDistanceX4;
    };

    typedef RefPtr<HeightMap>   HeightMapPtr;

} // end namespace Lazy
