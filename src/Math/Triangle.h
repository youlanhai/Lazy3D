#pragma once

#include "Vector.h"

namespace Math
{
    class Line;

    ///三角形
    class Triangle
    {
    public:
        Triangle(){}
        Triangle(const Vector3 & A, const Vector3 & B, const Vector3 & C)
            : a(A), b(B), c(C)
        {}

        Vector3 normal() const
        {
            Vector3 ab = b - a;
            Vector3 bc = c - b;
            Vector3 n = ab.cross(bc);
            n.normalize();
            return n;
        }

        Vector3 & operator [](int i)
        {
            if (i == 0) return a;
            if (i == 1) return b;
            if (i == 2) return c;
            throw std::out_of_range("Triangle index out of range");
        }
        const Vector3 & operator [](int i) const
        {
            if (i == 0) return a;
            if (i == 1) return b;
            if (i == 2) return c;
            throw std::out_of_range("Triangle index out of range");
        }

        bool isPointIn(const Vector3 & p) const;

        bool isLineIntersect(const Line & line, Vector3 * cross = NULL) const;

        bool isSphereIntersect(const Vector3 & center, float radius, Vector3 * cross = NULL) const;

        void applyMatrix(const Matrix4x4 & mat)
        {
            a.applyMatrix(mat);
            b.applyMatrix(mat);
            c.applyMatrix(mat);
        }

        bool intersect(const Triangle & triangle, const Vector3 & offset) const;

        bool intersectRay(const Vector3 & start, const Vector3 & dir, float *pDist = NULL) const;

    public:
        Vector3 a, b, c;
    };


    typedef std::vector<int> TriangleIndices;
    typedef std::vector<Triangle> TriangleSet;


}//namespace Math
