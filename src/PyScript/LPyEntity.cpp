#include "stdafx.h"
#include "LPyEntity.h"
#include "LPyPhysics.h"

namespace LazyPy
{
    LAZYPY_DEF_FUN_1(destroyEntity);

    ////////////////////////////////////////////////////////////////////
    LAZYPY_IMP("Model", PyModel, "Lazy");
    
    LAZYPY_BEGIN_EXTEN(PyModel);
    LAZYPY_GETSET(yaw);
    LAZYPY_GETSET(pitch);
    LAZYPY_GETSET(roll);
    LAZYPY_GETSET(scale);
    LAZYPY_GET(resource);

    LAZYPY_METHOD_0(isSkeleton);
    LAZYPY_METHOD_0(isMesh);
    LAZYPY_METHOD_1(showBound);
    LAZYPY_METHOD(playAction);
    LAZYPY_METHOD_0(stopAction);
    LAZYPY_METHOD_0(getActionCount);
    LAZYPY_METHOD_1(getActionName);
    LAZYPY_METHOD_1(setAnimSpeed);
    LAZYPY_METHOD_0(getAAABB);
    LAZYPY_METHOD_0(getHeight);
    LAZYPY_END_EXTEN();

    PyModel::PyModel()
    {
    }

    LAZYPY_IMP_INIT(PyModel)
    {
        PyErr_SetString(PyExc_TypeError, "Can't create instance of Lazy.PyModel!");
        return false;
    }

    LAZYPY_IMP_METHOD_0(PyModel, isSkeleton)
    {
        return toPyObject(m_model->isSkeleton());
    }
    LAZYPY_IMP_METHOD_0(PyModel, isMesh)
    {
        return toPyObject(m_model->isMesh());
    }
    LAZYPY_IMP_METHOD_1(PyModel, showBound)
    {
        m_model->showBound(fromPyObject<bool>(value));
        Py_RETURN_NONE;
    }
    LAZYPY_IMP_METHOD(PyModel, playAction)
    {
        wchar_t *name;
        PyObject *loop = nullptr;
        if (!PyArg_ParseTuple(arg, "u|O", &name, &loop)) return NULL;

        m_model->playAction(name, fromPyObject<bool>(loop));
        Py_RETURN_NONE;
    }
    LAZYPY_IMP_METHOD_0(PyModel, stopAction)
    {
        m_model->stopAction();
        Py_RETURN_NONE;
    }
    LAZYPY_IMP_METHOD_0(PyModel, getActionCount)
    {
        return toPyObject(m_model->getActionCount());
    }
    LAZYPY_IMP_METHOD_1(PyModel, getActionName)
    {
        std::wstring name;
        m_model->getActionName(name, fromPyObject<int>(value));

        return toPyObject(name);
    }
    LAZYPY_IMP_METHOD_1(PyModel, setAnimSpeed)
    {
        m_model->setAnimSpeed(fromPyObject<float>(value));
        Py_RETURN_NONE;
    }

    LAZYPY_IMP_METHOD_0(PyModel, getAAABB)
    {
        Physics::AABB aabb = m_model->getAABB();
        PyObject * pMin = toPyObject(aabb.min);
        PyObject * pMax = toPyObject(aabb.max);
        PyObject * pRet = PyTuple_New(2);
        PyTuple_SetItem(pRet, 0, pMin);
        PyTuple_SetItem(pRet, 1, pMax);
        return pRet;
    }

    LAZYPY_IMP_METHOD_0(PyModel, getHeight)
    {
        Physics::AABB aabb = m_model->getAABB();
        return toPyObject(aabb.max.y - aabb.min.y);
    }

    ////////////////////////////////////////////////////////////////////
    EntityPhysics::EntityPhysics(PyObject *self)
        : m_self(self)
    {

    }

    void EntityPhysics::onAITrigger()
    {
        if (!PyObject_HasAttrString(m_self, "onAITrigger")) return;

        PyObject *pRet = PyObject_CallMethod(m_self, "onAITrigger", NULL);
        Py_XDECREF(pRet);
    }

    void EntityPhysics::onMoveToFinished(bool succed)
    {
        if (!PyObject_HasAttrString(m_self, "onMoveToFinished")) return;

        PyObject *pRet = PyObject_CallMethod(m_self, "onMoveToFinished", "i", (int)succed);
        Py_XDECREF(pRet);
    }

    void EntityPhysics::onMoveToEntityFinish(bool succed)
    {
        if (!PyObject_HasAttrString(m_self, "onMoveToEntityFinish")) return;

        PyObject *pRet = PyObject_CallMethod(m_self, "onMoveToEntityFinish", "i", (int) succed);
        Py_XDECREF(pRet);
    }

    void EntityPhysics::onSearchToFinished(bool succed)
    {
        if (!PyObject_HasAttrString(m_self, "onSearchToFinished")) return;

        PyObject *pRet = PyObject_CallMethod(m_self, "onSearchToFinished", "i", (int) succed);
        Py_XDECREF(pRet);
    }

    void EntityPhysics::onStateChange(DWORD oldState)
    {
        if (!PyObject_HasAttrString(m_self, "onStateChange")) return;

        PyObject *pRet = PyObject_CallMethod(m_self, "onStateChange", "I", oldState);
        Py_XDECREF(pRet);
    }


    LAZYPY_IMP("Physics", PyEntityPhysics, "Lazy");
    LAZYPY_BEGIN_EXTEN(PyEntityPhysics);
    LAZYPY_GET(source);
    LAZYPY_GETSET(state);
    LAZYPY_GETSET(enble);
    LAZYPY_GETSET(aiEnble);
    LAZYPY_GETSET(aiInterval);

    LAZYPY_METHOD_1(searchPath);
    LAZYPY_METHOD_1(faceTo);
    LAZYPY_METHOD_1(faceToDir);
    LAZYPY_METHOD_1(moveTo);
    LAZYPY_METHOD_1(moveToEntity);
    LAZYPY_METHOD_0(breakAutoMove);

    LAZYPY_END_EXTEN();

    PyEntityPhysics::PyEntityPhysics()
    {
        m_source = (PyEntity*)Py_None;
        Py_INCREF(m_source);
    }

    PyEntityPhysics::~PyEntityPhysics()
    {
        Py_DECREF(m_source);
        m_source = nullptr;
    }

    LAZYPY_IMP_INIT(PyEntityPhysics)
    {
        m_physics = new EntityPhysics(this);
        return true;
    }

    LAZYPY_IMP_GET(PyEntityPhysics, source)
    {
        return toPyObject(m_source);
    }

    void PyEntityPhysics::setSource(PyEntity *source)
    {
        copyPyObject((PyObject**) &m_source, source);
    }

    LAZYPY_IMP_GET(PyEntityPhysics, state)
    {
        return toPyObject(m_physics->getState());
    }

    LAZYPY_IMP_SET(PyEntityPhysics, state)
    {
        m_physics->setState(fromPyObject<DWORD>(value));
    }


    LAZYPY_IMP_GET(PyEntityPhysics, enble)
    {
        return toPyObject(m_physics->isEnabled());
    }

    LAZYPY_IMP_GET(PyEntityPhysics, aiEnble)
    {
        return toPyObject(m_physics->isAIEnable());
    }

    LAZYPY_IMP_GET(PyEntityPhysics, aiInterval)
    {
        return toPyObject(m_physics->getAIInterval());
    }

    LAZYPY_IMP_SET(PyEntityPhysics, enble)
    {
        m_physics->enable(fromPyObject<bool>(value));
    }

    LAZYPY_IMP_SET(PyEntityPhysics, aiEnble)
    {
        m_physics->enableAI(fromPyObject<bool>(value));
    }

    LAZYPY_IMP_SET(PyEntityPhysics, aiInterval)
    {
        m_physics->setAIInterval(fromPyObject<float>(value));
    }

    LAZYPY_IMP_METHOD_1(PyEntityPhysics, searchPath)
    {
        Physics::Vector3 vec;
        if (!v3FromPyObject(vec, value)) return nullptr;

        return toPyObject(m_physics->searchPath(vec));
    }

    LAZYPY_IMP_METHOD_1(PyEntityPhysics, faceTo)
    {
        Physics::Vector3 vec;
        if (!v3FromPyObject(vec, value)) return nullptr;

        m_physics->faceTo(vec);
        Py_RETURN_NONE;
    }

    LAZYPY_IMP_METHOD_1(PyEntityPhysics, faceToDir)
    {
        Physics::Vector3 vec;
        if (!v3FromPyObject(vec, value)) return nullptr;

        m_physics->faceToDir(vec);
        Py_RETURN_NONE;
    }

    LAZYPY_IMP_METHOD_1(PyEntityPhysics, moveTo)
    {
        Physics::Vector3 vec;
        if (!v3FromPyObject(vec, value)) return nullptr;

        m_physics->moveTo(vec);
        Py_RETURN_NONE;
    }

    LAZYPY_IMP_METHOD_1(PyEntityPhysics, moveToEntity)
    {
        if (!PyEntity_Check(value)) return NULL;

        PyEntity *pEnt = (PyEntity*) value;
        m_physics->moveToEntity(pEnt->m_entity.get());

        Py_RETURN_NONE;
    }

    LAZYPY_IMP_METHOD_0(PyEntityPhysics, breakAutoMove)
    {
        m_physics->breakAutoMove();
        Py_RETURN_NONE;
    }

    ////////////////////////////////////////////////////////////////////

    LAZYPY_IMP("Entity", PyEntity, "Lazy");

    LAZYPY_BEGIN_EXTEN(PyEntity);
    LAZYPY_GET(id);
    LAZYPY_GETSET(position);
    LAZYPY_GETSET(speed);
    LAZYPY_GETSET(scale);
    LAZYPY_GETSET(model);
    LAZYPY_GETSET(physics);
    LAZYPY_GETSET(fadeDistance);
    LAZYPY_GETSET(lockHeight);

    LAZYPY_METHOD_1(show);
    LAZYPY_METHOD_0(visible);
    LAZYPY_METHOD_0(distToCamera);
    LAZYPY_METHOD_0(distToPlayer);
    LAZYPY_METHOD_1(lookAtPosition);

    LAZYPY_END_EXTEN();


    bool PyEntity_Check(PyObject *value, bool setError/* = true*/)
    {
        return helper::has_instance<PyEntity>(value, setError);
    }


    PyEntity::PyEntity()
    {
        m_entity = new Entity(this);

        m_pyModel = (PyModel*) Py_None;
        Py_DecRef(m_pyModel);

        m_pyPhysics = (PyEntityPhysics*) Py_None;
        Py_DecRef(m_pyPhysics);
    }

    PyEntity::~PyEntity()
    {
        if (m_entity)
        {
            m_entity->m_self = nullptr;
            m_entity->setPhysics(nullptr);
            m_entity->setModel(nullptr);
        }
        m_entity = nullptr;

        Py_DecRef(m_pyModel);
        Py_DecRef(m_pyPhysics);
    }

    int PyEntity::getId()
    {
        if (m_entity) return m_entity->getID();
        return 0;
    }

    LAZYPY_IMP_INIT(PyEntity)
    {
        PyErr_SetString(PyExc_RuntimeError, "Can't create instance of class Entity directly!");
        //m_entity = new Entity(this);
        return false;
    }

    LAZYPY_IMP_GET(PyEntity, id)
    {
        return toPyObject(getId());
    }

    LAZYPY_IMP_METHOD_1(PyEntity, show)
    {
        m_entity->show(fromPyObject<bool>(value));

        Py_RETURN_NONE;
    }

    LAZYPY_IMP_METHOD_0(PyEntity, visible)
    {
        return toPyObject(m_entity->visible());
    }

    LAZYPY_IMP_GET(PyEntity, position)
    {
        return toPyObject(m_entity->m_vPos);
    }

    LAZYPY_IMP_SET(PyEntity, position)
    {
        v3FromPyObject(m_entity->m_vPos, value);
    }

    LAZYPY_IMP_GET(PyEntity, scale)
    {
        return toPyObject(m_entity->m_vScale);
    }

    LAZYPY_IMP_SET(PyEntity, scale)
    {
        v3FromPyObject(m_entity->m_vScale, value);
    }

    LAZYPY_IMP_GET(PyEntity, speed)
    {
        return toPyObject(m_entity->m_vSpeed);
    }

    LAZYPY_IMP_SET(PyEntity, speed)
    {
        v3FromPyObject(m_entity->m_vSpeed, value);
    }

    LAZYPY_IMP_GET(PyEntity, model)
    {
        return toPyObject(m_pyModel);
    }

    LAZYPY_IMP_SET(PyEntity, model)
    {
        if (value == m_pyModel) return;

        if (value != Py_None && !helper::has_instance<PyModel>(value, true))
        {
            return;
        }

        Py_DecRef(m_pyModel);
        m_pyModel = (PyModel*) value;
        Py_IncRef(m_pyModel);

        m_entity->setModel(m_pyModel != Py_None ? m_pyModel->m_model : nullptr);
    }


    LAZYPY_IMP_GET(PyEntity, physics)
    {
        return toPyObject(m_pyPhysics);
    }

    LAZYPY_IMP_SET(PyEntity, physics)
    {
        if (value == m_pyPhysics) return;

        if (value != Py_None && !helper::has_instance<PyEntityPhysics>(value, true))
        {
            return;
        }

        copyPyObject((PyObject**)&m_pyPhysics, value);

        if (m_pyPhysics != Py_None)
        {
            m_pyPhysics->setSource(this);
            m_entity->setPhysics(m_pyPhysics->m_physics);
        }
        else
        {
            m_entity->setPhysics(nullptr);
        }
    }

    LAZYPY_IMP_GET(PyEntity, fadeDistance)
    {
        return toPyObject(m_entity->getShowDistance());
    }

    LAZYPY_IMP_SET(PyEntity, fadeDistance)
    {
        m_entity->setShowDistance(fromPyObject<float>(value));
    }
    LAZYPY_IMP_GET(PyEntity, lockHeight)
    {
        return toPyObject(m_entity->getLockHeight());
    }
    LAZYPY_IMP_SET(PyEntity, lockHeight)
    {
        m_entity->setLockHeight(fromPyObject<float>(value));
    }

    LAZYPY_IMP_METHOD_0(PyEntity, distToCamera)
    {
        return toPyObject(m_entity->distToCamera());
    }

    LAZYPY_IMP_METHOD_0(PyEntity, distToPlayer)
    {
        return toPyObject(m_entity->distToPlayer());
    }

    LAZYPY_IMP_METHOD_1(PyEntity, lookAtPosition)
    {
        Physics::Vector3 vec;
        if (!v3FromPyObject(vec, value)) return nullptr;

        m_entity->lookAtPosition(vec);
        Py_RETURN_NONE;
    }

    ////////////////////////////////////////////////////////////////////
    LAZYPY_IMP("EntityMgr", PyEntityMgr, "Lazy");

    LAZYPY_BEGIN_EXTEN(PyEntityMgr);
    LAZYPY_METHOD_1(addEntity);
    LAZYPY_METHOD_1(delEntity);
    LAZYPY_METHOD_1(getEntity);
    LAZYPY_END_EXTEN();

    PyEntityMgr::PyEntityMgr()
    {
        m_pDict = PyDict_New();

        MEMORY_CHECK_CONS;
    }

    PyEntityMgr::~PyEntityMgr()
    {
        PyObject *list = PyDict_Values(m_pDict);
        size_t n = PyList_Size(list);
        for (size_t i = 0; i < n; ++i)
        {
            PyObject *ret = _wraper_fundestroyEntity_1(nullptr, PyList_GetItem(list, i) );
            Py_XDECREF(ret);
        }

        Py_DECREF(list);
        Py_DECREF(m_pDict);

        MEMORY_CHECK_DEST;
    }

    LAZYPY_IMP_INIT(PyEntityMgr)
    {
        PyErr_SetString(PyExc_RuntimeError, "Can't create instance of Lazy.EntityMgr");
        return false;
    }

    LAZYPY_IMP_METHOD_1(PyEntityMgr, addEntity)
    {
        if (!PyEntity_Check(value)) return NULL;

        PyEntity *pEnt = (PyEntity*) value;
        int id = pEnt->getId();
        if (id <= 0)
        {
            PyErr_SetString(PyExc_RuntimeError, "Inviliad entity!");
            return NULL;
        }

        PyObject *pyId = toPyObject(id);
        PyDict_SetItem(m_pDict, pyId, pEnt);
        Py_DecRef(pyId);

        EntityMgr::instance()->add(pEnt->m_entity);

        Py_RETURN_NONE;
    }

    LAZYPY_IMP_METHOD_1(PyEntityMgr, delEntity)
    {
        if (!PyEntity_Check(value)) return NULL;

        PyEntity *pEnt = (PyEntity*) value;
        int id = pEnt->getId();
        if (id <= 0)
        {
            PyErr_SetString(PyExc_RuntimeError, "Inviliad entity!");
            return NULL;
        }

        PyObject *pyId = toPyObject(id);
        PyDict_DelItem(m_pDict, pyId);
        Py_DecRef(pyId);

        EntityMgr::instance()->remove(pEnt->m_entity);

        Py_RETURN_NONE;
    }

    LAZYPY_IMP_METHOD_1(PyEntityMgr, getEntity)
    {
        PyObject *pEnt = PyDict_GetItem(m_pDict, value);
        if (!pEnt) Py_RETURN_NONE;

        Py_IncRef(pEnt);
        return pEnt;
    }

    ////////////////////////////////////////////////////////////////////
    static PyEntity * s_pPlayer = nullptr;
    static PyEntityMgr * s_pEntityMgr = nullptr;


    LAZYPY_DEF_FUN_1(entity)
    {
        assert(s_pEntityMgr);

        return s_pEntityMgr->py_getEntity(value);
    }

    LAZYPY_DEF_FUN_0(entities)
    {
        assert(s_pEntityMgr);

        return PyDict_Copy(s_pEntityMgr->m_pDict);
    }

    LAZYPY_DEF_FUN(createEntity)
    {
        PyObject *pClass;
        PyObject *pDict = nullptr;
        if (!PyArg_ParseTuple(arg, "O|O", &pClass, &pDict)) return NULL;

        if (!helper::has_sub_class<PyEntity>(pClass, true)) return NULL;
        
        if (pDict && !PyDict_Check(pDict))
        {
            PyErr_SetString(PyExc_TypeError, "The second arg must be a dict!");
            return NULL;
        }

        //实例化对象
        PyEntity *pEntity = (PyEntity *) PyObject_CallObject(pClass, NULL);
        if (pEntity == NULL) return NULL;


        //设置属性
        if (pDict)
        {
            PyObject *key, *value;
            Py_ssize_t pos = 0;

            while (PyDict_Next(pDict, &pos, &key, &value)) 
            {
                PyObject_SetAttr(pEntity, key, value);
            }
        }
        
        //enterworld
        if (PyObject_HasAttrString(pEntity, "enterWorld"))
        {
            PyObject *ret = PyObject_CallMethod(pEntity, "enterWorld", nullptr);
            Py_XDECREF(ret);
        }

        PyObject *ret = s_pEntityMgr->py_addEntity(pEntity);
        Py_XDECREF(ret);

        int id = pEntity->getId();
        Py_DECREF(pEntity);

        return toPyObject(id);
    }

    LAZYPY_DEF_FUN_1(destroyEntity)
    {
        if (!PyEntity_Check(value)) return NULL;

        PyEntity *pEntity = (PyEntity*) value;

        //leaveworld
        if (PyObject_HasAttrString(pEntity, "leaveWorld"))
        {
            PyObject *ret = PyObject_CallMethod(pEntity, "leaveWorld", nullptr);
            Py_XDECREF(ret);
        }

        PyObject *ret = s_pEntityMgr->py_delEntity(pEntity);
        Py_XDECREF( ret );

        Py_RETURN_NONE;
    }

    LAZYPY_DEF_FUN(loadModel)
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

    LAZYPY_DEF_FUN_0(player)
    {
        return toPyObject(s_pPlayer);
    }

    LAZYPY_DEF_FUN_1(setPlayer)
    {
        if (s_pPlayer == value) Py_RETURN_NONE;

        if (value != Py_None && !PyEntity_Check(value)) return NULL;

        Py_DecRef(s_pPlayer);
        s_pPlayer = (PyEntity*)value;
        Py_IncRef(s_pPlayer);

        iEntity *player = nullptr;
        if (s_pPlayer != Py_None && s_pPlayer->m_entity)
        {
            player = s_pPlayer->m_entity.get();
        }

        EntityMgr::instance()->setPlayer(player);
        TerrainMap::instance()->setSource(player);
        getCamera()->setSource(player);
        //getApp()->getSkyBox()->setSource(player);

        Py_RETURN_NONE;
    }

    namespace _py_entity
    {
        LAZYPY_BEGIN_FUN(Lazy);
        LAZYPY_FUN_1(entity);
        LAZYPY_FUN_0(entities);
        LAZYPY_FUN(createEntity);
        LAZYPY_FUN_1(destroyEntity);
        LAZYPY_FUN(loadModel);
        LAZYPY_FUN_0(player);
        LAZYPY_FUN_1(setPlayer);

        LAZYPY_END_FUN();

        class EntityResInterface : public LazyPyResInterface
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

}