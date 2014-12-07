#pragma once

#include "../Core/SceneNode.h"

namespace Lzpy
{
    using namespace Lazy;

    class LPySceneNode : public PyBase
    {
        LZPY_DEF(LPySceneNode);
        
    public:

        LPySceneNode();
        ~LPySceneNode();



    
        SceneNodePtr m_node;
    };

}// end namespace Lzpy
