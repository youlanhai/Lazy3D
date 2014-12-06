#pragma once

#include "Vector.h"

namespace Lazy
{
    class Triangle;
    class Matrix;

    class AABB
    {
    public:

        Vector3 min;
        Vector3 max;

        AABB();
        AABB(const Vector3 & mi, const Vector3 & mx);

        void zero();

        //正规化。aabb经过变换后，min可能要大于max。
        void normalization();

        void getCenter(Vector3 & center) const;

        void getDiameter(Vector3 & diameter) const;

        ///包围盒求交集
        void sub(const AABB & aabb);

        ///包围盒求并集
        void add(const AABB & aabb);

        ///并入点
        void addPoint(const Vector3 & pt);

        ///并入三角形
        void addTriangle(const Triangle & tri);

        bool intersectsRay(const Vector3 & origin, const Vector3 & dir) const;

        bool intersect(const AABB & aabb) const;

        bool clipLine(Vector3 & start, Vector3 & end) const;


        void applyMatrix(const Matrix & mat);
        void applyMatrix(AABB & out, const Matrix & mat) const;
    };




}//end namespace Lazy
