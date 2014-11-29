#pragma once

#include "Base.h"
#include "RenderInterface.h"
#include "../utility/VisitPool.h"

namespace Lazy
{
    class SceneNode;
    typedef RefPtr<SceneNode> SceneNodePtr;

    /** ������㡣
     *  һ��������ӵ�������ӽ�㣬�Ӷ�����һ����������
     *  ���ڳ������ϵĽ�㣬ÿ֡�����Ա����£�update������Ⱦ��render����
     */
    class SceneNode : public IBase, public IRenderable
    {
    public:
        typedef VisitPool<SceneNodePtr> TChildren;
        enum DirtyFlag
        {
            DirtyLocal = 1 << 0,
            DirtyWorld = 1 << 1,

            DirtyAll = DirtyLocal | DirtyWorld,
        };

        SceneNode();
        ~SceneNode();

        void    setScale(const Vector3 & scale);
        const Vector3 & getScale() const{ return m_scale; }

        void    setPosition(const Vector3 & position);
        const Vector3 & getPosition() const{ return m_position; }

        void    setRotation(const Quaternion & rotation);
        const Quaternion & getRotation() const { return m_rotation; }

        void    setMatrix(const Matrix & matrix);
        const Matrix & getMatrix() const;
        void    genViewMatrix(Matrix & mat) const;

        const Matrix & getWorldMatrix(void) const;

        void    getLook(Vector3 & look) const;
        Vector3 getLook() const;

        void    getUp(Vector3 & up) const;
        Vector3 getUp() const;

        void    getRight(Vector3 & right) const;
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

        SceneNode * getParent() { return m_parent; }
        const SceneNode * getParent() const { return m_parent; }
        const std::string & getName() const { return m_name; }
        BOOL inWorld() const { return m_inWorld; }

        /** �������Ʋ����ӽ�㡣name����Ϊ·����ʽ���� "parent/child1" */
        SceneNodePtr findChild(const std::string & name);

        void addChild(SceneNodePtr child);
        void delChild(SceneNodePtr child);
        void delChildByName(const std::string & name);
        void clearChildren();
        void removeFromParent();

        /** �������볡�����󣬻ᱻ���á�*/
        virtual void onEnterWorld();
        /** �����ӳ������Ƴ�֮ǰ���ᱻ���á�*/
        virtual void onLeaveWorld();

        ///������Ⱦ
        virtual void render(IDirect3DDevice9 * pDevice);

        ///�߼�����
        virtual void update(float elapse);

    protected:
        void setParent(SceneNode * parent);
        void setDirtyFlag(int flag);


        std::string             m_name;
        TChildren               m_children;
        SceneNode *             m_parent;
        BOOL                    m_inWorld;
        BOOL                    m_visible;
        AABB                    m_aabb;

    private:
        Vector3                 m_position;
        Vector3                 m_scale;
        Quaternion              m_rotation;

        mutable Matrix          m_matrix;
        mutable int             m_matrixDirty;
        mutable Matrix          m_matWorld;
    };


} // end namespace Lazy
