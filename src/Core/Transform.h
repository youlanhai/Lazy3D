#pragma once

#include "RenderObj.h"

namespace Lazy
{

    class Transform
    {
    public:
        Transform();
        ~Transform();

        void setScale(const Vector3 & scale);
        const Vector3 & getScale() const{ return m_scale; }

        void setPosition(const Vector3 & position);
        const Vector3 & getPosition() const{ return m_position; }

        void setRotation(const Quaternion & rotation);
        const Quaternion & getRotation() const { return m_rotation; }

        void setMatrix(const Matrix & matrix);
        const Matrix & getMatrix() const;

        void getLook(Vector3 & look);
        Vector3 getLook();

        void getUp(Vector3 & up);
        Vector3 getUp();

        void getRight(Vector3 & right);
        Vector3 getRight();

        void rotationX(float angle);
        void rotationY(float angle);
        void rotationZ(float angle);
        void rotationLook(float angle);
        void rotationUp(float angle);
        void rotationRight(float angle);
        void rotationAxis(const Vector3 & axis, float angle);

        void moveLook(float delta);
        void moveRight(float delta);
        void moveUp(float delta);

    private:
        Vector3                 m_position;
        Vector3                 m_scale;
        Quaternion              m_rotation;

        mutable Matrix          m_matrix;
        mutable bool            m_matrixDirty;

    };

    class SceneNode :
        public IBase,
        public Transform,
        public IRenderable
    {
    public:

    };


} // end namespace Lazy
