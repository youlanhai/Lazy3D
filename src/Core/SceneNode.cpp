#include "stdafx.h"
#include "Transform.h"

namespace Lazy
{


    Transform::Transform()
        : m_scale(1.0f, 1.0f, 1.0f)
        , m_matrixDirty(false)
    {

    }

    Transform::~Transform()
    {
    }

    void Transform::setScale(const Vector3 & scale)
    {
        m_scale = scale;
        m_matrixDirty = true;
    }

    void Transform::setPosition(const Vector3 & position)
    {
        m_position = position;
        m_matrixDirty = true;
    }

    void Transform::setRotation(const Quaternion & rotation)
    {
        m_rotation = rotation;
        m_matrixDirty = true;
    }

    void Transform::setMatrix(const Matrix & matrix)
    {
        m_matrix = matrix;
        m_matrix.decompose(m_scale, m_rotation, m_position);
        m_matrixDirty = false;
    }

    const Matrix & Transform::getMatrix() const
    {
        if (m_matrixDirty)
        {
            m_matrixDirty = false;
            m_matrix.makeScale(m_scale);

            Matrix temp;
            temp.setRotationQuaternion(m_rotation);
            m_matrix *= temp;

            m_matrix._41 = m_position.x;
            m_matrix._42 = m_position.y;
            m_matrix._43 = m_position.z;
            m_matrix._44 = 1.0f;
        }
        return m_matrix;
    }

    void Transform::getLook(Vector3 & look)
    {
        getMatrix().getRow(2, look);
        look.normalize();
    }

    Vector3 Transform::getLook()
    {
        Vector3 look;
        getLook(look);
        return look;
    }

    void Transform::getUp(Vector3 & up)
    {
        getMatrix().getRow(1, up);
        up.normalize();
    }

    Vector3 Transform::getUp()
    {
        Vector3 up;
        getUp(up);
        return up;
    }

    void Transform::getRight(Vector3 & right)
    {
        getMatrix().getRow(0, right);
        right.normalize();
    }

    Vector3 Transform::getRight()
    {
        Vector3 right;
        getUp(right);
        return right;
    }

    void Transform::rotationX(float angle)
    {
        rotationAxis(Vector3(1, 0, 0), angle);
    }

    void Transform::rotationY(float angle)
    {
        rotationAxis(Vector3(0, 1, 0), angle);
    }

    void Transform::rotationZ(float angle)
    {
        rotationAxis(Vector3(0, 0, 1), angle);
    }

    void Transform::rotationLook(float angle)
    {
        rotationAxis(getLook(), angle);
    }

    void Transform::rotationUp(float angle)
    {
        rotationAxis(getUp(), angle);
    }

    void Transform::rotationRight(float angle)
    {
        rotationAxis(getRight(), angle);
    }

    void Transform::rotationAxis(const Vector3 & axis, float angle)
    {
        Quaternion quat;
        quat.setRotationAxis(axis, angle);

        m_rotation *= quat;
    }

    void Transform::moveLook(float delta)
    {
        m_position += getLook() * delta;
    }

    void Transform::moveRight(float delta)
    {
        m_position += getRight() * delta;
    }

    void Transform::moveUp(float delta)
    {
        m_position += getUp() * delta;
    }


} // end namespace Lazy
