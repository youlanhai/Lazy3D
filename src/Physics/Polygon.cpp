#include "stdafx.h"
#include "Polygon.h"
#include "Triangle.h"
#include "Plane.h"

namespace Lazy
{
    Polygon::Polygon(const Triangle & tri)
    {
        *this = tri;
    }

    const Polygon & Polygon::operator=(const Triangle & tri)
    {
        addTriangle(tri);
        return *this;
    }

    void Polygon::genPanel(Plane & panel) const
    {
        assert(size() >= 3);

        const_iterator it = begin();
        const Vector3 &a = *it++;
        const Vector3 &b = *it++;
        const Vector3 &c = *it;
        Vector3 ab = b - a;
        Vector3 bc = c - b;
        Vector3 n = ab.cross(bc);
        n.normalize();
        panel.set(n, a);
    }

    void Polygon::addTriangle(const Triangle & tri)
    {
        addPoint(tri.a);
        addPoint(tri.b);
        addPoint(tri.c);
    }

    void Polygon::addPoint(const Vector3 & p)
    {
        if (empty() || !p.tooClose(back()))
        {
            push_back(p);
            return;
        }
    }

    void Polygon::split(const Plane & panel, Polygon & frontPoly, Polygon & backPoly) const
    {
        if (size() < 3)
        {
            return;
        }

        const Vector3*  pLastPt = &back();
        float lastDist = panel.distToPoint(*pLastPt);
        bool wasFront = lastDist > 0.0f;

        for (const_iterator it = begin(); it != end(); ++it)
        {
            float dist = panel.distToPoint(*it);
            bool isFront = dist > 0.0f;

            if (wasFront != isFront)
            {
                //插值求割点
                float ratio = fabs(lastDist / (dist - lastDist));
                Vector3 cross = (*pLastPt) * (1 - ratio) + (*it) * ratio;
                frontPoly.addPoint(cross);
                backPoly.addPoint(cross);
            }

            if (isFront)
            {
                frontPoly.addPoint(*it);
            }
            else
            {
                backPoly.addPoint(*it);
            }

            pLastPt = &(*it);
            lastDist = dist;
            wasFront = isFront;
        }

        frontPoly.adjudge();
        backPoly.adjudge();
    }

    void Polygon::splitFront(const Plane & panel, Polygon & frontPoly) const
    {
        if (size() < 3)
        {
            return;
        }

        const Vector3*  pLastPt = &back();
        float lastDist = panel.distToPoint(*pLastPt);
        bool wasFront = lastDist >= 0;

        for (const_iterator it = begin(); it != end(); ++it)
        {
            float dist = panel.distToPoint(*it);
            bool isFront = dist >= 0;

            if (wasFront != isFront)
            {
                //插值求割点
                float ratio = fabs(lastDist / (dist - lastDist));
                Vector3 cross = (*pLastPt) * (1 - ratio) + (*it) * ratio;
                frontPoly.addPoint(cross);
            }

            if (isFront)
            {
                frontPoly.addPoint(*it);
            }

            pLastPt = &(*it);
            lastDist = dist;
            wasFront = isFront;
        }
        frontPoly.adjudge();
    }

    bool Polygon::choiceFront(const Plane & panel)
    {
        Polygon frontPoly;
        splitFront(panel, frontPoly);
        swap(frontPoly);
        return !empty();
    }

    void Polygon::adjudge()
    {
        if (size() < 3)
        {
            clear();
            return;
        }
        if (front().tooClose(back()))
        {
            pop_back();
        }
        if (size() < 3)
        {
            clear();
        }
    }

    float Polygon::minDistToPanel(const Plane & panel)
    {
        float minDist = MAX_FLOAT;
        if (empty())
        {
            return minDist;
        }
        for (iterator it = begin(); it != end(); ++it)
        {
            float dist = panel.distToPoint(*it);
            if (minDist > dist)
            {
                minDist = dist;
            }
        }
        return minDist;
    }

}//end namespace Lazy
