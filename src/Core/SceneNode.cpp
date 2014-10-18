#include "stdafx.h"
#include "SceneNode.h"

namespace Lazy
{


    SceneNode::SceneNode()
        : m_scale(1.0f, 1.0f, 1.0f)
        , m_rotation(0, 0, 0, 1)
        , m_position(0, 0, 0)
        , m_matrixDirty(0)
        , m_matrix(matIdentity)
        , m_visible(1)
    {
        m_aabb.zero();
    }

    SceneNode::~SceneNode()
    {
    }

    void SceneNode::setScale(const Vector3 & scale)
    {
        m_scale = scale;
        m_matrixDirty = 1;
    }

    void SceneNode::setPosition(const Vector3 & position)
    {
        m_position = position;
        m_matrixDirty = 1;
    }

    void SceneNode::setRotation(const Quaternion & rotation)
    {
        m_rotation = rotation;
        m_matrixDirty = 1;
    }

    void SceneNode::setMatrix(const Matrix & matrix)
    {
        m_matrix = matrix;
        m_matrix.decompose(m_scale, m_rotation, m_position);
        m_matrixDirty = 0;
    }

    const Matrix & SceneNode::getMatrix() const
    {
        if (m_matrixDirty)
        {
            m_matrixDirty = 0;

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
        getUp(right);
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

        m_rotation *= quat;
        m_matrixDirty = 1;
    }

    void SceneNode::moveLook(float delta)
    {
        m_position += getLook() * delta;
        m_matrixDirty = 1;
    }

    void SceneNode::moveRight(float delta)
    {
        m_position += getRight() * delta;
        m_matrixDirty = 1;
    }

    void SceneNode::moveUp(float delta)
    {
        m_position += getUp() * delta;
        m_matrixDirty = 1;
    }

    void SceneNode::move(const Vector3 & delta)
    {
        m_position += delta;
        m_matrixDirty = 1;
    }

    AABB SceneNode::getWorldBoundingBox() const
    {
        AABB aabb;
        m_aabb.applyMatrix(aabb, getMatrix());
        return aabb;
    }

} // end namespace Lazy
