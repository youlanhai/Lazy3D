#include "stdafx.h"
#include "SceneNodeFactory.h"

#include "AnimModel.h"
#include "Entity.h"

namespace Lazy
{
    /*static*/ SceneNodeFactory::Factory SceneNodeFactory::s_factory;

    SceneNodeFactory::SceneNodeFactory()
    {
    }


    SceneNodeFactory::~SceneNodeFactory()
    {
    }
    
    /*static*/ void SceneNodeFactory::init()
    {
#define REG_CREATOR(TYPE, CLASS) s_factory[SceneNodeType::TYPE] = (Creator) CLASS::createInstance;

        REG_CREATOR(Empty, SceneNode);
        REG_CREATOR(Model, AnimalModel);
        REG_CREATOR(Entity, IEntity);

#undef REG_CREATOR
    }

    /*static*/ SceneNode * SceneNodeFactory::create(int type)
    {
        Factory::iterator it = s_factory.find(type);
        if (it != s_factory.end())
            return (it->second)();

        assert(false && "SceneNodeFactory::create - invalid type.");
        return nullptr;
    }


} // end namespace Lazy
