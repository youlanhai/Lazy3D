#pragma once

#include "Vector.h"

namespace Lazy
{
    ///直线
    class Line
    {
    public:
        Line() {}

        Line(const Vector3 & s, const Vector3 & e)
            : start(s), end(e)
        {}

        float length() const
        {
            return start.distTo(end);
        }

        Vector3 direction() const
        {
            Vector3 dir = end - start;
            dir.normalize();
            return dir;
        }

        //线性插值
        Vector3 interpolation(float ratio) const
        {
            return end * ratio + start * (1.0f - ratio);
        }

        //线性插值
        Vector3 interpolation(float rs, float re) const
        {
            float down = rs + re;
            if (down == 0.0f)
            {
                return start;
            }
            down = 1.0f / down;
            //return interpolation(rs*down);
            return (end * rs + start * re) * down;
        }

        //点在直线上的投影
        Vector3 project(const Vector3 & point);

        bool intersect(Vector3 & cross, const Line & line);

    public:
        Vector3 start;
        Vector3 end;
    };
}
