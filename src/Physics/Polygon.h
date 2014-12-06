#pragma once

#include "Vector.h"

namespace Lazy
{
    class Triangle;
    class Plane;

    //////////////////////////////////////////////////////////////////////////
    //多边形
    //////////////////////////////////////////////////////////////////////////
    typedef std::vector<Vector3> PolygonBase;

    class Polygon : public PolygonBase
    {
    public:

        Polygon() : PolygonBase() {}

        Polygon(int size) : PolygonBase(size) {}

        Polygon(const Polygon& p) : PolygonBase(p) {}

        Polygon(const Triangle & tri);

        const Polygon & operator=(const Triangle & tri);

        void genPanel(Plane & panel) const;

        void addTriangle(const Triangle & tri);

        void split(const Plane & panel, Polygon & frontPoly, Polygon & backPoly) const;

        void splitFront(const Plane & panel, Polygon & frontPoly) const;

        bool choiceFront(const Plane & panel);

        void addPoint(const Vector3 & p);

        void adjudge();

        float minDistToPanel(const Plane & panel);
    };

}//end namespace Lazy
