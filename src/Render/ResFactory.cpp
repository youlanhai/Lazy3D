#include "stdafx.h"
#include "ResFactory.h"

#include "Texture.h"
#include "Mesh.h"
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

        case rt::mesh:
            return new Mesh(source);

        case rt::effect:
            return new Effect(source);

        default:
            break;
        };

        return nullptr;
    }

}