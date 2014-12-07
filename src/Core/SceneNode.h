#pragma once

#include "Base.h"
#include "RenderInterface.h"
#include "../utility/VisitPool.h"

#define SCENE_NODE_HEADER(CLASS)        \
    typedef This    Base;               \
    typedef CLASS   This;               \
    static CLASS * createInstance(){ return new CLASS(); } \

namespace Lazy
{
    class SceneNode;
    typedef RefPtr<SceneNode> SceneNodePtr;


    namespace SceneNodeType
    {
        const int Empty = 0;
        const int Model = 1;
        const int Entity = 2;

        const int UserType = 0xff;
    }


    /** 场景结点。
     *  一个结点可以拥有若干子结点，从而构成一个场景树。
     *  挂在场景树上的结点，每帧都可以被更新（update）和渲染（render）。
     */
    class SceneNode : public IBase, public IRenderable
    {
    public:
        typedef SceneNode This;
        static SceneNode * createInstance(){ return new SceneNode(); }

        typedef VisitPool<SceneNodePtr> TChildren;
        enum DirtyFlag
        {
            DirtyLocal = 1 << 0,
            DirtyWorld = 1 << 1,

            DirtyAll = DirtyLocal | DirtyWorld,
        };

        SceneNode();
        ~SceneNode();

        virtual int getType() const { return SceneNodeType::Empty; }

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

        void                setBoundingBox(const AABB & aabb){ m_aabb = aabb; }
        const AABB &        getBoundingBox() const { return m_aabb; }
        AABB                getWorldBoundingBox() const;

        SceneNode *         getParent() { return m_parent; }
        const SceneNode *   getParent() const { return m_parent; }

        const std::string & getName() const { return m_name; }
        void                setName(const std::string & name){ m_name = name; }

        BOOL inWorld() const { return m_inWorld; }

        /** 根据名称查找子结点。name可以为路径形式，如 "parent/child1" */
        SceneNodePtr findChild(const std::string & name);

        void addChild(SceneNodePtr child);
        void delChild(SceneNodePtr child);
        void delChildByName(const std::string & name);
        void clearChildren();
        void removeFromParent();

        TChildren &                 getChildren() { return m_children; }
        const TChildren &           getChildren() const { return m_children; }
        size_t                      getNbChildren() const { return m_children.size(); }

        TChildren::iterator         begin() { return m_children.begin(); }
        TChildren::const_iterator   begin() const { return m_children.begin(); }
        TChildren::iterator         end() { return m_children.end(); }
        TChildren::const_iterator   end() const { return m_children.end(); }

        /** 当结点加入场景树后，会被调用。*/
        virtual void onEnterWorld();
        /** 当结点从场景树移除之前，会被调用。*/
        virtual void onLeaveWorld();

        ///画面渲染
        virtual void render(IDirect3DDevice9 * pDevice);

        ///逻辑更新
        virtual void update(float elapse);

        virtual void saveToStream(LZDataPtr data);
        virtual bool loadFromStream(LZDataPtr data);

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
