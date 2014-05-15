#include "stdafx.h"

#include "Navigation.h"
#include "Core/App.h"

#include <queue>

float PI1_4 = D3DX_PI / 4.0f;
float PI2_3 = D3DX_PI * 2.0f / 3.0f;

const int DUp = 0;
const int DLeft = 1;
const int DRight = 2;
const int DDown = 3;

//四个方向：上 左 右 下
const int DIR[4][2] = {{-1, 0}, {0, -1}, {0, 1}, {1, 0}};

//反方向：下 右 左 上
const int DIR_INV_ID[] = {DDown, DRight, DLeft, DUp};

//另4个方向 左上 左下 右上 右下
const int DIR_OTHER[4][2] = {
    {DLeft, DUp}, 
    {DLeft, DDown}, 
    {DRight, DUp},
    {DRight, DDown},
};

namespace Physics
{
    Vector3 yNormal(0, 1.0f, 0);

    void makeRoleAABB(const NagenConfig & config, Physics::AABB & aabb, const Physics::Vector3 & pos)
    {
        float dt = config.cubeSize*0.5f;
        aabb.min = aabb.max = pos;
        aabb.min.x -= dt;
        aabb.min.z -= dt;
        aabb.max.x += dt;
        aabb.max.z += dt;
        aabb.min.y += config.modelHeight;
    }

    //////////////////////////////////////////////////////////////////////////
    
    WayPointData::WayPointData(int id_)
        : WayPoint(id_)
    {
    }

#if 0
    void WayPointData::addNeighbour(int id)
    { 
        for(int i=0; i<numAdj; ++i)
        {
            if(id == adjPoints[i]) return;
        }

        adjPoints[numAdj++] = id;
        assert(numAdj <= MaxWpAdj && "WayPoint::addNeighbour");
    }
#endif

    void WayPointData::connectFarNeigbour(WaySetPtr pSet)
    {
        for(int i=0; i<4; ++i)
        {
            const int d0 = DIR_OTHER[i][0];//左\右
            const int d1 = DIR_OTHER[i][1];//上\下

            //可以与4交的路点相连的条件是，交旁边的两个路点都必须存在。

            if(isInvalidWp(adjPoints[d0])) continue;
            if(isInvalidWp(adjPoints[d1])) continue;

            WayPointPtr ptr = pSet->getWayPoint(adjPoints[d1]);//用上\下去判断
            int farId = ptr->getNeighbour(d0);
            if(isInvalidWp(farId)) continue;

            adjPoints[4+i] = farId;
        }
    }

    ///删除无用邻接点
    void WayPointData::optimize()
    {
        numAdj = 0;
        for(int i=0; i<MaxWpAdj; ++i)
        {
            if(isValidWp(adjPoints[i]))
            {
                adjPoints[numAdj++] = adjPoints[i];
            }
        }
    }

    
    //////////////////////////////////////////////////////////////////////////
   
    WayPointDataPtr WaySetData::pushWayPoint()
    {
        int i = (int)m_wayPoints.size();
        WayPointDataPtr ptr = new WayPointData(i);
        m_wayPoints.push_back(ptr);
        return ptr;
    }

    void WaySetData::bindWayPoints()
    {
        assert(!m_wayPoints.empty() && "invalid wayset data.");
        
        rect = m_wayPoints.front()->rect;
//        yMax = MIN_FLOAT;
        yMin = MAX_FLOAT;

        WpPtrArray::iterator it;
        FOR_EACH(m_wayPoints, it)
        {
//            (*it)->setID = id;

            rect.expand((*it)->rect);
//            yMax = max(yMax, (*it)->yMax);
            yMin = min(yMin, (*it)->yMin);
        }
    }

    //////////////////////////////////////////////////////////////////////////

    HeightField::HeightField()
        : pNext(0)
    {
        MEMORY_CHECK_CONS;
    }

    HeightField::HeightField(CubeNode* node, float ymin, float ymax, HeightField* next)
        : pNode(node)
        , yMin(ymin)
        , yMax(ymax)
        , pNext(next)
        , mark(0)
        , wpId(InvalidWp)
        , setId(InvalidWp)
    {
        MEMORY_CHECK_CONS;
    }

    HeightField::~HeightField()
    {
        MEMORY_CHECK_DEST;
    }

    void HeightField::bindWp(WayPointPtr wp)
    {
        assert(wp && "bindWp");
        wp->rect = pNode->rect;
//        wp->yMax = yMax;
        wp->yMin = yMin;
        wpId = wp->id;
    }

    //////////////////////////////////////////////////////////////////////////

    CubeNode::CubeNode(int id_, const FRect & rc )
        : id(id_), rect(rc)
    {
        hfHead.yMax = hfHead.yMin = MIN_FLOAT;
    }

    CubeNode::~CubeNode()
    {
        HeightField* p = hfHead.pNext, *temp;
        while(p)
        {
            temp = p;
            p = p->pNext;

            delete temp;
        }
    }

    void CubeNode::render(LPDIRECT3DDEVICE9 pDevice)
    {
#if NAGEN_SHOW_TRIANGLE
        if (NagenDebug::showTriangle)
        {
            for (TriangleSet::iterator it=triangles.begin();
                it != triangles.end(); ++it)
            {
                drawTriangle(pDevice, *it, 0x7fffffff);
            }
        }
#endif
        if(NagenDebug::showLayer)
        {
            drawFRect(pDevice, rect, 0, 0xffff0000);
        }

        if(!NagenDebug::showHF) return;

        AABB aabb;
        HeightField* p = hfHead.pNext;
        while(p)
        {
            aabb.min.set(rect.left, p->yMin, rect.top);
            aabb.max.set(rect.right, p->yMax, rect.bottom);

            drawAABB(pDevice, aabb, 0xffff0000);
            p = p->pNext;
        }
    }

    void CubeNode::addTriangle(const Triangle & tri)
    {
        //将三角形裁剪到结点区域内
        Math::Polygon poly(tri);
        if(!poly.choiceFront(Plane(Vector3(1, 0, 0), rect.left))) return;
        if(!poly.choiceFront(Plane(Vector3(-1, 0, 0), -rect.right))) return;
        if(!poly.choiceFront(Plane(Vector3(0, 0, 1), rect.top))) return;
        if(!poly.choiceFront(Plane(Vector3(0, 0, -1), -rect.bottom))) return;

#if NAGEN_SHOW_TRIANGLE
        triangles.push_back(tri);
#endif

        //求出最低点和最高点y值，生成一个高度区间。
        float yMin = MAX_FLOAT;
        float yMax = MIN_FLOAT;
        for(size_t i=0; i<poly.size(); ++i)
        {
            yMin = min(poly[i].y, yMin);
            yMax = max(poly[i].y, yMax);
        }

        //合并高度域
        HeightField* prev = &hfHead;
        HeightField* p = prev->pNext;
        
        if(!p)
        {
            prev->pNext = new HeightField(this, yMin, yMax, p);
            return;
        }
        
        while(p)
        {
            //介于prev与p区间之间，则添加进去
            if(yMin > prev->yMax && yMax < p->yMin)
            {
                prev->pNext = new HeightField(this, yMin, yMax, p);
                break;
            }
            //位于p区间内，则忽略
            else if(yMin >= p->yMin && yMax<=p->yMax)
            {
                break;
            }
            //包含p区间，则将p区间删除
            else if(yMin < p->yMin && yMax> p->yMax)
            {
                prev->pNext = p->pNext;
                delete p;
                p = prev;
            }
            //p区间包含yMin，修改p的上限
            else if(yMin >= p->yMin && yMin <= p->yMax)
            {
                p->yMax = max(p->yMax, yMax);

                //向上合并重叠的区域
                prev = p;
                p = p->pNext;
                while(p)
                {
                    if(prev->yMax >= p->yMin)
                    {
                        prev->yMax = max(prev->yMax, p->yMax);//合并yMax

                        //删除冗余的结点p
                        prev->pNext = p->pNext;
                        delete p;

                        p = prev;
                    }

                    prev = p;
                    p = p->pNext;
                }

                break;
            }
            //p区间包含yMax，修改p的下限
            else if(yMax >= p->yMin && yMax <= p->yMax)
            {
                p->yMin = min(p->yMin, yMin);
                break;
            }

            prev = p;
            p = prev->pNext;
        }
    }

    void CubeNode::inverseHeightField()
    {
        //将障碍区域反转为通行区域
        HeightField *p = hfHead.pNext;
        while(p)
        {
            p->yMin = p->yMax;
            if(p->pNext)
            {
                p->yMax = p->pNext->yMin;
            }
            else
            {
                p->yMax = p->yMin + FindWay::Config.modelHeight*2;//防止误差。
            }

            p = p->pNext;
        }

        //删除过矮的区域
        HeightField *prev = &hfHead;
        p = prev->pNext;
        while(p)
        {
            if(p->yMax - p->yMin < FindWay::Config.modelHeight)
            {
                prev->pNext = p->pNext;
                delete p;
                p = prev;
            }
            p = p->pNext;
        }
    }

    /*  搜索符合攀爬的高度域。分为两种：向上爬，和向下走
     *  向上爬：要求yMin小于高度域的yMin，且差值不超过可爬升高度，并且yMax与高度域yMin之差大于模型高度；
     *  向下爬：与向上爬相反。yMin大于高度域的yMin。
     */
    HeightField* CubeNode::searchHF(float yMin, float yMax)
    {
        //注：p->yMax必定大于p->yMin。

        HeightField *p = hfHead.pNext;
        while(p)
        {
            if(yMin >= p->yMin)//向下走
            {
                if(yMin - p->yMin <= FindWay::Config.clampHeight &&
                    yMax - p->yMin >= FindWay::Config.modelHeight)
                {
                    return p;
                }
            }
            else if(yMin <= p->yMin)//向上爬
            {
                if(p->yMin - yMin <= FindWay::Config.clampHeight &&
                    p->yMax - yMin >= FindWay::Config.modelHeight)
                {
                    return p;
                }
            }

            p = p->pNext;
        }

        return 0;
    }

    void CubeNode::linkWithOtherNode(CubeNode * pOther)
    {
        assert(pOther);

        HeightField *p = hfHead.pNext;
        while(p)
        {
            if(isValidWp(p->setId))
            {
                HeightField *pDest = pOther->searchHF(p->yMin, p->yMax);
                if(pDest && isValidWp(pDest->setId))
                {
    #if 1
                    WaySetPtr pSet1 = WaySetMgr::instance()->getWaySet(p->setId);
                    WaySetPtr pSet2 = WaySetMgr::instance()->getWaySet(pDest->setId);

                    assert(pSet1 && pSet2 && "linkWithOtherNode");

                    WpLinker linker;
                    linker.wp = p->wpId;
                    linker.destWp = pDest->wpId;
                    pSet1->addEdgeLinker(pSet2->id, linker);

                    std::swap(linker.wp, linker.destWp);
                    pSet2->addEdgeLinker(pSet1->id, linker);
    #endif
                }
            }
            p = p->pNext;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    ChunkNode::ChunkNode(int id, const FRect & rc)
        : chunkID(id)
        , rect(rc)
        , loaded(false)
        , finished(false)
    {
        MEMORY_CHECK_CONS;

        for(int i=0; i<4; ++i) linkState[i] = false;
    }

    ChunkNode::~ChunkNode()
    {
        clear();
        MEMORY_CHECK_DEST;
    }
    
    void ChunkNode::doLoad()
    {
        if(loaded) return;
        loaded = true;
        
        debugMessage(_T("TRACE: load ChunkNode %d"), chunkID);

        rows = int(std::ceil(rect.height() / FindWay::Config.cubeSize));
        cols = int(std::ceil(rect.width() / FindWay::Config.cubeSize));

        int n = rows * cols;
        cubeNodes.reserve(n);

        for (int r=0; r<rows; ++r)
        {
            for (int c=0; c<cols; ++c)
            {
                FRect rc;
                rc.left = rect.left + c * FindWay::Config.cubeSize;
                rc.top = rect.top + r * FindWay::Config.cubeSize;
                rc.right = rc.left + FindWay::Config.cubeSize;
                rc.bottom = rc.top + FindWay::Config.cubeSize;

                cubeNodes.push_back(CubeNode(r*cols + c, rc));
            }
        }
    }

    void ChunkNode::addTriangle(const Triangle & tri)
    {
        FRect rc;
        rc.left = rc.right = tri.a.x;
        rc.top = rc.bottom = tri.a.z;

        for (int i=0; i<3; ++i)
        {
            rc.expand(tri[i].x, tri[i].z);
        }

        if(!rect.isIntersect(rc)) return;

        if(!loaded) doLoad();

        int left = int((rc.left - rect.left) / FindWay::Config.cubeSize);
        int top = int((rc.top - rect.top) / FindWay::Config.cubeSize);
        int right = int((rc.right - rect.left) / FindWay::Config.cubeSize);
        int bottom = int((rc.bottom - rect.top) / FindWay::Config.cubeSize);
        
        for (int r=top; r<=bottom; ++r)
        {
            for (int c=left; c<=right; ++c)
            {
                CubeNode *pNode = getNode(r, c);
                if (pNode)
                {
                    pNode->addTriangle(tri);
                }
            }
        }
    }

    void ChunkNode::addMesh(LPD3DXMESH mesh, const Matrix4x4 * pWorld)
    {
        if(!mesh) return;

        MeshExtractor extractor;
        if(!extractor.build(mesh)) return;

        for(size_t i=0; i<extractor.nbTriangle(); ++i)
        {
            if(pWorld)
            {
                Triangle tri = extractor.getTriangle(i);
                tri.applyMatrix(*pWorld);
                addTriangle(tri);
            }
            else
            {
                addTriangle(extractor.getTriangle(i));
            }
        }
    }

    void ChunkNode::clear()
    {
        cubeNodes.clear();
    }

    void ChunkNode::render(LPDIRECT3DDEVICE9 pDevice)
    {
        for (std::vector<CubeNode>::iterator it = cubeNodes.begin();
            it != cubeNodes.end(); ++it)
        {
            it->render(pDevice);
        }

        if (NagenDebug::showLayer)
        {
            drawFRect(pDevice, rect, 0);
        }
    }

    CubeNode* ChunkNode::getNode(int r, int c)
    {
        if(r>=0 && r<rows && c>=0 && c<cols)
            return &cubeNodes[r*cols + c];
        return NULL;
    }

    //生成路点
    void ChunkNode::genWayPoint()
    {
        if(finished)
        {
            debugMessage(_T("WARNNING: The chunk(%d) has been generated. Just skip it."), chunkID);
            return;
        }

        if(!loaded)
        {
            finished = true;
            debugMessage(_T("WARNNING: The chunk(%d) is empty. Just skip it."), chunkID);
            return;
        }
        finished = true;

        //反转高度域
        const int len = int(cubeNodes.size());
        for (int i=0; i<len; ++i)
        {
            cubeNodes[i].inverseHeightField();
        }

        //对每个子结点，进行路点计算
        for (int i=0; i<len; ++i)
        {
            CubeNode *pNode = &cubeNodes[i];

            HeightField *p = pNode->hfHead.pNext;
            while(p)
            {
                genWayPoint(p);
                p = p->pNext;
            }
        }
        
        WayChunkPtr waychunk = WayChunkMgr::instance()->getChunk(chunkID);
        debugMessage(_T("TRACE: ChunkNode %d genWayPoint finished. waySet num:%d, wayPoint num:%d"),
            chunkID, waychunk->numWaySet(), waychunk->numWayPoint());
    }
    
    //使用广度优先搜索生成路点
    void ChunkNode::genWayPoint(HeightField* pNode)
    {
        assert(pNode && "genWayPoint");

        //此结点已经计算过了
        if(pNode->mark != 0) return;

        WayChunkPtr waychunk = WayChunkMgr::instance()->getChunk(chunkID);

        //新的路点也将隶属于新的路点集合
        WaySetData* pWaySet = new WaySetData(0, waychunk->getId());

        std::queue<HeightField*> cubeQueue;
        std::vector<HeightField*> hfCache;

        pNode->bindWp(pWaySet->pushWayPoint());
        pNode->mark = 1;
        cubeQueue.push(pNode);

        pNode = NULL;

        while(!cubeQueue.empty())
        {
            HeightField *pHF = cubeQueue.front();
            cubeQueue.pop();
            hfCache.push_back(pHF);
            
            WayPointDataPtr pWayPoint = pWaySet->getWayPoint(pHF->wpId);

            //遍历周围4个方向
            int r = pHF->pNode->id / cols;
            int c = pHF->pNode->id % cols;
            for (int d=0; d<4; ++d)
            {
                int dr = DIR[d][0] + r;
                int dc = DIR[d][1] + c;

                CubeNode* adjNode = getNode(dr, dc);
                if(!adjNode) continue;

                HeightField* pAdjHF = adjNode->searchHF(pHF->yMin, pHF->yMax);
                if(NULL == pAdjHF) continue;

                WayPointDataPtr pWp;
                if(pAdjHF->mark == 0)//没有访问过
                {
                    pWp = pWaySet->pushWayPoint();
                    pAdjHF->bindWp(pWp);
                    pAdjHF->mark = 1;
                    cubeQueue.push(pAdjHF);
                }
                else//已经访问过了，在此处需要修改邻接点
                {
                    pWp = pWaySet->getWayPoint(pAdjHF->wpId);
                }

                //连接邻接信息
                pWayPoint->setNeighbour(d, pWp->id);
                pWp->setNeighbour(DIR_INV_ID[d], pWayPoint->id);
            }
        }

        if(pWaySet->numWp() < FindWay::Config.minSetPoints)
        {
            delete pWaySet;
            return;
        }

        WaySetMgr::instance()->addWaySet(pWaySet);
        pWaySet->bindWayPoints();
        waychunk->addWaySet(pWaySet->id);

        for(size_t i=0; i<hfCache.size(); ++i)
        {
            hfCache[i]->setId = pWaySet->id;
        }

        for(int i=0; i<pWaySet->numWp(); ++i)
        {
            WayPointDataPtr ptr = pWaySet->getData(i);
            ptr->connectFarNeigbour(pWaySet);
        }

        //两个循环不能合并
        for(int i=0; i<pWaySet->numWp(); ++i)
        {
            WayPointDataPtr ptr = pWaySet->getData(i);
            ptr->optimize();
        }
    }

    void ChunkNode::linkWithChunk(int dir, ChunkNodePtr otherNode)
    {
        XWRITE_LOG(_T("TRACE: Link way chunk %d and %d"), chunkID, otherNode->id());
        
        setLinkState(dir, true);
        otherNode->setLinkState(DIR_INV_ID[dir], true);
        
        if(dir == DUp)//要注意y方向跟常规思路，是相反的。
        {
            int base = (rows - 1) * cols;
            for(int i=0; i<cols; ++i)
            {
                cubeNodes[base+i].linkWithOtherNode(otherNode->getNode(i));
            }
        }
        else if(dir == DDown)
        {
            int base = (rows - 1) * cols;
            for(int i=0; i<cols; ++i)
            {
                cubeNodes[i].linkWithOtherNode(otherNode->getNode(base+i));
            }
        }
        else if(dir == DLeft)
        {
            for(int i=0; i<rows; ++i)
            {
                cubeNodes[i*cols].linkWithOtherNode(otherNode->getNode((i+1)*cols - 1));
            }
        }
        else if(dir == DRight)
        {
            for(int i=0; i<rows; ++i)
            {
                cubeNodes[(i+1)*cols - 1].linkWithOtherNode(otherNode->getNode(i*cols));
            }
        }
    }

    ///////////////////////////////////////////////////////////////////
    /*static*/ ChunkNodeMgr* ChunkNodeMgr::instance()
    {
        static ChunkNodeMgr g_mgr;
        return &g_mgr;
    }

    ChunkNodeMgr::ChunkNodeMgr()
    {
    }

    ChunkNodeMgr::~ChunkNodeMgr()
    {
    }

    void ChunkNodeMgr::render(LPDIRECT3DDEVICE9 pDevice) 
    {
    }

    ///加载地图
    bool ChunkNodeMgr::doLoadConfig()
    {
        //清除数据
        m_nodes.clear();
        WaySetMgr::instance()->clear();

        TerrainMap* pMap = TerrainMap::instance();
        if(!pMap->isUserfull())
        {
            throw(std::runtime_error("Current map is invalid!"));
        }

        const std::wstring & mapName = pMap->getMapName();

        if(!mapNameToWpPath(m_wayPath, mapName))
        {
            throw(std::runtime_error("The name of current map is invalid!"));
        }

        m_rows = pMap->nodeRows();
        m_cols = pMap->nodeCols();

        //重置寻路管理器
        WayChunkMgr::instance()->resetWayChunk(m_rows, m_cols, 
            pMap->xMin(), pMap->zMax(), pMap->nodeSize());

        //构造寻路数据
        int id = 0;
        for (TerrainMap::NodeIterator it = TerrainMap::instance()->begin();
            it != TerrainMap::instance()->end(); ++it)
        {
            m_nodes.push_back( new ChunkNode(id, (*it)->getRect()));
            ++id;
        }
        
        return true;
    }

    ///生成路点
    bool ChunkNodeMgr::doGenerate()
    {
        //TODO 拆分此方法，可以分别计算每一个chunk。

        if (!TerrainMap::instance()->isAllChunkLoaded())
        {
            LOG_ERROR(L"Must load all chunk first!");
            return false;
        }

        //加载地图配置
        doLoadConfig();

        std::queue<ChunkPtr> nodeQueue;
        nodeQueue.push(TerrainMap::instance()->getNode(m_rows/2, m_cols/2));

        Physics::Matrix4x4 matWorld;
        ChunkPtr pMapNode;

        int testN = 0;
        bool result = true;

        const std::wstring & mapName = TerrainMap::instance()->getMapName();

        while(!nodeQueue.empty())
        {
            pMapNode = nodeQueue.front();
            nodeQueue.pop();

            assert(pMapNode && "MapNodePtr must not be null!");

            const int rowId = pMapNode->rowID();
            const int colId = pMapNode->colID();

            XWRITE_LOG(_T("TRACE: Generate Chunk (%d, %d) way points."), rowId, colId);

            pMapNode->load();
            ChunkNodePtr chunk = getChunk(rowId, colId);

            if(chunk->isGenFinished())
            {
                XWRITE_LOG(_T("WARNNING: The chunk(%d, %d) has been generated!"), rowId, colId);
                continue;
            }

            //1.准备三角形

            //添加高度图mesh
            chunk->addMesh(pMapNode->getTerrainMesh());

#if 1
            //添加每个地图物件的模型
            ObjectIter it;
            FOR_EACH((*pMapNode), it)
            {
                (*it)->getWorldMatrix(matWorld);
                ModelPtr model = (*it)->getModel();
                if(model) chunk->addMesh(model->getMesh(), &matWorld);
            }
#endif

            //2.计算路点
            chunk->genWayPoint();

            //3.连接4个邻接chunk
            for(int i=0; i<4; ++i)
            {
                int dr = rowId + DIR[i][0];
                int dc = colId + DIR[i][1];

                if(dr < 0 || dr>=m_rows || dc<0 || dc>=m_cols)//不存在这个邻居
                {
                    chunk->setLinkState(i, true);
                    continue;
                }

                //关系已建立
                if(chunk->isLinked(i)) continue;

                ChunkNodePtr neighbour = getChunk(dr, dc);
                if(neighbour->isGenFinished())
                {
                    chunk->linkWithChunk(i, neighbour);
                }
                else
                {
                    nodeQueue.push(TerrainMap::instance()->getNode(dr, dc));
                }
            }

            //释放空间，减少内存消耗。
            if(chunk->isAllFinished())
            {
                chunk->clear();

                XWRITE_LOG(_T("TRACE: Finished Generate Chunk (%d, %d) way points."),
                    pMapNode->rowID(), pMapNode->colID());
            }

            ++testN;
            if(!getApp()->processMessage())
            {
                PostQuitMessage(0);
                result = false;
                break;
            }

            int percent = int(testN * 100.0f / numChunks());

            std::wstring caption;
            formatString(caption, L"Navigation(process '%s' %d%%)", mapName.c_str(), percent);
            getApp()->setCaption(caption);

            //if(testN == 1) break;
        }

        getApp()->setCaption(L"Navigation");
        return result;
    }

    ///保存路点数据
    bool ChunkNodeMgr::doSave(const Lazy::tstring & path/*=Lazy::EmptyStr*/) const
    {
        Lazy::tstring name = path;
        if(name.empty())
        {
            name = m_wayPath;
        }

        return WayChunkMgr::instance()->save(name);
    }

}