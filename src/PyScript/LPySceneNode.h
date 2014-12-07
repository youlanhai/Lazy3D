#pragma once

#include "../Core/SceneNode.h"
#include "LPyPhysics.h"

namespace Lzpy
{
    using namespace Lazy;

    object build_object(SceneNode * v);
    bool parse_object(SceneNode *& v, object o);

    object build_object(SceneNodePtr v);
    bool parse_object(SceneNodePtr & v, object o);

    class LPySceneNode : public PyBase
    {
        LZPY_DEF(LPySceneNode);
        
    public:

        LPySceneNode();
        ~LPySceneNode();

        LZPY_DEF_GET(type, m_node->getType);
        LZPY_DEF_GET(inWorld, m_node->inWorld);
        LZPY_DEF_GET(parent, m_node->getParent);

        LZPY_DEF_GET(name, m_node->getName);
        LZPY_DEF_SET(name, m_node->setName, std::string);
    
        LZPY_DEF_GET(position, m_node->getPosition);
        LZPY_DEF_SET(position, m_node->setPosition, Vector3);

        LZPY_DEF_GET(scale, m_node->getScale);
        LZPY_DEF_SET(scale, m_node->setScale, Vector3);

        LZPY_DEF_GET(rotation, m_node->getRotation);
        LZPY_DEF_SET(rotation, m_node->setRotation, Quaternion);

        LZPY_DEF_GET(look,  m_node->getLook);
        LZPY_DEF_GET(up,    m_node->getUp);
        LZPY_DEF_GET(right, m_node->getRight);

        LZPY_DEF_GET(visible, m_node->getVisible);
        LZPY_DEF_SET(visible, m_node->setVisible, bool);

        LZPY_DEF_GET(aabb, m_node->getBoundingBox);
        LZPY_DEF_SET(aabb, m_node->setBoundingBox, AABB);

        LZPY_DEF_METHOD_1(findChild);
        LZPY_DEF_METHOD_1(addChild);
        LZPY_DEF_METHOD_1(delChild);
        LZPY_DEF_METHOD_0(clearChildren);
        LZPY_DEF_METHOD_0(removeFromeParent);

        LZPY_DEF_METHOD_1(saveToStream);
        LZPY_DEF_METHOD_1(loadFromStream);

        SceneNodePtr m_node;
    };

}// end namespace Lzpy
