
#include "stdafx.h"
#include "WrapperEntity.h"

using namespace Physics;

bool hasatter(object o, const char * attr)
{
    return PyObject_HasAttrString(o.ptr(), attr);
}

template<typename T>
bool hasinstance(object o)
{
    type_handle type = objects::registered_class_object(typeid(T));
    return PyObject_IsInstance(o.ptr(), (PyObject*) (type.get()));
}

template<typename T>
bool hassubclass(object o)
{
    type_handle type = objects::registered_class_object(typeid(T));
    return PyObject_IsSubClass(o.ptr(), (PyObject*) (type.get()));
}



class WraperIPhysics : public iPhysics
{
    object m_self;
public:

    WraperIPhysics()
    {

    }

    void bindPython(object self)
    {
        m_self = self;
    }

    void onMoveToFinished(bool succed) override
    {
        if (hasatter(m_self, "onMoveToFinished"))
            m_self.attr("onMoveToFinished")(succed);
    }

    void onMoveToEntityFinish(bool succed) override
    {
        if (object fun = m_self.attr("onMoveToEntityFinish"))
            fun(succed);
    }
    void onStateChange(DWORD oldState) override
    {
        if (object fun = getattr(m_self, "onStateChange"))
            fun(oldState);
    }

    void onAITrigger() override
    {
        if (hasatter(m_self, "onAITrigger"))
            m_self.attr("onAITrigger")();
    }

};

class PyPhysics
{
public:
    RefPtr<WraperIPhysics> m_physics;

    PyPhysics(object self)
    {
        if (!hasinstance<PyPhysics>(self))
        {
            PyErr_SetString(PyExc_TypeError, "Physics type needed!");
            throw_error_already_set();
        }

#if 0
        reference_existing_object::apply<PyPhysics*>::type converter;
        PyObject* obj = converter(this);
        object self = object(handle<>(obj));
#endif

        m_physics = new WraperIPhysics();
        m_physics->bindPython(self);
    }

    void enable(bool v){ m_physics->enable(v); }
    bool isEnabled(){ return m_physics->isEnabled(); }

    bool searchPath(const Vector3 & dest){ return m_physics->searchPath(dest); }
    void faceTo(const Vector3 & dest){ m_physics->faceTo(dest); }
    void faceToDir(const Vector3 & dest){ m_physics->faceToDir(dest); }
    void moveTo(const Vector3 & dest){ m_physics->moveTo(dest); }
    void moveToEntity(PyEntity * ent){ m_physics->moveToEntity(ent->m_entity.get()); }

    void breakAutoMove(){ m_physics->breakAutoMove(); }

    void enableAI(bool v){ m_physics->enableAI(v); }
    bool isAIEnable(){ return m_physics->isAIEnable(); }

    void setAIInterval(float v){ m_physics->setAIInterval(v); }
    float getAIInterval(){ return m_physics->isAIEnable(); }

    void setState(DWORD v){ m_physics->setState(v); }
    DWORD getState(){ return m_physics->getState(); }

};

//////////////////////////////////////////////////////////////////////////

class WrapperIEntity : public iEntity
{
    object m_self;

public:

    WrapperIEntity()
    {}
    
    void bindPython(object self)
    {
        m_self = self;
    }

    virtual bool canSelect(void) const override
    {
        if (object func = m_self.attr("canSelect"))
            return func();

        return iEntity::canSelect();
    }

    virtual bool isPlayer(void) override
    {
        if (object func = m_self.attr("isPlayer"))
            return func();
        
        return iEntity::isPlayer();
    }

    void onFocusCursor(UINT msg) override
    {
        if (object func = m_self.attr("onFocusCursor"))
        {
            func(msg);
            return;
        }
        
        iEntity::onFocusCursor(msg);
    }
};


PyEntity::PyEntity()
{
    m_entity = new WrapperIEntity();
}

void PyEntity::setPhysics(object physics)
{
    if (physics.is_none())
    {
        m_entity->setPhysics(nullptr);
        m_physics = physics;
    }
    else if (extract<PyPhysics&>(physics).check())
    {
        PyPhysics & x = extract<PyPhysics&>(physics);
        m_entity->setPhysics(x.m_physics);
        m_physics = physics;
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "Physics type needed.");
        throw_error_already_set();
    }
}


class PyEntityMgr
{
public:
    dict m_dict;
    object m_player;

    object entity(int id)
    {
        return m_dict.get(id);
    }

    object entities()
    {
        return m_dict.copy();
    }

    int createEntity(object cls, dict attr)
    {
        //类类型检查
        type_handle EntityCls = objects::registered_class_object(typeid(PyEntity));
        if (!PyObject_IsSubclass(cls.ptr(), (PyObject*)(EntityCls.get())))
        {
            PyErr_SetString(PyExc_TypeError, "Entity Class Type needed!");
            throw_error_already_set();
        }

        //实例化对象
        object o = cls();
        
        //添加引用
        int id = extract<int>( o.attr("id") );
        m_dict[id] = o;

        PyEntity & entity = extract<PyEntity &>(o);
        EntityMgr::instance()->add(entity.m_entity);
        entity.m_entity.to<WrapperIEntity>()->bindPython(o);

        //设置附加属性
        PyObject *key, *value;
        Py_ssize_t pos = 0;

        while (PyDict_Next(attr.ptr(), &pos, &key, &value))
        {
            Py_INCREF(key);
            Py_INCREF(value);
            PyObject_SetAttr(o.ptr(), key, value);
        }

        //enterWorld
        object enterWorld = o.attr("enterWorld");
        enterWorld();

        return id;
    }

    void destroyEntity(object o)
    {
        //类型检查
        if (!extract<PyEntity&>(o).check())
        {
            PyErr_SetString(PyExc_TypeError, "Entity Type needed!");
            throw_error_already_set();
        }

        //leaveWorld
        object leaveWorld = o.attr("leaveWorld");
        leaveWorld();

        //移除引用
        PyEntity & entity = extract<PyEntity&>(o);
        PyDict_DelItem(m_dict.ptr(), object(entity.getID()).ptr());

        entity.m_entity.to<WrapperIEntity>()->bindPython(object());

        EntityMgr::instance()->remove(entity.m_entity);
        entity.m_entity = nullptr;
    }

    void setPlayer(object o)
    {
        if (o.is_none())
        {
            m_player = o;
            EntityMgr::instance()->setPlayer(nullptr);
            return;
        }

        if (!extract<PyEntity&>(o).check())
        {
            PyErr_SetString(PyExc_TypeError, "Entity Type needed!");
            throw_error_already_set();
        }

        m_player = o;
        PyEntity & entity = extract<PyEntity&>(o);
        EntityMgr::instance()->setPlayer(entity.m_entity);
    }

    object getPlayer()
    {
        return m_player;
    }

    UINT handleMouseEvent(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        return EntityMgr::instance()->handleMouseEvent(msg, wparam, lparam);
    }

};

PyEntityMgr * getEntityMgr()
{
    static PyEntityMgr s_mgr;
    return &s_mgr;
}


void setCameraSource(CCursorCamera * camera, PyEntity & entity)
{
    camera->setSource(entity.m_entity.get());
}

//////////////////////////////////////////////////////////////////////////

namespace
{
    ModelPtr getNullModel(){ return ModelPtr(NULL); }

    float getModelHeight(iModel *model)
    {
        const AABB & aabb = model->getAABB();
        return aabb.max.y - aabb.min.y;
    }

}

//////////////////////////////////////////////////////////////////////////

void wrapperEntity()
{
    LOG_INFO(L"EXPORT - wrapperEntity");

    auto copyConstRef = return_value_policy<copy_const_reference>();
    auto existObjRef = return_value_policy<reference_existing_object>();

    class_ <PyPhysics, boost::noncopyable>("iPhysics", init<object>())
        .add_property("state", &PyPhysics::getState, &PyPhysics::setState)
        .def("enable", &PyPhysics::enable)
        .def("isEnable", &PyPhysics::isEnabled)
        .def("searchPath", &PyPhysics::searchPath)
        .def("faceTo", &PyPhysics::faceTo)
        .def("faceToDir", &PyPhysics::faceToDir)
        .def("moveTo", &PyPhysics::moveTo)
        .def("moveToEntity", &PyPhysics::moveToEntity)
        .def("breakAutoMove", &PyPhysics::breakAutoMove)
        .def("enableAI", &PyPhysics::enableAI)
        .def("isAIEnable", &PyPhysics::isAIEnable)
        .def("setAIInterval", &PyPhysics::setAIInterval)
        .def("getAIInterval", &PyPhysics::getAIInterval)
        ;

    void(iModel::*model_set_scale)(const Math::Vector3 &) = &iModel::setScale;
    object model_get_scale = make_function(&iModel::getScale, copyConstRef);

    class_<iModel, ModelPtr, boost::noncopyable> ("iModel")
        .add_property("scale", model_get_scale, model_set_scale)
        .add_property("yaw", &iModel::getYaw, &iModel::setYaw)
        .add_property("pitch", &iModel::getPitch, &iModel::setPitch)
        .add_property("roll", &iModel::getRoll, &iModel::setRoll)
        .add_property("name", make_function(&iModel::source, copyConstRef))
        .add_property("height", getModelHeight)
        .def("getWorldMatrix", &iModel::getWorldMatrix, copyConstRef)
        .def("setWorldMatrix", &iModel::setWorldMatrix)
        .def("isSkeleton", &iModel::isSkeleton)
        .def("showBound", &iModel::showBound)
        .def("playAction", &iModel::playAction)
        .def("stopAction", &iModel::stopAction)
        .def("getActionCount", &iModel::getActionCount)
        .def("getActionName", &iModel::getActionName)
        .def("setAnimSpeed", &iModel::setAnimSpeed)
        ;

    def("getNullModel", getNullModel);
    def("loadModel", ModelFactory::loadModel); //, return_value_policy<manage_new_object>()
    def("loadModelBg", ModelFactory::loadModelBg<object>);

    class_<PyEntity, boost::noncopyable>("iEntity")
        .add_property("id", &PyEntity::getID)
        .add_property("showDistance", &PyEntity::getShowDistance, &PyEntity::setShowDistance)
        .add_property("model", &PyEntity::getModel, &PyEntity::setModel)
        .add_property("physics", &PyEntity::getPhysics, &PyEntity::setPhysics)
        .add_property("position", make_function(&PyEntity::getPosition, copyConstRef), &PyEntity::setPosition)
        .add_property("speed", make_function(&PyEntity::getSpeed, copyConstRef), &PyEntity::setSpeed)
        .add_property("scale", make_function(&PyEntity::getScale, copyConstRef), &PyEntity::setScale)
        .def("show", &PyEntity::show)
        .def("visible", &PyEntity::visible)
        .def("enableLockHeight", &PyEntity::enableLockHeight)
        .def("setLockHeight", &PyEntity::setLockHeight)
        .def("enableSphereShow", &PyEntity::enableSphereShow)
        .def("isSphereShowEnable", &PyEntity::isSphereShowEnable)
        .def("isActive", &PyEntity::isActive)
        .def("distToCamera", &PyEntity::distToCamera)
        .def("distToPlayer", &PyEntity::distToPlayer)
        .def("lookAtPosition", &PyEntity::lookAtPosition)
        ;

    class_<PyEntityMgr, boost::noncopyable>("EntityMgr", no_init)
        .def("handleMouseEvent", &PyEntityMgr::handleMouseEvent)
        .def("entity", &PyEntityMgr::entity)
        .def("entities", &PyEntityMgr::entities)
        .def("player", &PyEntityMgr::getPlayer)
        .def("setPlayer", &PyEntityMgr::setPlayer)
        .def("createEntity", &PyEntityMgr::createEntity)
        .def("destroyEntity", &PyEntityMgr::destroyEntity)
        ;
    def("entityMgr", getEntityMgr, existObjRef);

    class_<CCamera>("cCamera", no_init)
        .add_property("speed", &CCamera::speed, &CCamera::setSpeed)
        .add_property("distToPlayer", &CCamera::getDistance, &CCamera::setDistance)
        .add_property("yaw", &CCamera::yaw)
        .add_property("pitch", &CCamera::pitch)
        .add_property("roll", &CCamera::roll)
        .def("getPosition", &CCamera::position, return_value_policy<copy_const_reference>())
        .def("setPosition", &CCamera::setPosition)
        .def("getLook", &CCamera::look, return_value_policy<copy_const_reference>())
        .def("getRight", &CCamera::right, return_value_policy<copy_const_reference>())
        .def("getUp", &CCamera::up, return_value_policy<copy_const_reference>())
        .def("rotYaw", &CCamera::rotYaw)
        .def("rotPitch", &CCamera::rotPitch)
        .def("rotRoll", &CCamera::rotRoll)
        .def("moveLook", &CCamera::moveLook)
        .def("moveUp", &CCamera::moveUp)
        .def("moveRight", &CCamera::moveRight)
        .def("setNearFar", &CCamera::setNearFar)
        .def("setDistRange", &CCamera::setDistRange)
        ;

    class_<CCursorCamera, bases<CCamera>>("cCursorCamera", no_init)
        .add_property("rotSpeed", &CCursorCamera::getRotateSpeed, &CCursorCamera::setCurRoSpeed)
        .def("setSource", &setCameraSource)
        //.def("handleMessage", &CCursorCamera::handleMessage)
        .def("isDraged", &CCursorCamera::isDraged)
        .def("setHeight", &CCursorCamera::setHeight)
        ;
}
