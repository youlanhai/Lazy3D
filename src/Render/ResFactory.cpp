#include "stdafx.h"
#include "ResFactory.h"

#include "Texture.h"
#include "StaticMesh.h"
#include "SkinMesh.h"
#include "Effect.h"

namespace Lazy
{

    ResFactory::ResFactory()
    {
    }


    ResFactory::~ResFactory()
    {
    }

    ResourcePtr ResFactory::create(int type, const std::wstring & source)
    {
        switch (type)
        {
        case rt::texture: 
            return new Texture(source);

        case rt::staticmesh: 
            return new StaticMesh(source);

        case rt::skinmesh: 
            return new SkinMesh(source);

        case rt::effect:
            return new Effect(source);

        default:
            break;
        };

        return nullptr;
    }

}