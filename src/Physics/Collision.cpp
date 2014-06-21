
#include "stdafx.h"
#include "Collision.h"
#include "LazyConllision.h"
#include "PhysicsDebug.h"

namespace Lazy
{

    ICollision::ICollision(void)
    {
    }

    ICollision::~ICollision(void)
    {
    }



    /*static */CollisionPtr CollisionFactory::create(int type)
    {
        if (type == CollisionType::OCTree)
        {
            return new LazyCollision();
        }

        throw(std::invalid_argument("collision type doesn't supported!"));
    }

    //体碰撞实现
    CollisionPrevent::CollisionPrevent(
        const Vector3 & oldPos,
        const Vector3& newPos,
        bool vertical,
        const CollisionConfig & config)
        : m_start(oldPos)
        , m_end(newPos)
        , m_config(config)
        , m_isCollisiond(false)
        , m_verticalCheck(vertical)
    {
        if (!m_verticalCheck)
        {
            m_up.set(0, 1.0f, 0);

            //y轴增加一个爬行高度，表示可以爬升。
            m_start.y += m_config.climbHeight;
            m_end.y = m_start.y;

            m_look =  m_end - m_start;
            m_distance = m_look.length();
            m_look.normalize();

            m_right = m_up.cross(m_look);
            m_right.normalize();

        }
        else
        {
            m_look.set(0, m_end.y - m_start.y, 0);
            m_distance = m_look.length();
            m_look.normalize();

            m_right.set(-1, 0, 0);
            m_up = m_look.cross(m_right);
        }

        m_distance += m_config.modelDepth;
        m_end += m_look * m_config.modelDepth;

        resetPoints();
        resetBounds();

        m_collision = CollisionFactory::create(CollisionType::Default);
    }

    void CollisionPrevent::resetPoints()
    {

        Vector3 vHalfWidth = m_right * (m_config.modelWidth * 0.5f);
        Vector3 vHeight = m_up * m_config.modelHeight;

        if (!m_verticalCheck)
        {
            m_point8[0] = m_start - vHalfWidth; //近-左下
            m_point8[1] = m_point8[0] + vHeight; //近-左上

            m_point8[3] = m_start + vHalfWidth; //近-右下
            m_point8[2] = m_point8[3] + vHeight;//近-右上

        }
        else
        {
            //居中
            Vector3 leftUp = -vHalfWidth + vHeight * 0.5f;
            Vector3 rightUp = vHalfWidth + vHeight * 0.5f;

            m_point8[0] = m_start - rightUp; //近-左下
            m_point8[1] = m_start + leftUp; //近-左上

            m_point8[3] = m_start - leftUp; //近-右下
            m_point8[2] = m_start + rightUp;//近-右上
        }

        m_aabb.zero();
        for(int i = 0; i < 8; ++i)
        {
            m_aabb.addPoint(m_point8[i]);
        }

        resetFarPoints();
    }

    void CollisionPrevent::resetFarPoints()
    {
        Vector3 vDeep = m_look * m_distance;
        for (int i = 0; i < 4; ++i)
        {
            m_point8[i + 4] = m_point8[i] + vDeep;
        }
    }

    void CollisionPrevent::resetBounds()
    {
        m_nearP.set(m_look, m_point8[0]);

        m_leftP.set(m_right, m_point8[0]);
        m_rightP.set(-m_right, m_point8[2]);

        m_bottomP.set(m_up, m_point8[0]);
        m_topP.set(-m_up, m_point8[2]);
    }

    bool CollisionPrevent::clip(Polygon & polyInOut)
    {
        if(!polyInOut.choiceFront(m_leftP))
        {
            return false;
        }
        if(!polyInOut.choiceFront(m_rightP))
        {
            return false;
        }
        if(!polyInOut.choiceFront(m_topP))
        {
            return false;
        }
        if(!polyInOut.choiceFront(m_bottomP))
        {
            return false;
        }
        if(!polyInOut.choiceFront(m_nearP))
        {
            return false;
        }

        return true;
    }

    int CollisionPrevent::witchSide(const Triangle & tri, const Matrix & world)
    {
        Triangle triangle(tri);
        triangle.applyMatrix(world);
        Plane panel(triangle);
        return witchSide(panel);
    }

    int CollisionPrevent::witchSide(const Plane & panel)
    {
        int side = 0;
        for (int i = 0; i < 8; ++i)
        {
            float dist = panel.distToPoint(m_point8[i]);
            if(dist > 0.0f)
            {
                side |= SIDE_FRONT;
            }
            else if(dist < 0.0f)
            {
                side |= SIDE_BACK;
            }
        }
        return side;
    }

    bool CollisionPrevent::collision(const Triangle & tri, const Matrix & world)
    {
        Triangle tempTri = tri;
        tempTri.a.applyMatrix(world);
        tempTri.b.applyMatrix(world);
        tempTri.c.applyMatrix(world);


        float minDist = min(m_nearP.distToPoint(tempTri.a),  m_nearP.distToPoint(tempTri.b));
        minDist = min(minDist,  m_nearP.distToPoint(tempTri.c));
        if (minDist > m_distance)
        {
            return false;
        }

        Polygon poly(tempTri);
        if(!clip(poly))
        {
            return false;
        }

        float dist = poly.minDistToPanel(m_nearP);
        if (m_distance > dist)
        {
            m_isCollisiond = true;
            m_distance = dist;
            m_hitTriangle = tempTri;

            resetFarPoints();
            m_debugPolygons.push_back(poly);
            return true;
        }
        return false;
    }

    void CollisionPrevent::drawDebug(IDirect3DDevice9* pDevice) const
    {
        pDevice->SetTransform(D3DTS_WORLD, &matIdentity);

        //near
        Polygon poly(4);
        for (int i = 0; i < 4; ++i)
        {
            poly[i] = m_point8[i];
        }
        drawPolygon(pDevice, poly, 0x7f00ffff);

        //far
        for (int i = 0; i < 4; ++i)
        {
            poly[i] = m_point8[i + 4];
        }
        drawPolygon(pDevice, poly, 0x7f00ffff);

        //left
        poly[0] = m_point8[0];
        poly[1] = m_point8[1];
        poly[2] = m_point8[5];
        poly[3] = m_point8[4];
        drawPolygon(pDevice, poly, 0x7fffff00);

        //right
        poly[0] = m_point8[3];
        poly[1] = m_point8[2];
        poly[2] = m_point8[6];
        poly[3] = m_point8[7];
        drawPolygon(pDevice, poly, 0x7f00ff00);


        //bottom
        poly[0] = m_point8[0];
        poly[1] = m_point8[3];
        poly[2] = m_point8[7];
        poly[3] = m_point8[4];
        drawPolygon(pDevice, poly, 0x7f0000ff);

        //top
        poly[0] = m_point8[1];
        poly[1] = m_point8[2];
        poly[2] = m_point8[6];
        poly[3] = m_point8[5];
        drawPolygon(pDevice, poly, 0x7fff00ff);

#ifdef _DEBUG
        for (size_t i = 0; i < m_debugPolygons.size(); ++i)
        {
            drawPolygon(pDevice, m_debugPolygons[i], 0xffff0000);
        }
        drawDebugTriangle(pDevice);
#endif

    }

    bool CollisionPrevent::preventCollision(LPD3DXMESH pMesh, const Matrix & world)
    {
        CollisionInfo info;
        info.pMesh = pMesh;

        //计算mesh的逆矩阵。用于将Player的包围盒转换到mesh空间。
        Matrix inverseMat;
        world.getInvert(inverseMat);

        //计算world矩阵的缩放系数
        float scale = getMatrixScale(world);

        info.newPos = m_end;
        info.oldPos = m_start;
        if (!m_verticalCheck)
        {
            Vector3 vHalfHeight = m_up * m_config.modelHeight * 0.5f;
            info.newPos = m_end + vHalfHeight;
            info.oldPos = m_start + vHalfHeight;
        }

        info.center = (m_start + m_end) * 0.5f;

        //将坐标转换到模型空间。
        info.newPos.applyMatrix(inverseMat);
        info.oldPos.applyMatrix(inverseMat);
        info.center.applyMatrix(inverseMat);

        //对包围盒的尺寸进行缩放
        float halfWidth = m_config.modelWidth * 0.5f * scale;
        float halfHeight = m_config.modelHeight * 0.5f * scale;
        float halfDist = m_start.distTo(m_end) + m_config.modelDepth;
        halfDist *= 0.5f * scale;

        info.extends = D3DXVECTOR3(halfWidth, halfHeight, halfDist);

        if(m_collision->preventCollision(info))
        {
            for (TriangleSet::const_iterator it = info.hitTriangles.begin();
                    it != info.hitTriangles.end(); ++it)
            {
                collision(*it, world);
            }
            return true;
        }
        return false;
    }

    //////////////////////////////////////////////////////////////////////////

    RayCollision::RayCollision(const Vector3 & start, const Vector3 & dir, float distance/*=MAX_FLOAT*/)
        : m_start(start), m_dir(dir), m_hitDistance(distance), m_collied(false)
    {
        m_collisionPtr = CollisionFactory::create(CollisionType::OCTree);
    }

    bool RayCollision::pick(LPD3DXMESH pMesh, const Matrix & world)
    {
        Matrix invWorld;
        world.getInvert(invWorld);

        RayInfo info;
        info.pMesh = pMesh;
        info.start = m_start;
        info.dir = m_dir;

        info.start.applyMatrix(invWorld);
        info.dir.applyNormalMatrix(invWorld);

        if(m_collisionPtr->pickRay(info))
        {
            Vector3 end = info.start + info.dir * info.hitDistance;
            info.start.applyMatrix(world);
            end.applyMatrix(world);
            float distance = info.start.distTo(end);

            if (m_hitDistance > distance)
            {
                m_collied = true;
                m_hitDistance = distance;
                m_hitTriangle = info.hitTriangle;
                m_hitTriangle.applyMatrix(world);
                return true;
            }
        }

        return false;
    }

    void RayCollision::render(LPDIRECT3DDEVICE9 pDevice)
    {
        if (!m_collied)
        {
            //return;
        }

        pDevice->SetTransform(D3DTS_WORLD, &matIdentity);

        Vector3 end = m_start + m_dir * m_hitDistance;
        drawLine(pDevice, m_start, end, 0xff00ff00);
        drawTriangle(pDevice, m_hitTriangle, 0xffff0000);
    }
}//namespace Lazy
