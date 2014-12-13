#pragma once

#include "../Core/SceneNode.h"
#include "LPyPhysics.h"

namespace Lazy
{
    object build_object(SceneNode * v);
    bool parse_object(SceneNode *& v, object o);

    class PySceneNode : public PyBase
    {
        LZPY_DEF(PySceneNode);
        
    public:

        PySceneNode();
        ~PySceneNode();

        LZPY_DEF_GET(type, m_object->getType);
        LZPY_DEF_GET(inWorld, m_object->inWorld);
        LZPY_DEF_GET(parent, m_object->getParent);

        LZPY_DEF_GET(name, m_object->getName);
        LZPY_DEF_SET(name, m_object->setName, std::string);
    
        LZPY_DEF_GET(position, m_object->getPosition);
        LZPY_DEF_SET(position, m_object->setPosition, Vector3);

        LZPY_DEF_GET(scale, m_object->getScale);
        LZPY_DEF_SET(scale, m_object->setScale, Vector3);

        LZPY_DEF_GET(rotation, m_object->getRotation);
        LZPY_DEF_SET(rotation, m_object->setRotation, Quaternion);

        LZPY_DEF_GET(look,  m_object->getLook);
        LZPY_DEF_GET(up,    m_object->getUp);
        LZPY_DEF_GET(right, m_object->getRight);

        LZPY_DEF_GET(visible, m_object->getVisible);
        LZPY_DEF_SET(visible, m_object->setVisible, bool);

        LZPY_DEF_GET(aabb, m_object->getBoundingBox);
        LZPY_DEF_SET(aabb, m_object->setBoundingBox, AABB);

        LZPY_DEF_METHOD_1(findChild);
        LZPY_DEF_METHOD_1(addChild);
        LZPY_DEF_METHOD_1(delChild);
        LZPY_DEF_METHOD_0(clearChildren);
        LZPY_DEF_METHOD_0(removeFromeParent);

        LZPY_DEF_METHOD_1(saveToStream);
        LZPY_DEF_METHOD_1(loadFromStream);

        SceneNode * m_object;
    };

    BUILD_AND_PARSE_SCRIPT_OBJECT(PySceneNode, SceneNode);

}// end namespace Lazy
