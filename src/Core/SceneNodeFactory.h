#pragma once

#include "SceneNode.h"
#include <hash_map>

namespace Lazy
{
    namespace SceneNodeType
    {
        const int Empty = 0;
        const int Model = 1;
        const int Entity = 2;

        const int UserType = 0xff;
    }

    class SceneNodeFactory
    {
    public:
        typedef SceneNode* (*Creator)(void);

        SceneNodeFactory();
        ~SceneNodeFactory();

        static void init();
        static SceneNode * create(int type);

    private:
        typedef std::hash_map<int, Creator> Factory;
        static Factory s_factory;
    };

} // end namespace Lazy
