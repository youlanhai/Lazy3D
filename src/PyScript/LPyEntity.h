#pragma once

#include "../Core/Entity.h"
#include "LPyPhysics.h"
#include "LPySceneNode.h"

namespace Lzpy
{
    using namespace Lazy;

    class PyEntity;


    ////////////////////////////////////////////////////////////////////
    //physics
    ////////////////////////////////////////////////////////////////////

    class EntityPhysics : public IPhysics
    {
    public:
        EntityPhysics(PyObject *self);

        ///触发AI。
        virtual void onAITrigger() override;

        ///移动完毕
        virtual void onMoveToFinished(bool succed) override;

        virtual void onMoveToEntityFinish(bool succed) override;

        virtual void onSearchToFinished(bool succed) override;

        ///状态发生变化
        virtual void onStateChange(DWORD oldState) override;

    public:
        object_base m_self;
    };


    class PyEntityPhysics : public PyBase
    {
        LZPY_DEF(PyEntityPhysics, PyBase);
    public:
        PyEntityPhysics();
        ~PyEntityPhysics();

        LZPY_DEF_GET_MEMBER(source, m_source);

        LZPY_DEF_GET(state, m_physics->getState);
        LZPY_DEF_SET(state, m_physics->setState, DWORD);

        LZPY_DEF_GET(aiEnble, m_physics->getAIEnable);
        LZPY_DEF_SET(aiEnble, m_physics->setAIEnable, bool);

        LZPY_DEF_GET(enable, m_physics->isEnabled);
        LZPY_DEF_SET(enable, m_physics->enable, bool);

        LZPY_DEF_GET(aiInterval, m_physics->getAIInterval);
        LZPY_DEF_SET(aiInterval, m_physics->setAIInterval, float);


        LZPY_DEF_METHOD_1(searchPath);
        LZPY_DEF_METHOD_1(faceTo);
        LZPY_DEF_METHOD_1(faceToDir);
        LZPY_DEF_METHOD_1(moveTo);
        LZPY_DEF_METHOD_1(moveToEntity);
        LZPY_DEF_METHOD_0(breakAutoMove);

        RefPtr<EntityPhysics>   m_physics;
        object_ptr<PyEntity>    m_source;
    };


    ////////////////////////////////////////////////////////////////////
    //entity
    ////////////////////////////////////////////////////////////////////
    class Entity : public IEntity
    {
    public:
        Entity(PyObject * self);

        ///是否可以被鼠标选择
        virtual bool canSelect(void) const override;

        virtual bool ifPlayer(void) const override;

        virtual void onFocusCursor(UINT msg) override;

        object_base m_self;
    };

    class PyEntity : public PyBase
    {
        LZPY_DEF(PyEntity, PyBase);
    public:
        PyEntity();
        ~PyEntity();

        LZPY_DEF_GET(id, getId);

        LZPY_DEF_GETSET_MEMBER(position, m_entity->m_vPos);

        LZPY_DEF_GETSET_MEMBER(speed, m_entity->m_vSpeed);
        LZPY_DEF_GETSET_MEMBER(scale, m_entity->m_vScale);

        LZPY_DEF_GET_MEMBER(model, m_model);
        LZPY_DEF_SET(model, setModel, object);

        LZPY_DEF_GET_MEMBER(physics, m_physics);
        LZPY_DEF_SET(physics, setPhysics, object);

        LZPY_DEF_GET(fadeDistance, m_entity->getShowDistance);
        LZPY_DEF_SET(fadeDistance, m_entity->setShowDistance, float);

        LZPY_DEF_GET(lockHeight, m_entity->getLockHeight);
        LZPY_DEF_SET(lockHeight, m_entity->setLockHeight, float);

        LZPY_DEF_GET(visible, m_entity->visible);
        LZPY_DEF_SET(visible, m_entity->show, bool);

        LZPY_DEF_GET(distToCamera, m_entity->distToCamera);
        LZPY_DEF_GET(distToPlayer, m_entity->distToPlayer);

        LZPY_DEF_METHOD_1(lookAtPosition);

        int getId();
        void setModel(object model);
        void setPhysics(object physics);

        RefPtr<Entity>  m_entity;
        object_ptr<PyModel>  m_model;
        object_ptr<PyEntityPhysics>  m_physics;
    };

    class PyEntityMgr : public PyBase
    {
        LZPY_DEF(PyEntityMgr, PyBase);
    public:
        PyEntityMgr();
        ~PyEntityMgr();

        LZPY_DEF_METHOD_1(addEntity);
        LZPY_DEF_METHOD_1(delEntity);
        LZPY_DEF_METHOD_1(getEntity);

        bool addEntity(object entity);
        bool destroyEntity(object entity);

        dict m_entities;
    };


    void exportPyEntity(const char * module);
}