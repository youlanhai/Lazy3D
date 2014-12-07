#include "stdafx.h"
#include "LPySceneNode.h"
#include "LPyLzd.h"

namespace Lzpy
{
    object build_object(SceneNode * v)
    {
        if (nullptr == v)
            return none_object;

        if (v->getScript())
            return v->getScript();

        LPySceneNode * p = new_instance_ex<LPySceneNode>();
        p->m_node = v;
        p->m_node->setScript(object_base(p));
        return new_reference(p);
    }

    bool parse_object(SceneNode *& v, object o)
    {
        if (o.is_none())
        {
            v = nullptr;
            return true;
        }

        if (!CHECK_INSTANCE(LPySceneNode, o.get()))
            return false;

        v = o.cast<LPySceneNode>()->m_node.get();
        return true;
    }

    object build_object(SceneNodePtr v)
    {
        return build_object(v.get());
    }

    bool parse_object(SceneNodePtr & v, object o)
    {
        SceneNode *p;
        bool ret = parse_object(p, o);
        v = p;
        return ret;
    }

    LZPY_CLASS_EXPORT(LPySceneNode)
    {
        LZPY_GET(type);
        LZPY_GET(inWorld);
        LZPY_GET(parent);
        LZPY_GET(look);
        LZPY_GET(up);
        LZPY_GET(right);


        LZPY_GETSET(name);
        LZPY_GETSET(position);
        LZPY_GETSET(scale);
        LZPY_GETSET(rotation);
        LZPY_GETSET(visible);
        LZPY_GETSET(aabb);

        LZPY_METHOD_1(findChild);
        LZPY_METHOD_1(addChild);
        LZPY_METHOD_1(delChild);

        LZPY_METHOD_0(clearChildren);
        LZPY_METHOD_0(removeFromeParent);

        LZPY_METHOD_1(saveToStream);
        LZPY_METHOD_1(loadFromStream);
    }

    LZPY_IMP_INIT(LPySceneNode)
    {
        PyErr_SetString(PyExc_RuntimeError, "LPySceneNode can't initialize directly!");
        return false;
    }

    LPySceneNode::LPySceneNode()
    {
    }


    LPySceneNode::~LPySceneNode()
    {
        if (m_node)
            m_node->setScript(null_object);
    }

    LZPY_IMP_METHOD_1(LPySceneNode, findChild)
    {
        std::string name;
        if (!parse_object(name, value))
            return null_object;

        SceneNodePtr child = m_node->findChild(name);
        return build_object(child);
    }

    LZPY_IMP_METHOD_1(LPySceneNode, addChild)
    {
        if (!CHECK_INSTANCE(LPySceneNode, value.get()))
            return null_object;

        m_node->addChild(value.cast<LPySceneNode>()->m_node);
        return none_object;
    }

    LZPY_IMP_METHOD_1(LPySceneNode, delChild)
    {
        if (!CHECK_INSTANCE(LPySceneNode, value.get()))
            return null_object;

        m_node->delChild(value.cast<LPySceneNode>()->m_node);
        return none_object;
    }

    LZPY_IMP_METHOD_0(LPySceneNode, clearChildren)
    {
        m_node->clearChildren();
        return none_object;
    }

    LZPY_IMP_METHOD_0(LPySceneNode, removeFromeParent)
    {
        m_node->removeFromParent();
        return none_object;
    }

    LZPY_IMP_METHOD_1(LPySceneNode, saveToStream)
    {
        if (!CHECK_INSTANCE(PyLazyData, value.get()))
            return null_object;

        m_node->saveToStream(value.cast<PyLazyData>()->m_data);
        return none_object;
    }

    LZPY_IMP_METHOD_1(LPySceneNode, loadFromStream)
    {
        if (!CHECK_INSTANCE(PyLazyData, value.get()))
            return null_object;

        bool ret = m_node->loadFromStream(value.cast<PyLazyData>()->m_data);
        return build_object(ret);
    }

} // end namespace Lzpy
