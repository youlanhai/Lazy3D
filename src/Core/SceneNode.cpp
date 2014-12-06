#include "stdafx.h"
#include "SceneNode.h"
#include "../Math/MathTool.h"

namespace Lazy
{

    SceneNode::SceneNode()
        : m_scale(1.0f, 1.0f, 1.0f)
        , m_rotation(0, 0, 0, 1)
        , m_position(0, 0, 0)
        , m_matrixDirty(0)
        , m_matrix(matIdentity)
        , m_matWorld(matIdentity)
        , m_visible(TRUE)
        , m_inWorld(FALSE)
        , m_parent(nullptr)
    {
        m_aabb.zero();
    }

    SceneNode::~SceneNode()
    {
        removeFromParent();
        clearChildren();
    }

    void SceneNode::setScale(const Vector3 & scale)
    {
        m_scale = scale;
        setDirtyFlag(DirtyAll);
    }

    void SceneNode::setPosition(const Vector3 & position)
    {
        m_position = position;
        setDirtyFlag(DirtyAll);
    }

    void SceneNode::setRotation(const Quaternion & rotation)
    {
        m_rotation = rotation;
        setDirtyFlag(DirtyAll);
    }

    void SceneNode::setMatrix(const Matrix & matrix)
    {
        m_matrix = matrix;
        m_matrix.decompose(m_scale, m_rotation, m_position);
        setDirtyFlag(DirtyWorld);
    }

    const Matrix & SceneNode::getMatrix() const
    {
        if (m_matrixDirty & DirtyLocal)
        {
            m_matrixDirty &= ~DirtyLocal;

            m_matrix.setRotationQuaternion(m_rotation);
            m_matrix[0] *= m_scale.x;
            m_matrix[1] *= m_scale.y;
            m_matrix[2] *= m_scale.z;

            m_matrix._41 = m_position.x;
            m_matrix._42 = m_position.y;
            m_matrix._43 = m_position.z;
            m_matrix._44 = 1.0f;
        }
        return m_matrix;
    }

    void SceneNode::genViewMatrix(Matrix & mat) const
    {
        mat.setRotationQuaternion(m_rotation);

        Vector3 pos;
        pos.x = -m_position.dot(mat[0]);
        pos.y = -m_position.dot(mat[1]);
        pos.z = -m_position.dot(mat[2]);

        mat.transpose();
        mat[3] = pos;
    }

    const Matrix & SceneNode::getWorldMatrix(void) const
    {
        if (m_matrixDirty & DirtyWorld)
        {
            m_matWorld = getMatrix();
            if (m_parent)
                m_matWorld.postMultiply(m_parent->getWorldMatrix());

            m_matrixDirty &= ~DirtyWorld;
        }
        return m_matWorld;
    }

    void SceneNode::getLook(Vector3 & look) const
    {
        getMatrix().getRow(2, look);
        look.normalize();
    }

    Vector3 SceneNode::getLook() const
    {
        Vector3 look;
        getLook(look);
        return look;
    }

    void SceneNode::getUp(Vector3 & up) const
    {
        getMatrix().getRow(1, up);
        up.normalize();
    }

    Vector3 SceneNode::getUp() const
    {
        Vector3 up;
        getUp(up);
        return up;
    }

    void SceneNode::getRight(Vector3 & right) const
    {
        getMatrix().getRow(0, right);
        right.normalize();
    }

    Vector3 SceneNode::getRight() const
    {
        Vector3 right;
        getRight(right);
        return right;
    }

    void SceneNode::rotationX(float angle)
    {
        rotationAxis(Vector3(1, 0, 0), angle);
    }

    void SceneNode::rotationY(float angle)
    {
        rotationAxis(Vector3(0, 1, 0), angle);
    }

    void SceneNode::rotationZ(float angle)
    {
        rotationAxis(Vector3(0, 0, 1), angle);
    }

    void SceneNode::rotationLook(float angle)
    {
        rotationAxis(getLook(), angle);
    }

    void SceneNode::rotationUp(float angle)
    {
        rotationAxis(getUp(), angle);
    }

    void SceneNode::rotationRight(float angle)
    {
        rotationAxis(getRight(), angle);
    }

    void SceneNode::rotationAxis(const Vector3 & axis, float angle)
    {
        Quaternion quat;
        quat.setRotationAxis(axis, angle);

        setRotation( m_rotation * quat );
    }

    void SceneNode::moveLook(float delta)
    {
        setPosition(m_position + getLook() * delta);
    }

    void SceneNode::moveRight(float delta)
    {
        setPosition(m_position + getRight() * delta);
    }

    void SceneNode::moveUp(float delta)
    {
        setPosition(m_position + getUp() * delta);
    }

    void SceneNode::move(const Vector3 & delta)
    {
        setPosition(m_position + delta);
    }

    AABB SceneNode::getWorldBoundingBox() const
    {
        AABB aabb;
        m_aabb.applyMatrix(aabb, getWorldMatrix());
        return aabb;
    }

    SceneNodePtr SceneNode::findChild(const std::string & name)
    {
        SceneNodePtr child;
        size_t pos = name.find("/");
        size_t len = (pos != name.npos ? pos : name.size());
        const char * str = name.c_str();

        for (SceneNodePtr it : m_children)
        {
            const std::string & dst = it->m_name;
            if (dst.size() == len && strncmp(dst.c_str(), str, len))
            {
                child = it;
                break;
            }
        }

        if (pos != name.npos)
            child = findChild(name.substr(pos + 1));
        return child;
    }

    void SceneNode::addChild(SceneNodePtr child)
    {
        assert(!child->inWorld() && "SceneNode::addChild - the child has been in world.");
        m_children.add(child);
        child->setParent(this);

        if (m_inWorld)
            child->onEnterWorld();
    }

    void SceneNode::delChild(SceneNodePtr child)
    {
        assert(child->m_parent == this && "SceneNode::delChild - the child doens't owned by this node.");

        if (child->inWorld())
            child->onLeaveWorld();

        m_children.remove(child);
        child->setParent(nullptr);
    }

    void SceneNode::delChildByName(const std::string & name)
    {
        SceneNodePtr child = findChild(name);
        if (child)
            delChild(child);
    }

    void SceneNode::clearChildren()
    {
        if (this->inWorld())
            this->onLeaveWorld();

        for (SceneNodePtr child : m_children)
            child->setParent(nullptr);
        m_children.clear();
    }

    void SceneNode::removeFromParent()
    {
        if (m_parent)
            m_parent->delChild(this);
    }

    /** 当结点加入场景树后，会被调用。*/
    void SceneNode::onEnterWorld()
    {
        assert(!m_inWorld && "SceneNode::onEnterWorld - this node has been in world.");
        m_inWorld = true;

        m_children.lock();
        for (SceneNodePtr child : m_children)
            child->onEnterWorld();
        m_children.unlock();
    }

    /** 当结点从场景树移除之前，会被调用。*/
    void SceneNode::onLeaveWorld()
    {
        assert(m_inWorld && "SceneNode::onLeaveWorld - this node has been leave world.");
        m_inWorld = false;

        m_children.lock();
        for (SceneNodePtr child : m_children)
            child->onLeaveWorld();
        m_children.unlock();
    }

    void SceneNode::setParent(SceneNode * parent)
    {
        m_parent = parent;
        setDirtyFlag(DirtyWorld);
    }

    void SceneNode::setDirtyFlag(int flag)
    {
        int oldFlag = m_matrixDirty;
        m_matrixDirty |= flag;

        if (oldFlag == 0 && m_matrixDirty != 0)
        {
            for (SceneNodePtr child : m_children)
                child->setDirtyFlag(DirtyWorld);
        }
    }

    ///画面渲染
    void SceneNode::render(IDirect3DDevice9 * pDevice)
    {
        m_children.lock();
        for (SceneNodePtr child : m_children)
        {
            if(child->getVisible()) child->render(pDevice);
        }
        m_children.unlock();
    }

    ///逻辑更新
    void SceneNode::update(float elapse)
    {
        m_children.lock();
        for (SceneNodePtr child : m_children)
        {
            if (child->getVisible()) child->update(elapse);
        }
        m_children.unlock();
    }

    void SceneNode::saveToStream(LZDataPtr data)
    {
        data->writeInt(L"type", getType());
        if (!m_name.empty())
            data->writeString(L"name", charToWChar(m_name));

        writeQuaternion(data, L"rotation", m_rotation);
        writeVector3(data, L"scale", m_scale);
        writeVector3(data, L"position", m_position);
        writeAABB(data, L"bb", m_aabb);
    }

    bool SceneNode::loadFromStream(LZDataPtr data)
    {
        setDirtyFlag(DirtyAll);

        m_name = wcharToChar( data->readString(L"name") );

        readQuaternion(data, L"rotation", m_rotation);
        readVector3(data, L"scale", m_scale);
        readVector3(data, L"position", m_position);
        readAABB(data, L"bb", m_aabb);

        return true;
    }

} // end namespace Lazy
