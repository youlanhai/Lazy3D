//////////////////////////////////////////////////////////////////////////
//bsp
//////////////////////////////////////////////////////////////////////////

#pragma

#include "Collision.h"

namespace Lazy
{
    typedef RefPtr<class BspNode> BspNodePtr;
    typedef RefPtr<class BspTree> BspTreePtr;

    //////////////////////////////////////////////////////////////////////////
    //bsp 结点
    //////////////////////////////////////////////////////////////////////////
    class BspNode : public IBase
    {
        MAKE_UNCOPY(BspNode)

    public:

        BspNode() { }

        explicit BspNode(const Triangle & tri);

        virtual void addTriangle(const Triangle & tri, const Polygon & splitPoly);

        void load(Lazy::DataStream & stream);
        void save(Lazy::DataStream & stream) const;

        bool collision(
            const Triangle & triangle,
            const Vector3 & offset,
            CollisionPrevent & ci,
            const Matrix & world) const;

        bool collisionThis(
            const Triangle & triangle,
            const Vector3 & offset,
            CollisionPrevent & ci,
            const Matrix & world) const;

        bool collision(CollisionPrevent & ci, const Matrix & world) const;

        bool collisionThis(CollisionPrevent & ci, const Matrix & world) const;

        void drawDebug(IDirect3DDevice9 *device) const;

    protected:

        virtual void addToChild(BspNodePtr & child,
                                const Triangle & tri,
                                const Polygon & poly);

    protected:
        Plane           m_panel;    //分割平面
        TriangleSet     m_triangleSet; //当前节点的三角形集合
        BspNodePtr      m_front;     //左子树
        BspNodePtr      m_back;    //右子树
    };

    //////////////////////////////////////////////////////////////////////////
    //BSP树
    //////////////////////////////////////////////////////////////////////////
    class BspTree : public IBase
    {
        MAKE_UNCOPY(BspTree)

    public:
        BspTree() {}
        ~BspTree() {}

        bool generateBsp(ID3DXMesh* mesh);

        bool load(const Lazy::tstring & fname);

        bool save(const Lazy::tstring & fname) const;

        bool collision(CollisionPrevent & ci, const Matrix & world) const;

        BspNodePtr root() const { return m_root; }

        void drawDebug(IDirect3DDevice9 *pDevice, const Matrix * pWorld = NULL) const;

    protected:

        void addTriangle(const Triangle & tri);

        virtual BspNodePtr createBspNode(const Triangle & tri);

    protected:
        BspNodePtr m_root;
    };

    //////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////

    //设置bsp文件的存贮路径
    void setBspFilePath(const Lazy::tstring & path);

    const Lazy::tstring & getBspFilePath();

    //根据模型文件名，加载其对应的bsp数据。
    BspTreePtr getModelBspTree(const Lazy::tstring & modelName);

    //从文件中加载bsp数据。
    BspTreePtr loadBspTree(const Lazy::tstring & bspName);

    //////////////////////////////////////////////////////////////////////////
    //平衡二叉树。在构造bsp时使用，以优化二叉树结构。
    //////////////////////////////////////////////////////////////////////////

    class AVLBspNode;
    typedef RefPtr<AVLBspNode> AVLBspNodePtr;

    class AVLBspTree : public BspTree
    {
    public:
        virtual BspNodePtr createBspNode(const Triangle & tri);

        void setRoot(AVLBspNodePtr ptr);

        void testSave(const std::wstring & fname);
    };

    class AVLBspNode : public BspNode
    {
    public:
        AVLBspNode(AVLBspNode * parent)
            : m_parent(parent)
            , m_balence(0)
            , m_tree(NULL)
        {
        }

        AVLBspNode(AVLBspTree *tree, AVLBspNode * parent, const Triangle & tri)
            : BspNode(tri)
            , m_tree(tree)
            , m_parent(parent)
            , m_balence(0)
        {
        }

        bool isRoot() { return m_parent == NULL; }
        bool isLeft() { return m_parent->left() == this; }
        bool isRight() { return m_parent->right() == this; }

        AVLBspNodePtr left()
        {
            //assert(m_front && "no left");
            if (m_front)
            {
                return (AVLBspNodePtr)m_front;
            }
            return NULL;
        }
        AVLBspNodePtr right()
        {
            //assert(m_back && "no right");
            if(m_back)
            {
                return (AVLBspNodePtr)m_back;
            }
            return NULL;
        }

        AVLBspNode* parent() { return m_parent; }
        void setParent(AVLBspNode* ptr) { m_parent = ptr; }

        void setLeft(AVLBspNodePtr ptr)
        {
            m_front = ptr;
            if (ptr)
            {
                ptr->setParent(this);
            }
        }
        void setRight(AVLBspNodePtr ptr)
        {
            m_back = ptr;
            if (ptr)
            {
                ptr->setParent(this);
            }
        }

        void updateBalence();

        void testSave(Lazy::LZDataPtr xmlRoot);

    protected:

        virtual void addToChild(BspNodePtr & child,
                                const Triangle & tri,
                                const Polygon & poly);

    public:
        int         m_balence;
        AVLBspNode  *m_parent;
        AVLBspTree  *m_tree;
    };

}//end namespace Lazy