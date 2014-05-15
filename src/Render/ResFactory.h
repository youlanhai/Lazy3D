#pragma once

#include "Resource.h"

namespace Lazy
{

    class ResFactory : public IResourceFactory
    {
    public:
        ResFactory();
        ~ResFactory();

        virtual ResourcePtr create(int type, const std::wstring & source);
    };

}