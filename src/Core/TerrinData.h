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

        const int NbChunkGrid = 100;

        ///chunk行（列）顶点个数
        const int NbChunkVertex = NbChunkGrid + 1;

        ///chunk一个网格的尺寸
        const float ChunkGridSize = 1.0f;

        ///一个chunk的尺寸
        const float ChunkSize = NbChunkGrid * ChunkGridSize;

    }

    ///自定义顶点格式
    struct TerrinVertex
    {
        D3DXVECTOR3 pos;	//< 未经过坐标变换后的点
        D3DXVECTOR3 nml;	//< 顶点法向量
        float u, v;		//< 纹理坐标
        static DWORD FVF;//<灵活顶点格式
        static int SIZE;
    };

} // end namespace Lazy
