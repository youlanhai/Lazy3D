#pragma once

namespace Lazy
{
    struct CollisionInfo
    {
        Vector3 newPos;
        Vector3 oldPos;
        LPD3DXMESH	pMesh;
        Vector3 center;
        Vector3 extends;
        TriangleSet hitTriangles;
    };

    struct RayInfo
    {
        Vector3 start;
        Vector3 dir;
        LPD3DXMESH pMesh;
        float hitDistance;
        Triangle hitTriangle;
    };

    //Collision interface
    class ICollision : public IBase
    {
    public:
        ICollision(void);
        virtual ~ICollision(void);

        virtual bool preventCollision(CollisionInfo & info) = 0;

        virtual bool pickRay(RayInfo & info) = 0;
    };

    typedef RefPtr<ICollision> CollisionPtr;

    //collision type
    namespace CollisionType
    {
        const int Opcode = 1;//使用opcode库，已经不支持此类型了。
        const int OCTree = 2;//使用八叉树
        const int Bsp    = 3;//二叉树，暂不支持。

        const int Default = OCTree;
    }


    //collision factory
    class CollisionFactory
    {
    public:
        static CollisionPtr create(int type);
    };

    //碰撞体配置
    struct CollisionConfig
    {
        CollisionConfig() {}
        CollisionConfig(float w, float h, float d, float c)
            : modelWidth(w), modelHeight(h), modelDepth(d), climbHeight(c)
        {}

        float modelWidth;
        float modelHeight;
        float modelDepth;
        float climbHeight;//可爬行高度
    };

    //TODO: 重构以下代码

    //体碰撞
    class CollisionPrevent
    {
    public:
        CollisionPrevent(const Vector3 & oldPos, const Vector3& newPos, bool vertical,
                         const CollisionConfig & config);

        void drawDebug(IDirect3DDevice9* device) const;

        //与模型碰撞检测。
        bool preventCollision(LPD3DXMESH pMesh, const Matrix & world);

    public:

        bool collision(const Triangle & tri, const Matrix & world);

        bool clip(Polygon & polyInOut);

        int witchSide(const Triangle & tri, const Matrix & world);

        int witchSide(const Plane & panel);

        void resetPoints();

        void resetFarPoints();

        void resetBounds();

    public:
        CollisionConfig m_config;
        Vector3 m_start;
        Vector3 m_end;

        float   m_distance;

        bool    m_verticalCheck;
        bool    m_isCollisiond;

        Vector3 m_look;
        Vector3 m_up;
        Vector3 m_right;

        Plane m_leftP;
        Plane m_rightP;
        Plane m_bottomP;
        Plane m_topP;
        Plane m_nearP;
        AABB  m_aabb;

        Vector3 m_point8[8];

        CollisionPtr m_collision;
        Triangle m_hitTriangle;

        std::vector<Polygon> m_debugPolygons;

    };

    //射线拾取
    class RayCollision
    {
    public:
        RayCollision() {}

        RayCollision(const Vector3 & start, const Vector3 & dir, float distance = MAX_FLOAT);

        //射线拾取
        bool pick(LPD3DXMESH pMesh, const Matrix & world);

        bool isCollied() const { return m_collied; }

        void getCollidPos(Vector3 & pos) const
        {
            pos = m_dir;
            pos *= m_hitDistance;
            pos += m_start;
        }

        void render(LPDIRECT3DDEVICE9 pDevice);

    public:
        Vector3     m_start;
        Vector3     m_dir;
        float       m_hitDistance;
        Triangle    m_hitTriangle;
        bool        m_collied;
        CollisionPtr m_collisionPtr;
    };
}//namespace Lazy
