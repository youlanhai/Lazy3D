#include "stdafx.h"
#include "LPyEntity.h"
#include "LPyPhysics.h"
#include "../utility/MemoryCheck.h"

namespace Lzpy
{
    ////////////////////////////////////////////////////////////////////
    ///
    ////////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(PyModel);
        LZPY_GETSET(yaw);
        LZPY_GETSET(pitch);
        LZPY_GETSET(roll);
        LZPY_GETSET(scale);
        LZPY_GET(resource);
        LZPY_GET(isSkeleton);
        LZPY_GET(isMesh);
        LZPY_GET(aabb);
        LZPY_GET(height);
        LZPY_GETSET(aabbVisible);

        LZPY_METHOD(playAction);
        LZPY_METHOD_0(stopAction);
        LZPY_METHOD_0(getActionCount);
        LZPY_METHOD_1(getActionName);
        LZPY_METHOD_1(setAnimSpeed);
    LZPY_CLASS_END();

    PyModel::PyModel()
    {
    }

    LZPY_IMP_INIT(PyModel)
    {
        PyErr_SetString(PyExc_RuntimeError, "PyModel can't initialize directly!");
        return false;
    }

    LZPY_IMP_METHOD(PyModel, playAction)
    {
        std::wstring name;
        bool loop = false;
        if (!arg.parse_tuple_default(1, &name, &loop))
            return null_object;

        m_model->playAction(name, loop);
        return none_object;
    }

    LZPY_IMP_METHOD_0(PyModel, stopAction)
    {
        m_model->stopAction();
        return none_object;
    }

    LZPY_IMP_METHOD_0(PyModel, getActionCount)
    {
        return build_object(m_model->getActionCount());
    }

    LZPY_IMP_METHOD_1(PyModel, getActionName)
    {
        std::wstring name;
        m_model->getActionName(name, parse_object<int>(value));

        return build_object(name);
    }

    LZPY_IMP_METHOD_1(PyModel, setAnimSpeed)
    {
        m_model->setAnimSpeed(parse_object<float>(value));
        return none_object;
    }

    float PyModel::getHeight()
    {
        Physics::AABB aabb = m_model->getAABB();
        return aabb.max.y - aabb.min.y;
    }

    object PyModel::getAABB()
    {
        Physics::AABB aabb = m_model->getAABB();
        return build_tuple(aabb.min, aabb.max);
    }

    ////////////////////////////////////////////////////////////////////
    ///
    ////////////////////////////////////////////////////////////////////
    EntityPhysics::EntityPhysics(PyObject *self)
        : m_self(self)
    {}

    void EntityPhysics::onAITrigger()
    {
        m_self.call_method_quiet("onAITrigger");
    }

    void EntityPhysics::onMoveToFinished(bool succed)
    {
        m_self.call_method_quiet("onMoveToFinished", succed);
    }

    void EntityPhysics::onMoveToEntityFinish(bool succed)
    {
        m_self.call_method_quiet("onMoveToEntityFinish", succed);
    }

    void EntityPhysics::onSearchToFinished(bool succed)
    {
        m_self.call_method_quiet("onSearchToFinished", succed);
    }

    void EntityPhysics::onStateChange(DWORD oldState)
    {
        m_self.call_method_quiet("onStateChange", oldState);
    }

    ////////////////////////////////////////////////////////////////////
    ///
    ////////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(PyEntityPhysics);
        LZPY_GET(source);
        LZPY_GETSET(state);
        LZPY_GETSET(enable);
        LZPY_GETSET(aiEnble);
        LZPY_GETSET(aiInterval);

        LZPY_METHOD_1(searchPath);
        LZPY_METHOD_1(faceTo);
        LZPY_METHOD_1(faceToDir);
        LZPY_METHOD_1(moveTo);
        LZPY_METHOD_1(moveToEntity);
        LZPY_METHOD_0(breakAutoMove);
    LZPY_CLASS_END();

    PyEntityPhysics::PyEntityPhysics()
        : m_source(none_object)
    {
        m_physics = new EntityPhysics(this);
    }

    PyEntityPhysics::~PyEntityPhysics()
    {
    }

    LZPY_IMP_INIT(PyEntityPhysics)
    {
        return true;
    }

    LZPY_IMP_METHOD_1(PyEntityPhysics, searchPath)
    {
        Physics::Vector3 vec;
        if (!parse_object(vec, value)) 
            return null_object;

        return build_object(m_physics->searchPath(vec));
    }

    LZPY_IMP_METHOD_1(PyEntityPhysics, faceTo)
    {
        Physics::Vector3 vec;
        if (!parse_object(vec, value))
            return null_object;

        m_physics->faceTo(vec);
        return none_object;
    }

    LZPY_IMP_METHOD_1(PyEntityPhysics, faceToDir)
    {
        Physics::Vector3 vec;
        if (!parse_object(vec, value))
            return null_object;

        m_physics->faceToDir(vec);
        return none_object;
    }

    LZPY_IMP_METHOD_1(PyEntityPhysics, moveTo)
    {
        Physics::Vector3 vec;
        if (!parse_object(vec, value))
            return null_object;

        m_physics->moveTo(vec);
        return none_object;
    }

    LZPY_IMP_METHOD_1(PyEntityPhysics, moveToEntity)
    {
        if (!CHECK_INSTANCE(PyEntity, value.get()))
            return null_object;

        PyEntity *pEnt = value.cast<PyEntity>();
        m_physics->moveToEntity(pEnt->m_entity.get());

        return none_object;
    }

    LZPY_IMP_METHOD_0(PyEntityPhysics, breakAutoMove)
    {
        m_physics->breakAutoMove();
        return none_object;
    }

    ////////////////////////////////////////////////////////////////////
    ///
    ////////////////////////////////////////////////////////////////////

    Entity::Entity(PyObject * self)
        : m_self(self)
    {}

    ///是否可以被鼠标选择
    bool Entity::canSelect(void) const 
    {
        object ret = const_cast<object_base &>(m_self).call_method_quiet("canSelect");

        return parse_object<bool>(ret);
    }

    bool Entity::isPlayer(void) 
    {
        if (!m_self.hasattr("isPlayer")) return false;

        object ret = m_self.call_method_quiet("isPlayer");
        return parse_object<bool>(ret);
    }

    void Entity::onFocusCursor(UINT msg) 
    {
        m_self.call_method_quiet("onFocusCursor", msg);
    }

    LZPY_CLASS_BEG(PyEntity);
        LZPY_GET(id);
        LZPY_GETSET(position);
        LZPY_GETSET(speed);
        LZPY_GETSET(scale);
        LZPY_GETSET(model);
        LZPY_GETSET(physics);
        LZPY_GETSET(fadeDistance);
        LZPY_GETSET(lockHeight);
        LZPY_GETSET(visible);
        LZPY_GET(distToCamera);
        LZPY_GET(distToPlayer);

        LZPY_METHOD_1(lookAtPosition);
    LZPY_CLASS_END();


    PyEntity::PyEntity()
    {
        m_entity = new Entity(this);
        m_model = none_object;
        m_physics = none_object;
    }

    PyEntity::~PyEntity()
    {
        if (m_entity)
        {
            m_entity->setPhysics(nullptr);
            m_entity->setModel(nullptr);
        }
        m_entity = nullptr;
        m_model = null_object;
        m_physics = null_object;
    }

    int PyEntity::getId()
    {
        if (m_entity) return m_entity->getID();
        return 0;
    }

    LZPY_IMP_INIT(PyEntity)
    {
        return true;
    }

    void PyEntity::setModel(object model)
    {
        if (m_model == model)
            return;

        if (model.is_none())
        {
            m_model = model;
            m_entity->setModel(nullptr);
        }
        else if (CHECK_INSTANCE(PyModel, model.get()))
        {
            m_model = model;
            m_entity->setModel(model.cast<PyModel>()->m_model);
        }
    }

    void PyEntity::setPhysics(object physics)
    {
        if (m_physics == physics)
            return;

        if (m_physics)
        {
            m_physics.call_method_quiet("onActive", false);

            m_physics->m_source = none_object;
            m_physics = none_object;
            m_entity->setPhysics(nullptr);
        }

        if (physics.is_none())
        {

        }
        else if (CHECK_INSTANCE(PyEntityPhysics, physics.get()))
        {
            m_physics = physics;
            m_physics->m_source = object(this);
            m_entity->setPhysics(m_physics->m_physics);

            m_physics.call_method_quiet("onActive", true);
        }
    }

    LZPY_IMP_METHOD_1(PyEntity, lookAtPosition)
    {
        Physics::Vector3 vec;
        if (!parse_object(vec, value))
            return null_object;

        m_entity->lookAtPosition(vec);
        return none_object;
    }

    ////////////////////////////////////////////////////////////////////
    ///
    ////////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(PyEntityMgr);
        LZPY_METHOD_1(addEntity);
        LZPY_METHOD_1(delEntity);
        LZPY_METHOD_1(getEntity);
    LZPY_CLASS_END();

    PyEntityMgr::PyEntityMgr()
    {
        MEMORY_CHECK_CONS(this);
    }

    PyEntityMgr::~PyEntityMgr()
    {
        list list = m_entities.values();

        size_t n = list.size();
        for (size_t i = 0; i < n; ++i)
        {
            destroyEntity(list[i]);
        }

        MEMORY_CHECK_DEST(this);
    }

    LZPY_IMP_INIT(PyEntityMgr)
    {
        PyErr_SetString(PyExc_RuntimeError, "Can't create instance of Lazy.EntityMgr");
        return false;
    }

    LZPY_IMP_METHOD_1(PyEntityMgr, addEntity)
    {
        return build_object( addEntity(value) );
    }

    LZPY_IMP_METHOD_1(PyEntityMgr, delEntity)
    {
        return build_object( destroyEntity(value) );
    }

    LZPY_IMP_METHOD_1(PyEntityMgr, getEntity)
    {
        return m_entities.getitem(value);
    }

    bool PyEntityMgr::addEntity(object entity)
    {
        if (!CHECK_INSTANCE(PyEntity, entity.get()))
            return false;

        PyEntity *pEnt = entity.cast<PyEntity>();
        EntityMgr::instance()->add(pEnt->m_entity);
        m_entities.setitem(pEnt->getId(), entity);

        entity.call_method_quiet("enterWorld");
        return true;
    }

    bool PyEntityMgr::destroyEntity(object entity)
    {
        if (!CHECK_INSTANCE(PyEntity, entity.get()))
            return false;

        entity.call_method_quiet("leaveWorld");
        
        PyEntity *pEnt = entity.cast<PyEntity>();
        EntityMgr::instance()->remove(pEnt->m_entity);

        m_entities.delitem(entity.cast<PyEntity>()->getId());
        return true;
    }

    ////////////////////////////////////////////////////////////////////
    ///
    ////////////////////////////////////////////////////////////////////
    static PyEntity * s_pPlayer = nullptr;
    static PyEntityMgr * s_pEntityMgr = nullptr;


    LZPY_DEF_FUN_1(entity)
    {
        assert(s_pEntityMgr);

        return xincref(s_pEntityMgr->py_getEntity(object(value)));
    }

    LZPY_DEF_FUN_0(entities)
    {
        assert(s_pEntityMgr);

        return PyDict_Copy(s_pEntityMgr->m_entities.get());
    }

    LZPY_DEF_FUN(createEntity)
    {
        PyObject *pClass;
        if (!PyArg_ParseTuple(arg, "O", &pClass))
            return NULL;

        if (!CHECK_SUBCLASS(PyEntity, pClass))
            return NULL;

        //实例化对象
        object entity = new_reference(PyObject_CallObject(pClass, NULL));
        if (!entity) return NULL;
        
        s_pEntityMgr->addEntity(entity);
        
        int id = entity.cast<PyEntity>()->getId();
        return xincref(build_object(id));
    }

    LZPY_DEF_FUN_1(destroyEntity)
    {
        bool ret = s_pEntityMgr->destroyEntity(object(value));
        return xincref(build_object(ret));
    }

    LZPY_DEF_FUN(loadModel)
    {
        wchar_t *source;
        int type;
        if (!PyArg_ParseTuple(arg, "ui", &source, &type)) return NULL;

        ModelPtr model = ModelFactory::loadModel(source, type);
        if (!model) Py_RETURN_NONE;

        PyModel *pModel = helper::new_instance_ex<PyModel>();
        pModel->m_model = model;
        return pModel;
    }

    LZPY_DEF_FUN_0(player)
    {
        return xincref(s_pPlayer);
    }

    LZPY_DEF_FUN_1(setPlayer)
    {
        if (s_pPlayer == value)
            Py_RETURN_NONE;

        iEntity *player = nullptr;
        if (value == Py_None)
        {
        }
        else if (CHECK_INSTANCE(PyEntity, value))
        {
            player = ((PyEntity*) value)->m_entity.get();
        }
        else
        {
            return NULL;
        }

        xdecref(s_pPlayer);
        s_pPlayer = (PyEntity*) xincref(value);

        EntityMgr::instance()->setPlayer(player);
        TerrainMap::instance()->setSource(player);
        getCamera()->setSource(player);
        //getApp()->getSkyBox()->setSource(player);

        Py_RETURN_NONE;
    }

    namespace _py_entity
    {

        class EntityResInterface : public LzpyResInterface
        {
        public:

            virtual void init() override
            {
                s_pEntityMgr = helper::new_instance_ex<PyEntityMgr>();

                s_pPlayer = (PyEntity*) Py_None;
                Py_INCREF(s_pPlayer);
            }


            virtual void fini() override
            {
                Py_XDECREF(s_pEntityMgr);
                s_pEntityMgr = nullptr;

                Py_XDECREF(s_pPlayer);
                s_pPlayer = nullptr;
            }
        };

        static EntityResInterface s_resInterface;
    }

    void exportPyEntity(const char * module)
    {
        LZPY_REGISTER_CLASS(Model, PyModel);
        LZPY_REGISTER_CLASS(Entity, PyEntity);
        LZPY_REGISTER_CLASS(Physics, PyEntityPhysics);
        LZPY_REGISTER_CLASS(EntityMgr, PyEntityMgr);

        LZPY_FUN_1(entity);
        LZPY_FUN_0(entities);
        LZPY_FUN(createEntity);
        LZPY_FUN_1(destroyEntity);
        LZPY_FUN(loadModel);
        LZPY_FUN_0(player);
        LZPY_FUN_1(setPlayer);
    }

}