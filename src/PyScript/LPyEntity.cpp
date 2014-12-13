#include "stdafx.h"
#include "LPyEntity.h"
#include "LPyPhysics.h"
#include "../utility/MemoryCheck.h"

namespace Lzpy
{

    ////////////////////////////////////////////////////////////////////
    ///
    ////////////////////////////////////////////////////////////////////
    LZPY_CLASS_EXPORT(PyEntityPhysics)
    {
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
    }

    PyEntityPhysics::PyEntityPhysics()
    {
    }

    PyEntityPhysics::~PyEntityPhysics()
    {
    }

    LZPY_IMP_INIT(PyEntityPhysics)
    {
        return false;
    }

    LZPY_IMP_METHOD_1(PyEntityPhysics, searchPath)
    {
        Vector3 vec;
        if (!parse_object(vec, value))
            return null_object;

        return build_object(m_object->searchPath(vec));
    }

    LZPY_IMP_METHOD_1(PyEntityPhysics, faceTo)
    {
        Vector3 vec;
        if (!parse_object(vec, value))
            return null_object;

        m_object->faceTo(vec);
        return none_object;
    }

    LZPY_IMP_METHOD_1(PyEntityPhysics, faceToDir)
    {
        Vector3 vec;
        if (!parse_object(vec, value))
            return null_object;

        m_object->faceToDir(vec);
        return none_object;
    }

    LZPY_IMP_METHOD_1(PyEntityPhysics, moveTo)
    {
        Vector3 vec;
        if (!parse_object(vec, value))
            return null_object;

        m_object->moveTo(vec);
        return none_object;
    }

    LZPY_IMP_METHOD_1(PyEntityPhysics, moveToEntity)
    {
        IEntity *pEnt;
        if (!parse_object(pEnt, value))
            return null_object;

        m_object->moveToEntity(pEnt);
        return none_object;
    }

    LZPY_IMP_METHOD_0(PyEntityPhysics, breakAutoMove)
    {
        m_object->breakAutoMove();
        return none_object;
    }

    ////////////////////////////////////////////////////////////////////
    ///
    ////////////////////////////////////////////////////////////////////

    LZPY_CLASS_EXPORT(PyEntity)
    {
        LZPY_GET(id);

        LZPY_GETSET(model);
        LZPY_GETSET(physics);
        LZPY_GETSET(fadeDistance);
        LZPY_GETSET(visible);
        LZPY_GET(distToCamera);
        LZPY_GET(distToPlayer);

        LZPY_METHOD_1(lookAtPosition);
    }


    PyEntity::PyEntity()
    {
    }

    PyEntity::~PyEntity()
    {
    }

    LZPY_IMP_INIT(PyEntity)
    {
        return true;
    }

    LZPY_IMP_METHOD_1(PyEntity, lookAtPosition)
    {
        Vector3 vec;
        if (!parse_object(vec, value))
            return null_object;

        Vector3 look = vec - entity()->getPosition();
        look.normalize();
        entity()->faceToDir(look);
        return none_object;
    }

    ////////////////////////////////////////////////////////////////////
    ///
    ////////////////////////////////////////////////////////////////////
    LZPY_CLASS_EXPORT(PyEntityMgr)
    {
        LZPY_METHOD_1(addEntity);
        LZPY_METHOD_1(delEntity);
        LZPY_METHOD_1(getEntity);
    }

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
        EntityMgr::instance()->add(pEnt->entity());
        m_entities.setitem(pEnt->getID(), entity);

        entity.call_method_quiet("enterWorld");
        return true;
    }

    bool PyEntityMgr::destroyEntity(object entity)
    {
        if (!CHECK_INSTANCE(PyEntity, entity.get()))
            return false;

        entity.call_method_quiet("leaveWorld");

        PyEntity *pEnt = entity.cast<PyEntity>();
        EntityMgr::instance()->remove(pEnt->entity());

        m_entities.delitem(entity.cast<PyEntity>()->getID());
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

        int id = entity.cast<PyEntity>()->getID();
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
        if (!PyArg_ParseTuple(arg, "u", &source)) return NULL;

        ModelPtr model = ModelFactory::loadModel(source);
        if (!model) Py_RETURN_NONE;

        object m = build_object(model.get());
        return xincref(m);
    }

    LZPY_DEF_FUN_0(player)
    {
        return xincref(s_pPlayer);
    }

    LZPY_DEF_FUN_1(setPlayer)
    {
        if (s_pPlayer == value)
            Py_RETURN_NONE;

        IEntity *player = nullptr;
        if (value == Py_None)
        {
        }
        else if (CHECK_INSTANCE(PyEntity, value))
        {
            player = ((PyEntity*) value)->entity();
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
                s_pEntityMgr = new_instance_ex<PyEntityMgr>();

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

    void exportEntity(const char * module)
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