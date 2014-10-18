#pragma once

#include "Base.h"
#include "RenderObj.h"

namespace Lazy
{

    class SceneNode : public IBase, public IRenderable
    {
    public:
        SceneNode();
        ~SceneNode();

        void setScale(const Vector3 & scale);
        const Vector3 & getScale() const{ return m_scale; }

        void setPosition(const Vector3 & position);
        const Vector3 & getPosition() const{ return m_position; }

        void setRotation(const Quaternion & rotation);
        const Quaternion & getRotation() const { return m_rotation; }

        void setMatrix(const Matrix & matrix);
        const Matrix & getMatrix() const;
        void genViewMatrix(Matrix & mat) const;

        void getLook(Vector3 & look) const;
        Vector3 getLook() const;

        void getUp(Vector3 & up) const;
        Vector3 getUp() const;

        void getRight(Vector3 & right) const;
        Vector3 getRight() const;

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
        void move(const Vector3 & delta);

        void setVisible(bool visible) { m_visible = visible ? 0 : 1; }
        bool getVisible() const { return m_visible != 0; }

        void setBoundingBox(const AABB & aabb){ m_aabb = aabb; }
        const AABB & getBoundingBox() const { return m_aabb; }
        AABB getWorldBoundingBox() const;

    private:
        Vector3                 m_position;
        Vector3                 m_scale;
        Quaternion              m_rotation;

        mutable Matrix          m_matrix;
        mutable int             m_matrixDirty;

    protected:
        int                     m_visible;
        AABB                    m_aabb;
    };


} // end namespace Lazy
