#include "stdafx.h"
#include "Rect.h"
#include "Vector.h"
#include "Triangle.h"
#include "AABB.h"


namespace Lazy
{
    void FRect::set(float l, float t, float r, float b)
    {
        left = l;
        top = t;
        right = r;
        bottom = b;
    }

    void FRect::zero()
    {
        left = top = MAX_FLOAT;
        right = bottom = MIN_FLOAT;
    }

    bool FRect::isIn(float x_, float y_) const
    {
        if (x_ < left || x_ > right) return false;
        if (y_ < top || y_ > bottom) return false;
        return true;
    }

    void FRect::expand(const FRect & rc)
    {
        left = min2(left, rc.left);
        top = min2(top, rc.top);
        right = max2(right, rc.right);
        bottom = max2(bottom, rc.bottom);
    }

    void FRect::expand(float x, float y)
    {
        left = min2(left, x);
        top = min2(top, y);
        right = max2(right, x);
        bottom = max2(bottom, y);
    }

    void FRect::delta(float dx, float dy)
    {
        left -= dx;
        right += dx;
        top -= dy;
        bottom += dy;
    }

    ///将aabb的xz平面转换成区域
    void FRect::fromAABBXZ(const AABB & aabb)
    {
        left = aabb.min.x;
        top = aabb.min.z;
        right = aabb.max.x;
        bottom = aabb.max.z;
    }

    bool FRect::isIntersect(const FRect & rc) const
    {
        if (left > rc.right) return false;
        if (right < rc.left) return false;
        if (top > rc.bottom) return false;
        if (bottom < rc.top) return false;
        return true;
    }


    bool FRect::isTriangleXZIn(const Triangle & tri) const
    {
        float minx = min2(tri.a.x, min2(tri.b.x, tri.c.x));
        if (minx > right) return false;

        float maxx = max2(tri.a.x, max2(tri.b.x, tri.c.x));
        if (maxx < left) return false;

        float minz = min2(tri.a.z, min2(tri.b.z, tri.c.z));
        if (minz > bottom) return false;

        float maxz = max2(tri.a.z, max2(tri.b.z, tri.c.z));
        if (maxz < top) return false;

        return true;
    }

    ///分离定理求矩形与射线相交
    bool FRect::isIntersect(const Vector2 & start, const Vector2 & dir) const
    {
#if 1
        float ty = dir.x * (top - start.y);
        float lx = dir.y * (left - start.x);
        float rx = dir.y * (right - start.x);
        float by = dir.x * (bottom - start.y);

        //dir x leftTop = ty - lx
        float delta = ty - lx;

        //dir x rightTop = ty - rx
        if (delta * (ty - rx) < 0.0f) return true;

        //dir x leftBottom = by - lx
        if (delta * (by - lx) < 0.0f) return true;

        //dir x rightBottom = by - rx
        if (delta * (by - rx) < 0.0f) return true;
#else

        Vector2 vec(left, top);
        vec -= start;

        float delta = dir.cross(vec);

        vec.set(right, top);
        vec -= start;
        float temp = dir.cross(vec);
        if (delta * temp < 0.0f) return true;

        vec.set(right, bottom);
        vec -= start;
        temp = dir.cross(vec);
        if (delta * temp < 0.0f) return true;

        vec.set(left, bottom);
        vec -= start;
        temp = dir.cross(vec);
        if (delta * temp < 0.0f) return true;

#endif
        return false;
    }

    ///正规化。aabb经过变换后，min可能要大于max。
    void FRect::normalization()
    {
        if (left > right) std::swap(left, right);
        if (top > bottom) std::swap(top, bottom);
    }



}//end namespace Lazy

