#pragma once

#include "../Core/Entity.h"
#include "LPyPhysics.h"
#include "LPyModel.h"

namespace Lzpy
{
    using namespace Lazy;

    class PyEntity;


    ////////////////////////////////////////////////////////////////////
    //physics
    ////////////////////////////////////////////////////////////////////
    class PyEntityPhysics : public PyBase
    {
        LZPY_DEF(PyEntityPhysics);
    public:
        PyEntityPhysics();
        ~PyEntityPhysics();

        LZPY_DEF_GET(source, m_object->getSource);

        LZPY_DEF_GET(state, m_object->getState);
        LZPY_DEF_SET(state, m_object->setState, DWORD);

        LZPY_DEF_GET(aiEnble, m_object->getAIEnable);
        LZPY_DEF_SET(aiEnble, m_object->setAIEnable, bool);

        LZPY_DEF_GET(enable, m_object->isEnabled);
        LZPY_DEF_SET(enable, m_object->enable, bool);

        LZPY_DEF_GET(aiInterval, m_object->getAIInterval);
        LZPY_DEF_SET(aiInterval, m_object->setAIInterval, float);


        LZPY_DEF_METHOD_1(searchPath);
        LZPY_DEF_METHOD_1(faceTo);
        LZPY_DEF_METHOD_1(faceToDir);
        LZPY_DEF_METHOD_1(moveTo);
        LZPY_DEF_METHOD_1(moveToEntity);
        LZPY_DEF_METHOD_0(breakAutoMove);

        IPhysics*   m_object;
    };

    BUILD_AND_PARSE_SCRIPT_OBJECT(PyEntityPhysics, IPhysics);
    ////////////////////////////////////////////////////////////////////
    //entity
    ////////////////////////////////////////////////////////////////////
    class PyEntity : public PySceneNode
    {
        LZPY_DEF(PyEntity);
    public:
        PyEntity();
        ~PyEntity();

        inline IEntity * entity() { return (IEntity*) m_object; }

        LZPY_DEF_GET(id, entity()->getID);

        LZPY_DEF_GET(model, entity()->getModel);
        LZPY_DEF_SET(model, entity()->setModel, IModel*);

        LZPY_DEF_GET(physics, entity()->getPhysics);
        LZPY_DEF_SET(physics, entity()->setPhysics, IPhysics*);

        LZPY_DEF_GET(fadeDistance, entity()->getShowDistance);
        LZPY_DEF_SET(fadeDistance, entity()->setShowDistance, float);

        LZPY_DEF_GET(distToCamera, entity()->distToCamera);
        LZPY_DEF_GET(distToPlayer, entity()->distToPlayer);

        LZPY_DEF_METHOD_1(lookAtPosition);

        int getID() { return entity()->getID(); }
    };

    BUILD_AND_PARSE_SCRIPT_OBJECT(PyEntity, IEntity);
    ////////////////////////////////////////////////////////////////////
    //entity mgr
    ////////////////////////////////////////////////////////////////////
    class PyEntityMgr : public PyBase
    {
        LZPY_DEF(PyEntityMgr);
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

}