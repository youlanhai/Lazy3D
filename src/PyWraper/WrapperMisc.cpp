
#include "stdafx.h"

#include "..\Physics\LazyConllision.h"
#include "WrapperMisc.h"
#include "WrapperEntity.h"


namespace
{

    ///后台加载任务
    class TaskWithPyFun : public Lazy::TaskInterface
    {
    public:
        TaskWithPyFun(object pWork, object pFinish, int workID)
            : m_pWork(pWork), m_pFinish(pFinish), m_workID(workID)
        {
        }

        virtual void doTask(void) override
        {
            if(!m_pWork.is_none())
            {
                m_pWork();
            }
        }

        virtual void onTaskFinish(void) override
        {
            if(!m_pFinish.is_none())
            {
                m_pFinish();
            }
        }

    private:
        object  m_pWork;
        object  m_pFinish;
        int     m_workID;
    };

    ///添加后台加载任务
    void addLoadingTask(object work, object finish)
    {
        bool immediate = false;

        int workID = 0;
        if (!immediate)
        {
            //延迟一帧再执行。防止后台线程立即执行时，造成资源访问冲突。
            workID = Lazy::LoadingMgr::instance()->getDispatchStep();
        }
        Lazy::LoadingMgr::instance()->addTask(new TaskWithPyFun(work, finish, workID));
    }


    //////////////////////////////////////////////////////////////////////////

    SoundPtr loadSound(LPCSTR szName)
    {
        return cSoundMgr::instance()->loadSound(szName);
    }

    SoundPtr playSound(LPCSTR szName)
    {
        SoundPtr pSound = loadSound(szName);
        if (pSound)
        {
            pSound->play();
        }
        return pSound;
    }

    void debugMsg(LPCSTR msg)
    {
        OutputDebugStringA(msg);
    }

    void writeLog(const std::string & msg)
    {
        WRITE_LOGA(msg);
    }

    int getWheelDelta(WPARAM wParam)
    {
        return GET_WHEEL_DELTA_WPARAM(wParam);
    }

    void setAmbientLight(DWORD color)
    {
        getApp()->getDevice()->SetRenderState(D3DRS_AMBIENT, color);
    }

    void setDirectionLight(const Math::Vector3 & dir, float a, float r, float g, float b)
    {
        //设置灯光
        LPDIRECT3DDEVICE9 device = getApp()->getDevice();
        CLight light;
        light.SetDirectionLight1(device, dir, D3DXCOLOR(r, g, b, a));
    }

    const D3DFILLMODE FillMap[] =  {D3DFILL_POINT, D3DFILL_WIREFRAME, D3DFILL_SOLID};
    const int MaxFillMode = 3;

    int getFillMode()
    {
        DWORD mode;
        getApp()->getDevice()->GetRenderState(D3DRS_FILLMODE, &mode);
        for (int i=0; i<MaxFillMode; ++i)
        {
            if(mode == FillMap[i]) return i;
        }
        return  0;
    }

    void setFillMode(int mode)
    {
        if(mode < 0 || mode >= MaxFillMode) return;
        getApp()->getDevice()->SetRenderState(D3DRS_FILLMODE, FillMap[mode]);
    }


    void setTopBoardSource(cTopBoard * board, PyEntity & entity)
    {
        board->setSource(entity.m_entity.get());
    }

    void setMapSource(TerrainMap * map, PyEntity & entity)
    {
        map->setSource(entity.m_entity.get());
    }

    void setSnowSource(cSnow * snow, PyEntity & entity)
    {
        snow->setSource(entity.m_entity.get());
    }


    void setSkySource(cSkyBox * sky, PyEntity & entity)
    {
        sky->setSource(entity.m_entity.get());
    }

    ///////////////////////////////////////////////////////////////////
    void showChunkOctree(bool show){ MapConfig::ShowChunkOctree = show; }
    bool isChunkOctreeVisible(){ return MapConfig::ShowChunkOctree; }

    void enableSelectItem(bool enalbe){ MapConfig::CanSelectItem = enalbe; }
    bool canSelectItem(){ return MapConfig::CanSelectItem; }

    void showAllItemAABB(bool show){ MapConfig::ShowAllItemAABB = show; }
    bool isAllItemAABBVisible(){ return MapConfig::ShowAllItemAABB; }

}


void wrapperMisc()
{
    LOG_INFO(L"EXPORT - wrapperMisc");

    auto copyConstRef = return_value_policy<copy_const_reference>();
    auto exsitingRef = return_value_policy<reference_existing_object>();

    def("addLoadingTask", addLoadingTask);
    def("setAmbientLight", setAmbientLight);
    def("setDirectionLight", setDirectionLight);
    def("getOCTreeCacheBytes", Physics::getOCTreeCacheBytes);

    //常用API
    def("WRITE_LOG", writeLog);
    def("debug", debugMsg);

    def("getApp", getApp, exsitingRef);
    def("getResMgr", getResMgr, exsitingRef);
    def("getCamera", getCamera, exsitingRef);
    def("getMap", TerrainMap::instance, exsitingRef);
    def("getPick", getPick, exsitingRef);
    def("getWheelDelta", getWheelDelta);

    def("getFillMode", getFillMode);
    def("setFillMode", setFillMode);

    def("showChunkOctree", showChunkOctree);
    def("isChunkOctreeVisible", isChunkOctreeVisible);

    def("enableSelectItem", enableSelectItem);
    def("canSelectItem", canSelectItem);

    def("showAllItemAABB", showAllItemAABB);
    def("isAllItemAABBVisible", isAllItemAABBVisible);

    def("isEntitySphereEnable", isEntitySphereEnable);
    def("setEntitySphereEnable", setEntitySphereEnable);

    def("getMaxDistToNpc", getMaxDistToNpc);
    def("setMaxDistToNpc", setMaxDistToNpc);

    def("enableDrawCollision", enableDrawCollision);
    def("isDrawCollisionEnable", isDrawCollisionEnable);

    def("loadSound", loadSound);
    def("playSound", playSound);


    class_<CApp>("CApp", no_init)
        .add_property("width", &CApp::getWidth)
        .add_property("height", &CApp::getHeight)
        .add_property("caption", make_function(&CApp::getCaption, copyConstRef), &CApp::setCaption)
        .def("getResMgr", &CApp::getResMgr, exsitingRef)
        .def("getSkyBox", &CApp::getSkyBox)
        .def("getKeyboard", &CApp::getKeyboard, exsitingRef)
        .def("quitGame", &CApp::quitGame)
        .def("getCursorPos", &CApp::getCursorPos)
        .def("startGameLoading", &CApp::startGameLoading)
        .def("stopGameLoading", &CApp::stopGameLoading)
        .def("getTimeScale", &CApp::getTimeScale)
        .def("setTimeScale", &CApp::setTimeScale)
        ;

    //常用类
    class_<IRenderRes, boost::noncopyable>("iRenderRes", no_init)
        .def("load", pure_virtual(&IRenderRes::load))
        .def("getType", pure_virtual(&IRenderRes::getType))
        ;

    class_<cResMgr>("ResMgr", no_init)
        .def("init", &cResMgr::init)
        .def("get", &cResMgr::get)
        .def("remove", &cResMgr::remove)
        ;

    class_<cPick>("cPick", init<>())
        .def("handleMouseEvent", &cPick::handleMouseEvent)
        .def("isIntersectWithTerrain", &cPick::isIntersectWithTerrain)
        .def("getPosOnTerrain", &cPick::getPosOnTerrain, copyConstRef)
        .def("isTerrainObjEnable", &cPick::isTerrainObjEnable)
        .def("setTerrainObjEnable", &cPick::setTerrainObjEnable)
        ;


    //公告板

    class_<cBillboard>("Billboard", init<>())
        .def("setSize", &cBillboard::setSize)
        .def("setImage", &cBillboard::setImage)
        .def("show", &cBillboard::show)
        .def("visible", &cBillboard::visible)
        .add_property("maxShowDistance", &cBillboard::getShowMaxDistance, &cBillboard::setMaxShowDistance)
        ;

    class_<cTopBoard, bases<cBillboard>>("TopBoard")
        .def("create", &cTopBoard::create)
        .add_property("text", make_function(&cTopBoard::getText, copyConstRef), &cTopBoard::setText)
        .add_property("color", &cTopBoard::getColor, &cTopBoard::setColor)
        .add_property("biasHeight", &cTopBoard::getBiasHeight, &cTopBoard::setBiasHeight)
        .def("setSource", &setTopBoardSource)
        ;

    //粒子系统
    class_<cParticleSystem>("cParticleSystem", init<int, const std::wstring & >())
        .def("isDead", &cParticleSystem::isDead)
        .def("reset", &cParticleSystem::reset)
        ;

    class_<cSnow, bases<cParticleSystem>>("cSnow", init<int, const std::wstring &, I3DObject*>())
        .def("setSource", &setSnowSource)
        .def("setRange", &cSnow::setRange)
        .def("show", &cSnow::show)
        .def("visible", &cSnow::visible)
        ;


    object sky_image = make_function(&cSkyBox::getSkyImage, return_value_policy<copy_const_reference>());
    class_<cSkyBox, RefPtr<cSkyBox>, boost::noncopyable>("SkyBox", no_init)
        .def("show", &cSkyBox::show)
        .def("visible", &cSkyBox::visible)
        .def("toggle", &cSkyBox::toggle)
        .def("setSkyRange", &cSkyBox::setSkyRange)
        .add_property("image", sky_image, &cSkyBox::setSkyImage)
        .def("setSource", &setSkySource)
        ;


    //地形
    class_<TerrainMap>("TerrainMap", no_init)
        .def("loadMap", &TerrainMap::loadMap)
        .def("setSource", &setMapSource)
        .def("isInBound", &TerrainMap::isInBound)
        .def("getHeight", &TerrainMap::getHeight)
        .def("setShowLevel", &TerrainMap::setShowLevel)
        .def("getShowLevel", &TerrainMap::getShowLevel)
        .def("xMin", &TerrainMap::xMin)
        .def("xMax", &TerrainMap::xMax)
        .def("zMin", &TerrainMap::zMin)
        .def("zMax", &TerrainMap::zMax)
        .def("width", &TerrainMap::width)
        .def("height", &TerrainMap::height)
        .def("handeMouseEvent", &TerrainMap::handeMouseEvent)
        ;

    class_<TerrainResNode>("TerrainResNode", init<>())
        .def_readwrite("id", &TerrainResNode::id)
        .def_readwrite("type", &TerrainResNode::type)
        .def_readwrite("path", &TerrainResNode::path)
        ;

    typedef std::vector<TerrainResNode> TRNVector;
    class_<TRNVector>("TRNVector")
        .def(vector_indexing_suite< TRNVector > ())
        ;

    class_<cTerrainRes, boost::noncopyable>("cTerrainRes", no_init)
        .def("getResPool", &cTerrainRes::getResPool, exsitingRef)
        .def("getType", &cTerrainRes::getType)
        .def("getPath", &cTerrainRes::getPath, copyConstRef)
        ;

    LOG_INFO(L"EXPORT - wrapperMisc END.");
}
