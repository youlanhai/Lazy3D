#include "stdafx.h"
#include "Plane.h"
#include "Line.h"
#include "Triangle.h"

namespace Math
{
    Plane::Plane(const Vector3 & n, const Vector3 & p)
        : normal(n)
    {
        distance = normal.dot(p);
    }

    Plane::Plane(const Vector3 & n, float d)
        : normal(n), distance(d)
    {}

    Plane::Plane(const Triangle & tri)
        : normal(tri.normal())
    {
        distance = normal.dot(tri.a);
    }

    void Plane::set(const Vector3 & n, const Vector3 & p)
    {
        normal = n;
        distance = normal.dot(p);
    }

    float Plane::distToPoint(const Vector3 & p) const
    {
        return normal.dot(p) - distance;
    }

    int Plane::witchSide(const Vector3 & pt) const
    {
        float dt = distToPoint(pt);
        if (amostZero(dt)) return SIDE_ON;
        if (dt > 0) return SIDE_FRONT;
        return SIDE_BACK;
    }

    int Plane::witchSide(const Triangle & tri)
    {
        int side = 0;
        side |= witchSide(tri.a);
        side |= witchSide(tri.b);
        side |= witchSide(tri.c);
        return side;
    }

    bool Plane::isIntersect(const Line & line, Vector3 * cross) const
    {
        float distS = distToPoint(line.start);
        float distE = distToPoint(line.end);
        if (distS * distE > 0)
        {
            return false;
        }

        if (cross)
        {
            *cross = line.interpolation(fabs(distS), fabs(distE));
        }
        return true;
    }

    bool Plane::isIntersect(const Vector3 & start, const Vector3 & end, float * pPos) const
    {
        float distS = distToPoint(start);
        float distE = distToPoint(end);
        if (distS * distE > 0)
        {
            return false;
        }

        if (pPos)
        {
            *pPos = fabs(distS) / (fabs(distS) + fabs(distE));
        }
        return true;
    }

    bool Plane::isSphereIntersect(const Vector3 & center, float radius, Vector3 * cross/*=NULL*/)
    {
        float ds = distToPoint(center);
        if (fabs(ds) > radius)
        {
            return false;
        }
        if (cross)
        {
            *cross = center + normal * ds;
        }
        return true;
    }

}//end namespace Math