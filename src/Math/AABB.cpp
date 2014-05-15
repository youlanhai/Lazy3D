#include "stdafx.h"
#include "AABB.h"
#include "Triangle.h"

namespace Math
{

    bool AABB::intersectsRay(const Vector3 & origin, const Vector3 & dir) const
    {
        Vector3 ptOnPlane;
        Vector3 base(0.0f, 0.0f, 0.0f);

        int couter = 0;

        if (origin[0] <= min[0])
            base[0] = min[0];
        else if (origin[0] >= max[0])
            base[0] = max[0];
        else
        {
            couter++;
            float offset_min_x = origin[0] - min[0];
            float offset_max_x = max[0] - origin[0];

            if (offset_min_x < offset_max_x)
                base[0] = min[0];
            else
                base[0] = max[0];
        }

        if (origin[1] <= min[1])
            base[1] = min[1];
        else if (origin[1] >= max[1])
            base[1] = max[1];
        else
        {
            couter++;
            float offset_min_y = origin[1] - min[1];
            float offset_max_y = max[1] - origin[1];

            if (offset_min_y < offset_max_y)
                base[1] = min[1];
            else
                base[1] = max[1];
        }

        if (origin[2] <= min[2])
            base[2] = min[2];
        else if (origin[2] >= max[2])
            base[2] = max[2];
        else
        {
            couter++;
            float offset_min_z = origin[2] - min[2];
            float offset_max_z = max[2] - origin[2];

            if (offset_min_z < offset_max_z)
                base[2] = min[2];
            else
                base[2] = max[2];
        }

        if (couter == 3)
            return true;

        for (int axis = 0; axis <= 2; axis++)
        {
            if (dir[axis] != 0.f)
            {
                float t = (base[axis] - origin[axis]) / dir[axis];

                if (t > 0.f)
                {
                    ptOnPlane = origin + t * dir;

                    int nextAxis = (axis + 1) % 3;
                    int prevAxis = (axis + 2) % 3;

                    if (min[nextAxis] < ptOnPlane[nextAxis] && ptOnPlane[nextAxis] < max[nextAxis] &&
                        min[prevAxis] < ptOnPlane[prevAxis] && ptOnPlane[prevAxis] < max[prevAxis])
                    {
                        return true;
                    }
                }
            }
        }


        return false;
    }

    bool AABB::clipLine(Vector3 & start, Vector3 & end) const
    {
        float sMin = 0.0f;
        float eMax = 1.0f;

        Vector3 delta = end - start;

        for (int i = 0; i < 3; ++i)
        {
            if (delta[i] != 0)
            {
                float dMin = (min[i] - start[i]) / delta[i];
                float dMax = (max[i] - start[i]) / delta[i];

                if (delta[i] > 0)//射线方向从min指向max
                {
                    sMin = max2(sMin, dMin);
                    eMax = min2(eMax, dMax);
                }
                else//射线方向从max指向min，dMax应该和dMin交换
                {
                    sMin = max2(sMin, dMax);
                    eMax = min2(eMax, dMin);
                }
            }
            else if (start[i] < min[i] || start[i] > max[i]) //起点不在包围盒范围。
                return false;
        }

        end = start + eMax * delta;
        start = start + sMin * delta;

        return true;
    }

    bool AABB::intersect(const AABB & aabb) const
    {
        for (int i = 0; i < 3; ++i)
        {
            if (max[i] < aabb.min[i] || min[i] > aabb.max[i])
            {
                return false;
            }
        }
        return true;
    }

    void AABB::normalization()
    {
        for (int i = 0; i < 3; ++i)
        {
            if (min[i] > max[i]) std::swap(min[i], max[i]);
        }
    }

    void AABB::applyMatrix(const Matrix4x4 & mat)
    {
        min.applyMatrix(mat);
        max.applyMatrix(mat);
        normalization();
    }

    void AABB::applyMatrix(AABB & out, const Matrix4x4 & mat) const
    {
        min.applyMatrix(out.min, mat);
        max.applyMatrix(out.max, mat);
        out.normalization();
    }

    void AABB::add(const AABB & aabb)
    {
        min.x = min2(min.x, aabb.min.x);
        min.y = min2(min.y, aabb.min.y);
        min.z = min2(min.z, aabb.min.z);

        max.x = max2(max.x, aabb.max.x);
        max.y = max2(max.y, aabb.max.y);
        max.z = max2(max.z, aabb.max.z);
    }

    void AABB::addPoint(const Vector3 & pt)
    {
        min.x = min2(min.x, pt.x);
        min.y = min2(min.y, pt.y);
        min.z = min2(min.z, pt.z);

        max.x = max2(max.x, pt.x);
        max.y = max2(max.y, pt.y);
        max.z = max2(max.z, pt.z);
    }

    void AABB::addTriangle(const Triangle & tri)
    {
        addPoint(tri.a);
        addPoint(tri.b);
        addPoint(tri.c);
    }

    void AABB::sub(const AABB & aabb)
    {
        min.x = max2(min.x, aabb.min.x);
        min.y = max2(min.y, aabb.min.y);
        min.z = max2(min.z, aabb.min.z);

        max.x = min2(max.x, aabb.max.x);
        max.y = min2(max.y, aabb.max.y);
        max.z = min2(max.z, aabb.max.z);
    }

}//end namespace Math
