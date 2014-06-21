#pragma once

#include "Vector.h"

namespace Lazy
{
    class Line;
    class Triangle;

    ///平面
    class Plane
    {
    public:
        Plane() : distance(0.0f)  {}

        Plane(const Vector3 & n, const Vector3 & p);

        Plane(const Vector3 & n, float d);

        Plane(const Triangle & tri);

        void set(const Vector3 & n, const Vector3 & p);

        float distToPoint(const Vector3 & p) const;

        bool isIntersect(const Line & line, Vector3 * cross = NULL) const;

        bool isIntersect(const Vector3 & start, const Vector3 & end, float * pPos = NULL) const;

        bool isSphereIntersect(const Vector3 & center, float radius, Vector3 * cross = NULL);

        int witchSide(const Vector3 & pt) const;

        int witchSide(const Triangle & tri);

        bool operator==(const Plane & p)
        {
            return normal == p.normal && distance == p.distance;
        }

    public:
        Vector3 normal;
        float distance;
    };

}//end namespace Lazy

