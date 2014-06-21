#pragma once

namespace Lazy
{
    class Vector2;
    class AABB;
    class Triangle;

    ///矩形区域
    class FRect
    {
    public:
        float left, top, right, bottom;

        FRect() : left(0.0f), top(0.0f), right(0.0f), bottom(0.0f)
        {
        }

        FRect(float l, float t, float r, float b)
            : left(l), top(t), right(r), bottom(b)
        {
        }

        float width() const { return right - left; }
        float height() const { return bottom - top; }

        void zero();

        bool isIn(float x_, float y_) const;

        void expand(const FRect & rc);

        void expand(float x, float y);

        void delta(float dx, float dy);

        ///将aabb的xz平面转换成区域
        void fromAABBXZ(const AABB & aabb);

        ///正规化。aabb经过变换后，min可能要大于max。
        void normalization();

        ///两个区域是否相交
        bool isIntersect(const FRect & rc) const;

        ///矩形与射线相交
        bool isIntersect(const Vector2 & start, const Vector2 & dir) const;

        ///三角形xz所在区域，是否与当前区域相交
        bool isTriangleXZIn(const Triangle & tri) const;


        float & operator [](size_t i)
        {
            assert(i >= 0 && i < 4 && "FRect: index must in [0-3]!");
            return ((float*) this)[i];
        }

        const float & operator [](size_t i) const
        {
            assert(i >= 0 && i < 4 && "FRect: index must in [0-3]!");
            return ((float*) this)[i];
        }

    };



}//end namespace Lazy
