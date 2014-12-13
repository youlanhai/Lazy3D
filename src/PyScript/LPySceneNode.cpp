#include "stdafx.h"
#include "LPySceneNode.h"
#include "LPyLzd.h"

namespace Lazy
{
    LZPY_CLASS_EXPORT(PySceneNode)
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

    LZPY_IMP_INIT(PySceneNode)
    {
        PyErr_SetString(PyExc_RuntimeError, "PySceneNode can't initialize directly!");
        return false;
    }

    PySceneNode::PySceneNode()
        : m_object(nullptr)
    {
    }


    PySceneNode::~PySceneNode()
    {
    }

    LZPY_IMP_METHOD_1(PySceneNode, findChild)
    {
        std::string name;
        if (!parse_object(name, value))
            return null_object;

        SceneNode* child = m_object->findChild(name);
        return build_object(child);
    }

    LZPY_IMP_METHOD_1(PySceneNode, addChild)
    {
        if (!CHECK_INSTANCE(PySceneNode, value.get()))
            return null_object;

        m_object->addChild(value.cast<PySceneNode>()->m_object);
        return none_object;
    }

    LZPY_IMP_METHOD_1(PySceneNode, delChild)
    {
        if (!CHECK_INSTANCE(PySceneNode, value.get()))
            return null_object;

        m_object->delChild(value.cast<PySceneNode>()->m_object);
        return none_object;
    }

    LZPY_IMP_METHOD_0(PySceneNode, clearChildren)
    {
        m_object->clearChildren();
        return none_object;
    }

    LZPY_IMP_METHOD_0(PySceneNode, removeFromeParent)
    {
        m_object->removeFromParent();
        return none_object;
    }

    LZPY_IMP_METHOD_1(PySceneNode, saveToStream)
    {
        if (!CHECK_INSTANCE(PyLazyData, value.get()))
            return null_object;

        m_object->saveToStream(value.cast<PyLazyData>()->m_data);
        return none_object;
    }

    LZPY_IMP_METHOD_1(PySceneNode, loadFromStream)
    {
        if (!CHECK_INSTANCE(PyLazyData, value.get()))
            return null_object;

        bool ret = m_object->loadFromStream(value.cast<PyLazyData>()->m_data);
        return build_object(ret);
    }

} // end namespace Lazy
