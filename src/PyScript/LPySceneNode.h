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

    class PySceneNode : public PyBase
    {
        LZPY_DEF(PySceneNode);
        
    public:

        PySceneNode();
        ~PySceneNode();

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



    template<typename TPy, typename TCxx>
    object build_scene_node(TCxx * v)
    {
        if (nullptr == v)
            return none_object;

        if (v->getScript())
            return object( v->getScript() );

        TPy * p = new_instance_ex<TPy>();
        p->m_node = v;
        p->m_node->setScript(p);
        return new_reference(p);
    }

    template<typename TPy, typename TCxx>
    bool parse_scene_node(TCxx *& v, object o)
    {
        if (o.is_none())
        {
            v = nullptr;
            return true;
        }

        if (!CHECK_INSTANCE(TPy, o.get()))
            return false;

        v = static_cast<TCxx*>( o.cast<TPy>()->m_node.get() );
        return true;
    }


#define BUILD_AND_PARSE_SCENE_NODE(TYPE_PY, TYPE_CXX) \
    inline object build_object(TYPE_CXX * v)\
    {\
        return build_scene_node<TYPE_PY, TYPE_CXX>(v); \
    }\
    inline bool parse_object(TYPE_CXX *& v, object o)\
    {\
        return parse_scene_node<TYPE_PY, TYPE_CXX>(v, o); \
    }\
    inline object build_object(RefPtr<TYPE_CXX> v)\
    {\
        return build_scene_node<TYPE_PY, TYPE_CXX>(v.get()); \
    }\
    inline bool parse_object(RefPtr<TYPE_CXX> & v, object o)\
    {\
        TYPE_CXX *p = nullptr; \
        bool ret = parse_scene_node<TYPE_PY, TYPE_CXX>(p, o); \
        v = p; \
        return ret; \
    }


    BUILD_AND_PARSE_SCENE_NODE(PySceneNode, SceneNode);

}// end namespace Lzpy
