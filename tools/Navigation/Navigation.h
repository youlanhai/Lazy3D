#pragma once

/** 导航系统

一 生成导航网格

原理：
将整个mesh切分成最小的立方体网格，每个网格在y方向上由若干个障碍区域组成。
求障碍区域的相反区域，可以得到可通行区域。
每个可通行区域，与其周围网格中的可通行区域，可以建立起连接关系，从而形成一张导航网格。

大地图导航网格处理：
大地图必须切分成 n x m 规模的小区域。每个区域执行上面的算法生成导航网格数据，然后将
网格系统按照邻接信息进行连接。


二 自动寻路


*/

#include "Physics/PathFind.h"
#include "Core/TerrinMap.h"

#ifdef _DEBUG
#define NAGEN_SHOW_TRIANGLE 1
#else
#define NAGEN_SHOW_TRIANGLE 0
#endif

namespace Physics
{
    ///构造参考模型的aabb
    void makeRoleAABB(const NagenConfig & config, Physics::AABB & aabb, const Physics::Vector3 & pos);

    ///////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////

    ///路点数据，用于生成路点
    class WayPointData : public WayPoint
    {
    public:
        explicit WayPointData(int id_);

        ///设置邻接点
        //void addNeighbour(int id);
        inline void setNeighbour(int dir, int id){ adjPoints[dir]= id; }

        ///连接邻居的邻居，形成8方向搜索数据。
        void connectFarNeigbour(WaySetPtr pSet);

        ///删除无用邻接点
        void optimize();

    };
    typedef RefPtr<WayPointData> WayPointDataPtr;

    ///路点集合的数据，用于路点生成中
    class WaySetData : public WaySet
    {
    public:
        WaySetData(int id, int chunkID)
            : WaySet(id, chunkID)
        {}

        ///添加一个waypoint
        WayPointDataPtr pushWayPoint();

        ///在waypoint与wayset之间建立连接关系
        void bindWayPoints();
    };

    //////////////////////////////////////////////////////////////////////////
    //生成路点相关
    //////////////////////////////////////////////////////////////////////////

    class CubeNode;
    class ChunkNode;
    typedef RefPtr<ChunkNode> ChunkNodePtr;

    ///高度域。记录着一个障碍/可通行区域
    class HeightField
    {
    public:
        HeightField();
        HeightField(CubeNode* node, float ymin, float ymax, HeightField* next);
        ~HeightField();

        ///将高度域与路点建立连接关系
        void bindWp(WayPointPtr wp);

        //使用内存池
        DEC_MEMORY_ALLOCATER()

    public:
        HeightField*    pNext;///<下一个高度域
        float           yMin;///<此区域最小y值
        float           yMax;///<此区域最大y值
        CubeNode*       pNode;///<所属的结点
        int             mark;///<表示是否生成过路点。
        int             wpId;///<路点编号
        int             setId;///<所属wayset
    };

    ///chunk网格内的一个结点。为一系列高度域的拥有者。
    class CubeNode
    {
    public:
        CubeNode(int id_, const FRect & rc );
        ~CubeNode();

        void render(LPDIRECT3DDEVICE9 pDevice);

        ///添加一个地形三角形，生成三角形对应的高度域。
        void addTriangle(const Triangle & tri);

        ///将障碍高度域，反转成可通行高度域。
        void inverseHeightField();

        ///搜索符合攀爬的高度域。分为两种：向上爬，和向下走
        HeightField* searchHF(float yMin, float yMax);

        ///与其他chunk结点建立连接关系
        void linkWithOtherNode(CubeNode * pOther);
    public:
        int             id;
        FRect           rect;
        HeightField     hfHead;//高度域

#if NAGEN_SHOW_TRIANGLE
        TriangleSet triangles;
#endif
    };
    typedef std::vector<CubeNode> CubeArray;


    ///一个chunk
    class ChunkNode : public IBase
    {
    public:
        ChunkNode(int id, const FRect & rc);
        ~ChunkNode();

        int id() const { return chunkID; }

        ///释放占用的额外空间
        void clear();

        void render(LPDIRECT3DDEVICE9 pDevice);

        ///添加一个待计算的三角形
        void addTriangle(const Triangle & tri);
        void addMesh(LPD3DXMESH mesh, const Matrix4x4 * pWorld=NULL);

        ///生成路点
        void genWayPoint();

        CubeNode* getNode(int r, int c);
        CubeNode* getNode(int id){ return &cubeNodes[id]; }

        int numRow() const { return rows; }
        int numCol() const { return cols; }

        ///是否计算完成
        bool isGenFinished() const { return finished; }

        ///是否全部完成
        bool isAllFinished() const
        {
            for(int i=0; i<4; ++i)
                if(!linkState[i]) return false;
            return true;
        }

        ///设置与邻居的连接状态
        void setLinkState(int i, bool s){ linkState[i] = s; }

        ///与邻居是否已建立连接
        bool isLinked(int i)const{ return linkState[i]; }

        ///与邻接chunk建立连接关系
        void linkWithChunk(int dir, ChunkNodePtr otherNode);

    private:

        ///加载数据
        void doLoad();

        ///对一个可走结点，使用广度优先搜索生成路点
        void genWayPoint(HeightField* pNode);

    private:
        int     chunkID;
        FRect   rect;
        int     rows;
        int     cols;

        bool    loaded;///<数据是否已加载
        bool    finished;///<是否计算完成
        bool    linkState[4];///<与周围邻居的连接状态

        CubeArray cubeNodes;
    };

    ///整个地形
    class ChunkNodeMgr
    {
        typedef std::vector<ChunkNodePtr> ChunkNodePool;
    public:
        ///唯一实例
        static ChunkNodeMgr* instance();

        ///生成路点
        bool doGenerate();

        ///保存路点数据
        bool doSave(const Lazy::tstring & path=Lazy::EmptyStr) const;

        ///调试渲染
        void render(LPDIRECT3DDEVICE9 pDevice) ;

    public:

        ChunkNodePtr getChunk(int id)
        {
            if(id < 0 || id >= int(m_nodes.size())) return NULL;
            return m_nodes[id]; 
        }

        ChunkNodePtr getChunk(int r, int c)
        {
            return getChunk(r*m_cols + c);
        }

        int numChunks() const { return int(m_nodes.size()); }
        int numRow() const { return m_rows; }
        int numCol() const { return m_cols; }

    private:
        
        ///加载地图配置，为计算路点做准备
        bool doLoadConfig();

        ///重置为row行，col列
        void reset(int rows, int cols, float size);

    private:
        ChunkNodeMgr();
        ~ChunkNodeMgr();
        
        Lazy::tstring   m_wayPath;///<地图名称
        int             m_rows;///<chunk行数
        int             m_cols;///<chunk列数
        ChunkNodePool   m_nodes;///<chunk
    };

}//namespace Physics