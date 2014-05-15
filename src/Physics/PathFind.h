#pragma once

namespace Physics
{
    ///导航网格配置参数
    struct NagenConfig
    {
        float clampHeight;  ///<最大可爬升高度
        float modelHeight;  ///<模型高度
        float cubeSize;     ///<立方体网格尺寸
        int   minSetPoints; ///<每个wayset最小的路点个数
    };

    ///debug参数
    namespace NagenDebug
    {
        extern bool showLayer;
        extern bool showMesh;
        extern bool showHF;
        extern bool showTriangle;
        extern bool showWP;
        extern bool showPath;
    }

    namespace FindWay
    {
        ///全局的配置参数
        extern NagenConfig Config;

        ///设置配置参数
        inline void setConfig(const NagenConfig & config){ Config = config;}

        ///获取配置参数
        inline const NagenConfig & config()  { return Config; }

        ///路点文件路径
        extern Lazy::tstring WpPath;
    }
    
    const int MaxWpAdj = 8; ///<路点最大邻接点数。以后会扩展成8方向搜索，最多拥有8个邻接点。
    const int InvalidWp = -1;//无效id

    inline bool isInvalidWp(int id)
    {
        return id == InvalidWp;
    }
    inline bool isValidWp(int id)
    {
        return id != InvalidWp;
    }

    ///获取区域的中心坐标
    inline void getRectCenter(const FRect & rect, float y, Vector3 & pos)
    {
        pos.x = (rect.left + rect.right) * 0.5f;
        pos.y = y;
        pos.z = (rect.top + rect.bottom) * 0.5f;
    }

    
    typedef std::vector<int> WpIdArray;

    ///////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////
    
    ///路点数据
    class WayPoint: public IBase
    {
    public:
        int     id;
        FRect   rect;
        float   yMin;
//        int     setID;
//        float   yMax;

    public:
        explicit WayPoint(int id_);
        ~WayPoint();

        void render(LPDIRECT3DDEVICE9 pDevice, DWORD color=0xff00ffff) const;

        ///获得路点的中心位置
        inline void getCenter(Vector3 & center) const
        {
            getRectCenter(rect, yMin, center);
        }
        
        ///路点中心到pos的距离
        float distTo(const Vector3 & pos) const;
        
        ///加载数据。
        bool load(Lazy::DataStream & stream);

        ///保存数据。
        bool save(Lazy::DataStream & stream) const;
        
        //申明内存池
        DEC_MEMORY_ALLOCATER()

    public://A*寻路相关

        ///获取当前id
        inline int getId() const { return id; }

        //计算估价距离
        float distTo(const WayPoint & wp) const;

        ///邻接点个数
        inline int numNeighbour() const { return numAdj; }
        
        ///获取邻接点
        inline int getNeighbour(int i) const { return adjPoints[i]; }

    protected:
        int  numAdj;
        int  adjPoints[MaxWpAdj];///<4个邻接路点编号。InvalidWp表示无邻接点。
    };

    typedef RefPtr<WayPoint> WayPointPtr;
    typedef std::vector<WayPointPtr> WpPtrArray;

    ///////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////
    
    ///路点连接
    struct WpLinker
    {
        int wp;
        int destWp;
    };

    typedef std::vector<WpLinker> LinkerArray;

    ///路点集合
    class WaySet : public IBase
    {
    public:
        int     id;
//        int     chunkID;
        FRect   rect; //set的范围
//        float   yMax;
        float   yMin;
        
    public:
        WaySet(int id_, int chunkID_);
        ~WaySet();

        void render(LPDIRECT3DDEVICE9 pDevice) const;
    
        //查找最接近目标点的路点。
        int findWpByPos(const Vector3 &pos, float *distance=NULL) const;

        ///搜索selfWpId距离下一个set最近的一个路点。
        ///@param[in]   destPos     最终的目标位置
        ///@param[in]   nextSetId   下一个set
        ///@param[in]   selfWpId    源路点
        ///@param[out]  selfDestWp  同set中的目标路点
        ///@param[out]  nextDestWp  下一个路点（selfDestWp的另一端），位于nextSet中
        ///@return 如果成功，返回本true，否则返回false。
        bool findEdgeWp(const Vector3 & destPos, int selfWpId, int nextSetId, int & selfDestWp, int & nextDestWp) const;
        
        ///自动寻路
        bool findWayById(WpPtrArray & way, int start, int end) const;
        
        ///自动寻路
        bool findWay(WpPtrArray & way, const Vector3 & start, const Vector3 & end) const;

        WayPointPtr getWayPoint(int i) const { return m_wayPoints[i]; }

        ///路点个数
        int numWp() const { return (int)m_wayPoints.size(); }

        ///路点迭代器
        WpPtrArray::iterator begin(){ return m_wayPoints.begin(); }
        WpPtrArray::iterator end(){ return m_wayPoints.end(); }

        ///记录一个连通的邻居
        void addNeighbour(int setId);

        ///记录一个边界连接关系
        void addEdgeLinker(int destSet, const WpLinker & linker);

        //申明内存池
        DEC_MEMORY_ALLOCATER()

    public://A*寻路相关

        inline int getId() const { return id; }

        ///获取路点数据
        WayPointPtr getData(int id) const { return m_wayPoints[id]; }

        ///计算估价距离
        float distTo(const WaySet & wset) const;

        ///邻接点个数
        inline int numNeighbour() const { return int(adjSets.size()); }
        
        ///获取邻接点
        inline int getNeighbour(int i) const { return adjSets[i]; }

    public://存档相关
        
        ///保存到文件。
        ///@param[in] fname 目标文件名
        bool save(const Lazy::tstring & fname) const;
    
        ///从文件加载
        ///@param[in] fname 目标文件名
        bool load(const Lazy::tstring & fname);

        ///加载简洁信息，不含路点数据。
        bool loadInfo(Lazy::DataStream & stream);

        ///保存简洁信息，不含路点数据。
        bool saveInfo(Lazy::DataStream & stream) const;

    protected:

        ///真正加载路点数据
        void doLoadWayPoint();

    protected:
        WpIdArray    adjSets;    ///<邻接set

        bool         m_loaded; //数据是否已加载
        WpPtrArray   m_wayPoints; ///<路点数据

        typedef std::map<int, LinkerArray> DetailMap;
        DetailMap m_adjDetail;///<邻接的详细信息
    };
    typedef RefPtr<WaySet> WaySetPtr;
    typedef std::vector<WaySetPtr> WaySetArray;

    ///////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////

    ///管理着整个地图的“路点集合”信息
    class WaySetMgr : public IBase
    {
    public:
        ///单例
        static WaySetMgr * instance();
        
        ///清空数据
        void clear();
        
        ///保存到文件。
        ///@param[in] fname 目标路径
        bool save(const Lazy::tstring & path) const;
    
        ///从文件加载
        ///@param[in] fname 目标路径
        bool load(const Lazy::tstring & path);
    
        ///添加一个wayset，返回新添加wayset的id。    
        int addWaySet(WaySetPtr ptr);
        
        ///根据编号获取wayset
        WaySetPtr getWaySet(int i) const { return m_waySets[i]; }

        ///获取wayset数据
        WaySetPtr getData(int id) const { return m_waySets[id]; }

        ///只在set之间直接寻路。不涉及waypoint
        bool findWay(WaySetArray & way, int startId, int endId);

    private:
        WaySetMgr()
        {}

        ~WaySetMgr()
        {}

        std::vector<WaySetPtr> m_waySets;
    };

    ///////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////

    ///记录每个chunk的路点及路点集合的信息
    class WayChunk : public IBase
    {
    public:
        WayChunk(int id);

        ///从文件流中加载。
        bool load(Lazy::LZDataPtr stream);
        
        ///保存到文件流。一个地图的所有chunk信息，都保存到一个文件。
        bool save(Lazy::LZDataPtr stream) const;

        void render(LPDIRECT3DDEVICE9 pDevice) const;

        void addWaySet(int id){ m_waySets.push_back(id); }
        int numWaySet()const { return m_waySets.size(); }
        int numWayPoint() const;

        int getId() const { return m_id; }

        //返回way point id
        int findWpByPos(const Vector3 &pos) const;

        //返回waySetId
        int findSetByPos(const Vector3 &pos, int *wpId = NULL ) const;

        //在一个waySet中寻路
        bool findWay(WpPtrArray & way, const Vector3 & start, const Vector3 & end) const;
        
        //在一个waySet中寻路
        bool findWay(WpPtrArray & way, int waySetId, const Vector3 & start, const Vector3 & end) const;
        
        WpIdArray::iterator begin(){ return m_waySets.begin(); }
        WpIdArray::iterator end() { return m_waySets.end(); }

    private:
        int         m_id;///<chunk自身的id
        WpIdArray   m_waySets; ///<拥有的路点集合
    };
    typedef RefPtr<WayChunk> WayChunkPtr;

    ///////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////

    ///寻路chunk管理器。管理着一个地图的寻路数据。
    class WayChunkMgr
    {
    public:
        ///单例
        static WayChunkMgr * instance();

        ///加载寻路地图
        ///@param[in] path 目标路径
        bool load(const Lazy::tstring & path);

        ///获取寻路chunk
        ///@param[in] id chunk索引id
        ///@param[in] urgent 如果urgent=false，且此chunk还未加载时，返回值为null。
        WayChunkPtr getChunk(int id, bool urgent=true);

        ///根据位置获取寻路chunk
        WayChunkPtr getChunkByPos(float x, float z, bool urgent=true);
        
        ///跨chunk寻路
        bool findWay(WpPtrArray & way, const Vector3 & start, const Vector3 & end);

    public:

        ///调试渲染
        void render(LPDIRECT3DDEVICE9 pDevice);

        ///重置waychunk用于计算寻路数据
        void resetWayChunk(int rows, int cols, float x0, float z0, float size);
        
        ///保存寻路地图
        ///@param[in] path 目标路径
        bool save(const Lazy::tstring & path) const;

    private:

        ///重置地图chunk数量
        void resetChunks(int n);

        WayChunkMgr();

        ~WayChunkMgr();

    private:
        int m_rows;///<chunk行数
        int m_cols;///<chunk列数
        
        float m_x0;///<地图原点x坐标
        float m_z0;///<地图原点z坐标
        float m_chunkSize;///<每个chunk的尺寸

        Lazy::tstring m_path;///<寻路数据文件
        std::vector<WayChunkPtr> m_chunkPool; ///<wayChunk数组。到真正用的时候，才加载chunk的寻路数据。
    };


    typedef std::vector<Vector3> Vector3Array;
    ///路点转换成坐标
    void wayPointToPosition(Vector3Array & arr, const WpPtrArray & way);

    ///地图名称，转换成路点路径
    bool mapNameToWpPath(Lazy::tstring & wpPath, const Lazy::tstring & mapName);

}