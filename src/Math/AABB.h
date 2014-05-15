#pragma once

#include "Vector.h"

namespace Math
{
    class Triangle;
    class Matrix4x4;

    class AABB
    {
    public:

        Vector3 min;
        Vector3 max;

        void zero()
        {
            min = MathConst::vec3Max;
            max = MathConst::vec3Min;
        }

        //正规化。aabb经过变换后，min可能要大于max。
        void normalization();

        void getCenter(Vector3 & center) const
        {
            center = (max + min) * 0.5f;
        }

        void getDiameter(Vector3 & diameter) const
        {
            diameter = max - min;
        }

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


        void applyMatrix(const Matrix4x4 & mat);
        void applyMatrix(AABB & out, const Matrix4x4 & mat) const;
    };




}//end namespace Math
