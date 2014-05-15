#pragma once

#include "../Physics/Physics.h"

namespace LazyPy
{
    class PyEntity;


    ////////////////////////////////////////////////////////////////////
    //model
    ////////////////////////////////////////////////////////////////////

    class PyModel : public PyBase
    {
        LAZYPY_DEF(PyModel, PyBase);
    public:
        PyModel();

        LAZYPY_DEF_GET(yaw, m_model->getYaw);
        LAZYPY_DEF_SET(yaw, m_model->setYaw, float);

        LAZYPY_DEF_GET(pitch, m_model->getPitch);
        LAZYPY_DEF_SET(pitch, m_model->setPitch, float);

        LAZYPY_DEF_GET(roll, m_model->getRoll);
        LAZYPY_DEF_SET(roll, m_model->setRoll, float);

        LAZYPY_DEF_GET(scale, m_model->getScale);
        LAZYPY_DEF_SET(scale, m_model->setScale, float);

        LAZYPY_DEF_GET(resource, m_model->source);

        LAZYPY_DEF_METHOD_0(isSkeleton);
        LAZYPY_DEF_METHOD_0(isMesh);
        LAZYPY_DEF_METHOD_1(showBound);
        LAZYPY_DEF_METHOD(playAction);
        LAZYPY_DEF_METHOD_0(stopAction);
        LAZYPY_DEF_METHOD_0(getActionCount);
        LAZYPY_DEF_METHOD_1(getActionName); 
        LAZYPY_DEF_METHOD_1(setAnimSpeed);
        LAZYPY_DEF_METHOD_0(getAAABB);
        LAZYPY_DEF_METHOD_0(getHeight);
        
        ModelPtr m_model;
    };

    ////////////////////////////////////////////////////////////////////
    //physics
    ////////////////////////////////////////////////////////////////////

    class EntityPhysics : public iPhysics
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
        PyObject *m_self;
    };


    class PyEntityPhysics : public PyBase
    {
        LAZYPY_DEF(PyEntityPhysics, PyBase);
    public:
        PyEntityPhysics();
        ~PyEntityPhysics();

        LAZYPY_DEF_GET_MEMBER(source, m_source);

        LAZYPY_DEF_GET(state, m_physics->getState);
        LAZYPY_DEF_SET(state, m_physics->setState, DWORD);

        LAZYPY_DEF_GET(aiEnble, m_physics->isAIEnable);
        LAZYPY_DEF_SET(aiEnble, m_physics->enableAI, bool);

        LAZYPY_DEF_GET(enable, m_physics->isEnabled);
        LAZYPY_DEF_SET(enable, m_physics->enable, bool);

        LAZYPY_DEF_GET(aiInterval, m_physics->getAIInterval);
        LAZYPY_DEF_SET(aiInterval, m_physics->setAIInterval, float);


        LAZYPY_DEF_METHOD_1(searchPath);
        LAZYPY_DEF_METHOD_1(faceTo);
        LAZYPY_DEF_METHOD_1(faceToDir);
        LAZYPY_DEF_METHOD_1(moveTo);
        LAZYPY_DEF_METHOD_1(moveToEntity);
        LAZYPY_DEF_METHOD_0(breakAutoMove);

        void setSource(PyEntity *source);

        RefPtr<EntityPhysics> m_physics;
        object_ptr<PyEntity> m_source;
    };


    ////////////////////////////////////////////////////////////////////
    //entity
    ////////////////////////////////////////////////////////////////////

    bool PyEntity_Check(PyObject *value, bool setError = true);

    class Entity : public iEntity
    {
    public:
        Entity(PyObject * self)
            : m_self(self)
        {

        }

        ///是否可以被鼠标选择
        virtual bool canSelect(void) const override
        {
            object self = borrow_reference(m_self);

            if (!self.hasattr("canSelect")) return false;

            return self.call_method("canSelect");
        }

        virtual bool isPlayer(void) override
        {
            object self = borrow_reference(m_self);

            if (!self.hasattr("isPlayer")) return false;

            return self.call_method("isPlayer");
        }

        virtual void onFocusCursor(UINT msg) override
        {
            object self = borrow_reference(m_self);

            if (!self.hasattr("onFocusCursor")) return;

            self.call_method("onFocusCursor", msg);
        }

        PyObject *m_self;
    };

    class PyEntity : public PyBase
    {
        LAZYPY_DEF(PyEntity, PyBase);
    public:
        PyEntity();
        ~PyEntity();

        LAZYPY_DEF_GET(id, getId);

        LAZYPY_DEF_GETSET(position);


        LAZYPY_DEF_GETSET(speed);
        LAZYPY_DEF_GETSET(scale);
        LAZYPY_DEF_GETSET(model);
        LAZYPY_DEF_GETSET(physics);
        LAZYPY_DEF_GETSET(fadeDistance);
        LAZYPY_DEF_GETSET(lockHeight);

        LAZYPY_DEF_METHOD_1(show);
        LAZYPY_DEF_METHOD_0(visible);
        LAZYPY_DEF_METHOD_0(distToCamera);
        LAZYPY_DEF_METHOD_0(distToPlayer);
        LAZYPY_DEF_METHOD_1(lookAtPosition);

        int getId();

        RefPtr<Entity>  m_entity;
        object_ptr<PyModel>         m_model;
        object_ptr<PyEntityPhysics> m_physics;
    };

    class PyEntityMgr : public PyBase
    {
        LAZYPY_DEF(PyEntityMgr, PyBase);
    public:
        PyEntityMgr();
        ~PyEntityMgr();

        LAZYPY_DEF_METHOD_1(addEntity);
        LAZYPY_DEF_METHOD_1(delEntity);
        LAZYPY_DEF_METHOD_1(getEntity);

        PyObject *m_pDict;
    };
}