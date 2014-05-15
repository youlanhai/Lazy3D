#include "StdAfx.h"
#include "PathFind.h"

#include "PhysicsDebug.h"


namespace Physics
{
    const size_t WaysetMagic = Lazy::makeMagic('w', 's', 'e', 't');
    const size_t WaySetVersion = 0;
    const size_t WayPointMagic = Lazy::makeMagic('w', 'p', 'o', 't');
    
    namespace NagenDebug
    {
        bool showLayer = true;
        bool showMesh = true;
        bool showHF = false;
        bool showTriangle = false;
        bool showWP = true;
        bool showPath = true;
    }

    namespace FindWay
    {
        NagenConfig Config = { 0.5f, 1.8f, 0.5f, 8 };
        Lazy::tstring WpPath = _T("waypoint/");

        //访问标记 visit mark
        namespace Vm
        {
            const int None = 0;
            const int Open = 1;
            const int Close = 2;
        }
        
        
        ///寻路结点
        template<class T>
        struct WayNode : public IBase
        {
            typedef T wtype;
            typedef RefPtr<WayNode<wtype>> WayNodePtr;

            WayNode(){}
            ~WayNode(){}
        
            WayNode(WayNodePtr parent, wtype wp, wtype end)
                : pParent(parent)
                , pPoint(wp)
                , costToS(0)
            {
                assert(wp && end);

                makeCost(end);
            }

            ///获取数据的编号
            inline int getId() const { return pPoint->getId(); }

            ///计算花费
            void makeCost(wtype end)
            {
                if(pParent) costToS = pParent->costToS + pParent->pPoint->distTo(*pPoint);
                else costToS = 0;
                costTotal = costToS + pPoint->distTo(*end);
            }

            wtype       pPoint;///<结点数据
            WayNodePtr  pParent;///<父节点
            float       costToS;///<到起点的花费
            float       costTotal;///<总权值 = 起点花费 + 终点估价
            
            DEC_MEMORY_ALLOCATER()
        };

        ///将结点添加到open表中。估价值按照从小
        template<class T>
        void addToOpenList(std::list<T> & openList, T pNode)
        {
            std::list<T>::iterator it;
            FOR_EACH(openList, it)
            {
                if(pNode->costTotal <= (*it)->costTotal)
                {
                    break;
                }
            }

            if(it == openList.end()) openList.push_back(pNode);
            else openList.insert(it, pNode);
        }

        ///按编号搜索
        template<class T>
        struct PredWayNode
        {
            PredWayNode(int id) : id_(id) {}

            inline bool operator()(const T & node)
            {
                return id_ == node->getId();
            }

        private:
            int id_;
        };

        ///A*搜索算法
        template<class TYPE, class TYPE_O>
        bool findWayById(std::vector<TYPE> & way, int start, int end, TYPE_O owner)
        {
            typedef WayNode<TYPE> ThisNode;
            typedef ThisNode::WayNodePtr ThisNodePtr;

            //从终点搜索到起点
            std::swap(start, end);

            TYPE pEndPoint = owner->getData(end);

            if(start == end)
            {
                way.push_back(pEndPoint);
                return true;
            }
            
            std::list<ThisNodePtr> openList;//open表
            std::map<int, int> markMap;//标记表。记录每个路点的访问状态。

            addToOpenList<ThisNodePtr>(openList, new ThisNode(NULL, owner->getData(start), pEndPoint));
            markMap[start] = Vm::Open;

            while(!openList.empty())
            {
                //保证第一个元素，始终是权值最小的
                ThisNodePtr pWayNode = openList.front();
                openList.pop_front();
            
                TYPE pPoint = pWayNode->pPoint;
                markMap[pPoint->id] = Vm::Close;

                if(pPoint->id == end)
                {
                    ThisNodePtr p = pWayNode;
                    while(p)
                    {
                        way.push_back(p->pPoint);
                        p = p->pParent;
                    }
                    return true;
                }

                int nAdj = pPoint->numNeighbour();
                for(int i=0; i<nAdj; ++i)
                {
                    const int adjId = pPoint->getNeighbour(i);
                    //无效邻接
                    if(isInvalidWp(adjId)) continue;

                    const int mark = markMap[adjId];

                    if(mark == Vm::Close)
                    {
                       //已访问完毕
                    }
                    else if(mark == Vm::None)
                    {
                        ThisNodePtr pp = new ThisNode(pWayNode, owner->getData(adjId), pEndPoint);
                        addToOpenList(openList, pp);
                        markMap[adjId] = Vm::Open;
                    }
                    else if(mark == Vm::Open)
                    {
                        std::list<ThisNodePtr>::iterator it = std::find_if(
                            openList.begin(), openList.end(), PredWayNode<ThisNodePtr>(adjId));
                        assert(it!=openList.end() && "the adjId not in openlist.Because of th invalid mark value.");

                        ThisNodePtr pp = *it;
                        float costS = pWayNode->costToS + pp->pPoint->distTo(*pPoint);
                        if(costS < pp->costToS)
                        {
                            pp->pParent = pWayNode;
                            pp->makeCost(pEndPoint);

                            openList.erase(it);
                            addToOpenList(openList, pp);
                        }
                    }
                    else
                    {
                        assert(0 && "invalid mark value!");
                    }
                }
            }

            return false;
        }

    }//end FindWay

    ///////////////////////////////////////////////////////////////////

    WayPoint::WayPoint(int id_)
        : id(id_)
        , numAdj(0)
//        , setID(InvalidWp)
    {
        for (int i=0; i<MaxWpAdj; ++i)
        {
            adjPoints[i] = InvalidWp;
        }

    }

    WayPoint::~WayPoint()
    {
    }

    bool WayPoint::load(Lazy::DataStream & stream)
    {
        numAdj = 0;

        stream.loadStruct(id);
        stream.loadStruct(yMin);

        stream.loadStruct(rect.left);
        stream.loadStruct(rect.top);
        rect.right = rect.left + FindWay::Config.cubeSize;
        rect.bottom = rect.top + FindWay::Config.cubeSize;

//        stream.loadStruct(setID);
//        stream.loadStruct(yMax);

        //为邻接点个数扩展做准备
        stream.loadStruct(numAdj);
        assert(numAdj <= MaxWpAdj && "WayPoint::load");

        for(int i=0; i<numAdj; ++i)
        {
            stream.loadStruct(adjPoints[i]);
        }

        return true;
    }

    bool WayPoint::save(Lazy::DataStream & stream) const
    {
        stream.saveStruct(id);
        stream.saveStruct(yMin);

        stream.saveStruct(rect.left);
        stream.saveStruct(rect.top);

//        stream.saveStruct(setID);
//        stream.saveStruct(yMax);

        //为邻接点个数扩展做准备
        stream.saveStruct(numAdj);
        for(int i=0; i<numAdj; ++i)
        {
            stream.saveStruct(adjPoints[i]);
        }

        return true;
    }

    void WayPoint::render(LPDIRECT3DDEVICE9 pDevice, DWORD color) const
    {
        float dt = FindWay::Config.cubeSize / 10.0f;
        FRect rc = rect;
        rc.delta(-dt, -dt);
        drawFRectSolid(pDevice, rc, yMin+dt, color);
    }

    float WayPoint::distTo(const Vector3 & pos) const
    {
        Vector3 center;
        getRectCenter(rect, yMin, center);
        return pos.distToSq(center);
    }

    float WayPoint::distTo(const WayPoint & wp) const
    {
        Vector3 center1, center2;
        getRectCenter(rect, yMin, center1);
        getRectCenter(wp.rect, wp.yMin, center2);

        return center1.distToSq(center2);
    }

    ///////////////////////////////////////////////////////////////////
    WaySet::WaySet(int id_, int chunkID_)
        : id(id_)
//        , chunkID(chunkID_)
        , m_loaded(false)
//        , yMax(0)
        , yMin(0)
    {
    }

    WaySet::~WaySet()
    {
    }

    float WaySet::distTo(const WaySet & wset) const
    {
        Vector3 center1, center2;
        getRectCenter(rect, yMin, center1);
        getRectCenter(wset.rect, wset.yMin, center2);

        return center1.distToSq(center2);
    }

    void WaySet::render(LPDIRECT3DDEVICE9 pDevice) const
    {
        if(NagenDebug::showWP)
        {
            WpPtrArray::const_iterator it;
            FOR_EACH(m_wayPoints, it)
            {
                (*it)->render(pDevice);
            }
        }

        FRect rc = rect;
        rc.delta(-0.02f, -0.02f);
        drawFRect(pDevice, rc, 0, 0xff0000ff);
    }

    int WaySet::findWpByPos(const Vector3 &pos, float *distance) const
    {
        WpPtrArray::const_iterator it;

        int bestId = InvalidWp;
        float minDistance = MAX_FLOAT;

        FOR_EACH(m_wayPoints, it)
        {
            if(!(&*it)) continue;

            float dis = (*it)->distTo(pos);
            if(minDistance > dis)
            {
                minDistance = dis;
                bestId = (*it)->id;
            }
        }

        if(distance) *distance = minDistance;
        return bestId;
    }

    
    bool WaySet::findEdgeWp(const Vector3 & destPos, int selfWpId, int nextSetId, int & selfDestWp, int & nextDestWp) const
    {
        float minDistance = MAX_FLOAT;

        DetailMap::const_iterator linkIter = m_adjDetail.find(nextSetId);
        if(linkIter == m_adjDetail.end()) return false;

        WayPointPtr startWp = getWayPoint(selfWpId);

        const LinkerArray & arr = linkIter->second;
        LinkerArray::const_iterator it;
        FOR_EACH(arr, it)
        {
            WayPointPtr ptr = getWayPoint(it->wp);
            float dis = ptr->distTo(*startWp) + ptr->distTo(destPos);
            if(minDistance > dis)
            {
                minDistance = dis;
                selfDestWp = it->wp;
                nextDestWp = it->destWp;
            }
        }
        
        return minDistance < MAX_FLOAT;
    }

    bool WaySet::findWay(WpPtrArray & way, const Vector3 & start, const Vector3 & end) const
    {
        int startId = findWpByPos(start);
        if(isInvalidWp(startId)) return false;

        int endId = findWpByPos(end);
        if(isInvalidWp(endId)) return false;

        return findWayById(way, startId, endId);
    }

    bool WaySet::findWayById(WpPtrArray & way, int start, int end) const
    {
        return FindWay::findWayById<WayPointPtr>(way, start, end, this);
    }

    void WaySet::addNeighbour(int setId)
    {
        if(std::find(adjSets.begin(), adjSets.end(), setId) == adjSets.end())
            adjSets.push_back(setId);
    }

    bool operator==(const WpLinker & a, const WpLinker & b)
    {
        return a.wp == b.wp && a.destWp == b.destWp;
    }

    void WaySet::addEdgeLinker(int destSet, const WpLinker & linker)
    {
        addNeighbour(destSet);

#if 1
        LinkerArray & arr = m_adjDetail[destSet];
        for(size_t i=0; i<arr.size(); ++i)
        {
            if(arr[i] == linker)
            {
                assert(0 && "addEdgeLinker");
            }
        }
        arr.push_back(linker);
#else
        m_adjDetail[destSet].push_back(linker);
#endif
    }

    bool WaySet::save(const Lazy::tstring & fname) const
    {
#ifdef _DEBUG
        debugMessage(_T("TRACE: WaySet::save %d"), id);
#endif

        Lazy::DataStream stream;
//        stream.saveStruct(WayPointMagic);

        //保存路点数据
        stream.saveStruct(m_wayPoints.size());
        WpPtrArray::const_iterator it;
        FOR_EACH(m_wayPoints, it)
        {
            (*it)->save(stream);
        }

        //保存邻接数据
        stream.saveStruct(m_adjDetail.size());
        DetailMap::const_iterator dit;
        FOR_EACH(m_adjDetail, dit)
        {
            stream.saveStruct(dit->first);
            const LinkerArray & arr = dit->second;
            stream.saveStruct(arr.size());
            for(size_t i=0; i<arr.size(); ++i)
            {
                stream.saveStruct(arr[i]);
            }
        }

        Lazy::streambuffer & buffer = stream.steam();
        return Lazy::getfs()->writeBinary(fname, &buffer[0], buffer.size());
    }
    
    bool WaySet::load(const Lazy::tstring & fname)
    {
#ifdef _DEBUG
        debugMessage(_T("TRACE: WaySet::load %d"), id);
#endif

        m_wayPoints.clear();
        m_adjDetail.clear();

        Lazy::DataStream stream;
        if(!Lazy::getfs()->readBinary(fname, stream.steam()))
        {
            XWRITE_LOG(_T("ERROR: WaySet load data '%s' failed!"), fname.c_str());
            return false;
        }

#if 0
        size_t magic;
        stream.loadStruct(magic);
        if(WayPointMagic != magic)
        {
            XWRITE_LOG(_T("ERROR: Invalid WaySet data '%s'!"), fname.c_str());
            return false;
        }
#endif

        //加载路点数据
        size_t n;
        stream.loadStruct(n);
        m_wayPoints.reserve(n);

        for(size_t i=0; i<n; ++i)
        {
            WayPointPtr ptr = new WayPoint(i);
            ptr->load(stream);
            m_wayPoints.push_back(ptr);
        }

        //加载邻接数据
        stream.loadStruct(n);
        for(size_t i=0; i<n; ++i)
        {
            int adjId;
            stream.loadStruct(adjId);

            size_t numLink;
            WpLinker linker;
            stream.loadStruct(numLink);
            for(size_t k=0; k<numLink; ++k)
            {
                stream.loadStruct(linker);
                addEdgeLinker(adjId, linker);
            }
        }

        return true;
    }

    void WaySet::doLoadWayPoint()
    {
        m_loaded = true;
    }

    bool WaySet::loadInfo(Lazy::DataStream & stream)
    {

        stream.loadStruct(id);
        stream.loadStruct(rect);
        stream.loadStruct(yMin);

//        stream.loadStruct(chunkID);
//        stream.loadStruct(yMax);

        size_t n;
        stream.loadStruct(n);
        adjSets.resize(n);
        for(size_t i=0; i<adjSets.size(); ++i)
        {
            stream.saveStruct(adjSets[i]);
        }

        return true;
    }

    bool WaySet::saveInfo(Lazy::DataStream & stream) const
    {
        stream.saveStruct(id);
        stream.saveStruct(rect);
        stream.saveStruct(yMin);

//        stream.saveStruct(chunkID);
//        stream.saveStruct(yMax);

        stream.saveStruct(adjSets.size());
        for(size_t i=0; i<adjSets.size(); ++i)
        {
            stream.saveStruct(adjSets[i]);
        }

        return true;
    }

    ///////////////////////////////////////////////////////////////////////////

    bool WaySetMgr::findWay(WaySetArray & way, int start, int end)
    {
        return FindWay::findWayById<WaySetPtr>(way, start, end, this);
    }
    
    /*static*/ WaySetMgr * WaySetMgr::instance()
    {
        static WaySetMgr mgr;
        return &mgr;
    }

    ///清空数据
    void WaySetMgr::clear()
    {
        m_waySets.clear();
    }

    bool WaySetMgr::load(const Lazy::tstring & path)
    {
        debugMessage(_T("TRACE: WaySetMgr::load '%s'"), path.c_str());

        clear();


        Lazy::tstring pathName = path;
        Lazy::formatDirName(pathName);
        Lazy::tstring fname = pathName + _T("wayset.dat");

        Lazy::DataStream stream;
        if(!Lazy::getfs()->readBinary(fname, stream.steam()))
        {
            XWRITE_LOG(_T("ERROR: WaySetMgr save data '%s' failed!"), fname.c_str());
            return false;
        }

        //校验魔法数
        size_t magic, version;
        stream.loadStruct(magic);
        if(WaysetMagic != magic)
        {
            XWRITE_LOG(_T("ERROR: Invalid wayset data '%s'!"), fname.c_str());
            return false;
        }

        //校验版本
        stream.loadStruct(version);
        if(WaySetVersion != version)
        {
            XWRITE_LOG(_T("ERROR: wayset data '%s' Version not match!"), fname.c_str());
            return false;
        }

        //加载每个wayset的简略数据
        size_t n;
        stream.loadStruct(n);
        m_waySets.reserve(n);
        for(size_t i=0; i<n; ++i)
        {
            WaySetPtr ptr = new WaySet(0, 0);
            ptr->loadInfo(stream);
            m_waySets.push_back(ptr);
        }


#if 1   
        //可以等到用的时候，再加载详细数据

        //加载每个wayset的详细数据
        std::vector<WaySetPtr>::iterator it;
        FOR_EACH(m_waySets, it)
        {
            formatString(fname, _T("%d.wp"), (*it)->id);
            fname = pathName + fname;
            (*it)->load(fname);
        }
#endif
        return true;
    }

    bool WaySetMgr::save(const Lazy::tstring & path) const
    {
        Lazy::DataStream stream;
        stream.saveStruct(WaysetMagic);
        stream.saveStruct(WaySetVersion);

        //保存每个wayset的简略数据
        stream.saveStruct(m_waySets.size());
        std::vector<WaySetPtr>::const_iterator it;
        FOR_EACH(m_waySets, it)
        {
            (*it)->saveInfo(stream);
        }

        Lazy::tstring pathName = path;
        Lazy::formatDirName(pathName);
        Lazy::tstring fname = pathName + _T("wayset.dat");
        
        Lazy::streambuffer & buffer = stream.steam();
        if(!Lazy::getfs()->writeBinary(fname, &buffer[0], buffer.size()))
        {
            XWRITE_LOG(_T("ERROR: WaySetMgr save data '%s' failed!"), fname.c_str());
            return false;
        }

        //保存每个wayset的详细数据
        FOR_EACH(m_waySets, it)
        {
            formatString(fname, _T("%d.wp"), (*it)->id);
            fname = pathName + fname;
            if(!(*it)->save(fname))
            {
                XWRITE_LOG(_T("ERROR: WaySetMgr save sub set data '%s' failed!"), fname.c_str());
            }
        }
        return true;
    }
        
    int WaySetMgr::addWaySet(WaySetPtr ptr)
    {
        int id = (int)m_waySets.size();
        m_waySets.push_back(ptr);
        ptr->id = id;
        return id;
    }
    ///////////////////////////////////////////////////////////////////
    WayChunk::WayChunk(int id)
        : m_id(id)
    {

    }

    ///从文件流中加载
    bool WayChunk::load(Lazy::LZDataPtr stream)
    {
        debugMessage(_T("TRACE: WayChunk::load %d"), m_id);

        assert(stream && "WayChunk::load");
        m_waySets.clear();

        Lazy::LZDataBase::DataConstIterator it;
        FOR_EACH((*stream), it)
        {
            if((*it)->tag() != _T("set")) continue;

            m_waySets.push_back( (*it)->asInt() );
        }

        return true;
    }
        
    ///保存到文件流
    bool WayChunk::save(Lazy::LZDataPtr stream) const
    {
        assert(stream && "WayChunk::save");

        WpIdArray::const_iterator it;
        FOR_EACH(m_waySets, it)
        {
            Lazy::LZDataPtr ptr = stream->write(_T("set"));
            ptr->setInt(*it);
        }
        return true;
    }

    void WayChunk::render(LPDIRECT3DDEVICE9 pDevice) const
    {
        WpIdArray::const_iterator it;
        FOR_EACH(m_waySets, it)
        {
            WaySetPtr p = WaySetMgr::instance()->getWaySet(*it);
            if(p) p->render(pDevice);
        }
    }

    int WayChunk::numWayPoint() const 
    { 
        int n = 0;

        WpIdArray::const_iterator it;
        FOR_EACH(m_waySets, it)
        {
            WaySetPtr p = WaySetMgr::instance()->getWaySet(*it);
            if(p) n += p->numWp();
        }

        return n;
    }

    //返回way point id
    int WayChunk::findWpByPos(const Vector3 &pos) const
    {
        int id;
        findSetByPos(pos, &id);
        return id;
    }

    //返回waySetId
    int WayChunk::findSetByPos(const Vector3 &pos, int *wpId) const
    {
        int bestId = InvalidWp;
        int bestWpId = InvalidWp;
        float minHeight = MAX_FLOAT;

        WpIdArray::const_iterator it;
        FOR_EACH(m_waySets, it)
        {
            WaySetPtr p = WaySetMgr::instance()->getWaySet(*it);
            if(p && p->rect.isIn(pos.x, pos.z))
            {
                if(p->yMin - pos.y > FindWay::Config.clampHeight) continue;
//                if(pos.y - p->yMax > FindWay::Config.clampHeight) continue;

                float distance;
                int id = p->findWpByPos(pos, &distance);

                if(isInvalidWp(id)) continue;

                if(minHeight > distance)
                {
                    minHeight = distance;
                    bestId = p->id;
                    bestWpId = id;
                }
            }
        }

        if(wpId) *wpId = bestWpId;
        return bestId;
    }

    //在一个waySet中寻路
    bool WayChunk::findWay(WpPtrArray & way, const Vector3 & start, const Vector3 & end) const
    {
        int startId = findSetByPos(start);
        if(isInvalidWp(startId)) return false;

        int endId = findSetByPos(end);
        if(isInvalidWp(endId)) return false;

        if(startId != endId) return false;

        return findWay(way, startId, start, end);
    }
        
    //在一个waySet中寻路
    bool WayChunk::findWay(WpPtrArray & way, int waySetId, const Vector3 & start, const Vector3 & end) const
    {
        WaySetPtr pSet = WaySetMgr::instance()->getWaySet(waySetId);
        if(!pSet) return false;
        return pSet->findWay(way, start, end);
    }
        
    ///////////////////////////////////////////////////////////////////
    
    /*static*/ WayChunkMgr * WayChunkMgr::instance()
    {
        static WayChunkMgr s_mgr;
        return &s_mgr;
    }

    WayChunkMgr::WayChunkMgr()
        : m_rows(0), m_cols(0), m_x0(0), m_z0(0), m_chunkSize(0)
    {
        
    }

    WayChunkMgr::~WayChunkMgr()
    {

    }

    void WayChunkMgr::resetChunks(int n)
    {
        m_chunkPool.clear();
        for(int i=0; i<n; ++i)
        {
            m_chunkPool.push_back(new WayChunk(i));
        }
    }

    bool WayChunkMgr::load(const Lazy::tstring & path)
    {
        debugMessage(_T("TRACE: WayChunkMgr::load '%s'"), path.c_str());
        resetChunks(0);
        WaySetMgr::instance()->clear();

        m_path = path;
        Lazy::formatDirName(m_path);
        Lazy::tstring fname = m_path + _T("path.lzd");

        Lazy::LZDataPtr root = Lazy::openSection(fname);
        if(!root)
        {
            XWRITE_LOG(_T("ERROR: Load path data '%s' failed!"), fname.c_str());
            return false;
        }

        Lazy::LZDataPtr ptr = root->read(_T("basic"));
        m_rows = ptr->readInt(_T("rows"));
        m_cols = ptr->readInt(_T("cols"));
        m_x0 = ptr->readFloat(_T("x0"));
        m_z0 = ptr->readFloat(_T("z0"));
        m_chunkSize = ptr->readFloat(_T("chunkSize"));
        
        resetChunks(m_rows * m_cols);


        ptr = root->write(_T("chunk"));
        Lazy::tstring name;
        for(int r=0; r<m_rows; ++r)
        {
            for(int c=0; c<m_cols; ++c)
            {
                size_t key = r << 16 | c;
                formatString(name, _T("%8.8x"), key);

                Lazy::LZDataPtr pChunk = ptr->read(name);
                if(!pChunk)
                {
                    XWRITE_LOG(_T("ERROR: Load chunk data '%s' failed!"), name.c_str());
                    continue;
                }

                m_chunkPool[r * m_cols + c]->load(pChunk);
            }
        }

        if(!WaySetMgr::instance()->load(m_path))
        {
            return false;
        }
        return true;
    }

    bool WayChunkMgr::save(const Lazy::tstring & path) const
    {
        if (path.empty()) return false;

        if (!Lazy::getfs()->fileExist(path))
        {
            Lazy::tstring name = Lazy::getfs()->defaultPath() + path;

            LOG_INFO(L"Make dir %s", name.c_str());
            Lazy::getfs()->makeDirDeep(name);
        }

        Lazy::tstring fname = path;
        Lazy::formatDirName(fname);
        fname += _T("path.lzd");
        
        Lazy::LZDataPtr root = new Lazy::lzd();
        
        Lazy::LZDataPtr ptr = root->write(_T("basic"), Lazy::EmptyStr);

        ptr->writeInt(_T("rows"), m_rows);
        ptr->writeInt(_T("cols"), m_cols);
        ptr->writeFloat(_T("x0"), m_x0);
        ptr->writeFloat(_T("z0"), m_z0);
        ptr->writeFloat(_T("chunkSize"), m_chunkSize);

        ptr = root->write(_T("chunk"));

        Lazy::tstring name;
        for(int r=0; r<m_rows; ++r)
        {
            for(int c=0; c<m_cols; ++c)
            {
                size_t key = r << 16 | c;
                formatString(name, _T("%8.8x"), key);

                Lazy::LZDataPtr pChunk = ptr->write(name);
                m_chunkPool[r * m_cols + c]->save(pChunk);
            }
        }

        if(!root->save(fname))
        {
            XWRITE_LOG(_T("ERROR: Save path data '%s' failed!"), fname.c_str());
            return false;
        }

        if(!WaySetMgr::instance()->save(path)) return false;
        return true;
    }

    WayChunkPtr WayChunkMgr::getChunk(int id, bool urgent/*=true*/)
    {
        if(id<0 || id>=int(m_chunkPool.size())) return NULL;

        return m_chunkPool[id];
    }

    ///根据位置获取寻路chunk
    WayChunkPtr WayChunkMgr::getChunkByPos(float x, float z, bool urgent/*=true*/)
    {
        int r = int((m_z0 - z) / m_chunkSize);
        int c = int((x - m_x0) / m_chunkSize);

        if(r<0 || r>= m_rows || c<0 || c>=m_cols) return NULL;

        return getChunk(r * m_cols + c, urgent);
    }

    void WayChunkMgr::render(LPDIRECT3DDEVICE9 pDevice)
    {
        for (size_t i=0; i<m_chunkPool.size(); ++i)
        {
            WayChunkPtr ptr = m_chunkPool[i];
            if(ptr) ptr->render(pDevice);
        }
    }

    void WayChunkMgr::resetWayChunk(int rows, int cols, float x0, float z0, float size)
    {
        m_rows = rows;
        m_cols = cols;
        m_x0 = x0;
        m_z0 = z0;
        m_chunkSize = size;

        resetChunks(m_rows * m_cols);
    }

    int finWayBetweenSets(WpPtrArray & way, WaySetPtr pa, WaySetPtr pb, int startWpId, const Vector3 & end)
    {
        assert(pa && pb && isValidWp(startWpId) && "finWayIn2Set");

        int endWpId, oterWpId;
        if(!pa->findEdgeWp(end, startWpId, pb->id, endWpId, oterWpId)) return InvalidWp;

        //寻路
        if(!pa->findWayById(way, startWpId, endWpId)) return InvalidWp;
        
        return oterWpId;
    }

    bool WayChunkMgr::findWay(WpPtrArray & way, const Vector3 & start, const Vector3 & end)
    {
        //1.搜索起点/终点所在的chunk。
        //2.在chunk中搜索出所属wayset。
        //3.进行wayset级别的寻路。
        //4.在每个wayset内，进行waypoint级别寻路。

        WayChunkPtr startChunk = getChunkByPos(start.x, start.z);
        if(!startChunk) return false;

        WayChunkPtr endChunk = getChunkByPos(end.x, end.z);
        if(!endChunk) return false;

        //搜索起点wayset 和 waypoint
        int startWpId;
        int startSetId = startChunk->findSetByPos(start, &startWpId);
        if(isInvalidWp(startSetId)) return false;

        //搜索终点wayset 和 waypoint
        int endWpId;
        int endSetId = endChunk->findSetByPos(end, &endWpId);
        if(isInvalidWp(endSetId)) return false;

        //在wayset之间进行寻路
        WaySetArray waysets;
        if(!WaySetMgr::instance()->findWay(waysets, startSetId, endSetId)) return false;

        int n = int(waysets.size());
        debugMessage(_T("TRACE: find way between %d sets"), n);

        //在每个wayset内寻路
        for(int i=0; i<n-1; ++i)
        {
            startWpId = finWayBetweenSets(way, waysets[i], waysets[i+1], startWpId, end);
            if(isInvalidWp(startWpId)) return false;
        }

        return waysets[n-1]->findWayById(way, startWpId, endWpId);
    }


    ///路点转换成坐标
    void wayPointToPosition(Vector3Array & arr, const WpPtrArray & way)
    {
        size_t n = way.size();
        arr.resize(n);
        for(size_t i=0; i<n; ++i)
        {
            way[i]->getCenter( arr[n-i-1] );
        }
    }

    ///地图名称，转换成路点路径
    bool mapNameToWpPath(Lazy::tstring & wpPath, const Lazy::tstring & mapName)
    {
        //地图文件名格式为 path/mapName/map.lzd

        wpPath = Lazy::getFilePath(mapName);
        Lazy::removeFilePath(wpPath);
        if(wpPath.empty()) return false;

        wpPath = FindWay::WpPath + wpPath;
        return true;
    }
}

