#include "stdafx.h"
#include "Triangle.h"
#include "Line.h"
#include "Plane.h"

namespace Lazy
{

    inline bool inside(const Vector2 & a, const Vector2 & b)
    {
        return (a.x * b.y - a.y * b.x) >= 0;
    }

    //分割定理求两个2d三角形相交
    bool triangle2DIntersect(Vector2 triA[3], Vector2 triB[3])
    {
        bool signA = inside(triA[1] - triA[0], triA[2] - triA[0]);
        bool signB = inside(triB[1] - triB[0], triB[2] - triB[0]);
        Vector2 segmentA, segmentB;

        for (int i1 = 0, i2 = 2; i1 <= 2; i2 = i1, i1++)
        {
            segmentA = triA[i1] - triA[i2];
            //线段A可以将两个三角形分割开,则不相交
            if (inside(segmentA, triB[0] - triA[i2]) != signA &&
                    inside(segmentA, triB[1] - triA[i2]) != signA &&
                    inside(segmentA, triB[2] - triA[i2]) != signA)
            {
                return false;
            }

            segmentB = triB[i1] - triB[i2];
            if (inside(segmentB, triA[0] - triB[i2]) != signB &&
                    inside(segmentB, triA[1] - triB[i2]) != signB &&
                    inside(segmentB, triA[2] - triB[i2]) != signB)
            {
                return false;
            }
        }
        return true;
    }

    // Determine whether point P in triangle ABC
    bool pointinTriangle(const Vector3 & A, const Vector3 & B, const Vector3 & C, const Vector3 & P)
    {
        Vector3 v0 = C - A;
        Vector3 v1 = B - A;
        Vector3 v2 = P - A;

        float dot00 = v0.dot(v0);
        float dot01 = v0.dot(v1);
        float dot02 = v0.dot(v2);
        float dot11 = v1.dot(v1);
        float dot12 = v1.dot(v2);

        float inverDeno = 1 / (dot00 * dot11 - dot01 * dot01);

        float u = (dot11 * dot02 - dot01 * dot12) * inverDeno;
        if (u < 0 || u > 1) // if u out of range, return directly
        {
            return false;
        }

        float v = (dot00 * dot12 - dot01 * dot02) * inverDeno;
        if (v < 0 || v > 1) // if v out of range, return directly
        {
            return false;
        }

        return u + v <= 1;
    }

    bool Triangle::isPointIn(const Vector3 & p) const
    {
        return pointinTriangle(a, b, c, p);
    }

    bool Triangle::isLineIntersect(const Line & line, Vector3 * cross) const
    {
        Plane panel(*this);
        Vector3 point;
        if (!panel.isIntersect(line, &point))
        {
            return false;
        }
        if (isPointIn(point))
        {
            *cross = point;
            return true;
        }
        return false;
    }

    bool Triangle::isSphereIntersect(const Vector3 & center, float radius, Vector3 * cross/*=NULL*/) const
    {
        Plane panel(*this);
        Vector3 pos;
        if (!panel.isSphereIntersect(center, radius, &pos))
        {
            return false;
        }
        if (!isPointIn(pos))
        {
            return false;
        }
        if (cross)
        {
            *cross = pos;
        }
        return true;
    }

    //与三角形划过体碰撞
    bool Triangle::intersect(const Triangle & triangle,
                             const Vector3 & offset) const
    {
        //math3d::addDebugDrawTriangle(*this, 0x3fffffff);

        Plane panel(*this);
        //求位移在平面法线上的投影总长度
        float dt = panel.normal * offset;
        if (amostZero(dt))
        {
            return false;
        }

        //求裁剪比例
        float indt = 1.0f / dt;
        float at = panel.distToPoint(triangle.a) * indt;
        float bt = panel.distToPoint(triangle.b) * indt;
        float ct = panel.distToPoint(triangle.c) * indt;

        //不在范围
        if (at < 0.0f && bt < 0.0f && ct < 0.0f)
        {
            return false;
        }
        if (at > 1.0f && bt > 1.0f && ct > 1.0f)
        {
            return false;
        }

        //裁剪三角体得到的三角形
        Triangle clipT(
            triangle.a + offset * at,
            triangle.b + offset * bt,
            triangle.c + offset * ct);

        //接下来判断当前三角形与裁剪得到的三角形是否相交。
        //将三角形投影到2维空间，判断两个2维三角形是否相交。

        int indexU, indexV;
        panel.normal.projectTo2D(indexU, indexV);

        //当前三角形的投影
        Vector2 triA[3] =
        {
            Vector2(a[indexU], a[indexV]),
            Vector2(b[indexU], b[indexV]),
            Vector2(c[indexU], c[indexV]),
        };

        //裁剪三角形的投影
        Vector2 triB[3] =
        {
            Vector2(clipT.a[indexU], clipT.a[indexV]),
            Vector2(clipT.b[indexU], clipT.b[indexV]),
            Vector2(clipT.c[indexU], clipT.c[indexV]),
        };

        //math3d::addDebugDrawTriangle(*this, 0x3fffffff);

        if (!triangle2DIntersect(triA, triB))
        {
            return false;
        }

        return true;
    }

    bool Triangle::intersectRay(const Vector3 & start, const Vector3 & dir, float *pDist/*=NULL*/) const
    {
        Plane plane(*this);
        float delta = plane.normal.dot(dir);//夹角的余弦值
        float dist = plane.distToPoint(start);

        //如果起点在正面，方向与法线夹角小于90度时，不相交。
        //如果在反面，同理。
        if (dist * delta > 0)
        {
            return false;
        }

        float length = fabs(dist / delta);
        Vector3 end = start + dir * length;//计算终点

        //判断终点是否落在了三角形中。

        if (isPointIn(end))
        {
            if (pDist) *pDist = length;
            return true;
        }

        return false;
    }

}//end namespace Lazy
