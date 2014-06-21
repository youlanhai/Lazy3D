#include "stdafx.h"
#include "Line.h"

namespace Lazy
{

    Vector3 Line::project(const Vector3 & point)
    {
        Vector3 n = direction(); //直线方向
        Vector3 sp = point - start; //start -> point
        float d = sp * n; //边sp在直线上的投影长度
        Vector3 q = start + n * d; //投影点
        return q;
    }
    bool Line::intersect(Vector3 & cross, const Line & line)
    {
        Vector3 n1 = direction();
        Vector3 n2 = line.direction();
        Vector3 n1xn2 = n1.cross(n2);
        if (n1xn2.isZero()) //直线平行
        {
            return false;
        }
        float dn1xn2 = 1.0f / n1xn2.lengthSq();
        Vector3 p1p2 = line.start - start;

        float d1 = p1p2.cross(n2) * n1xn2 * dn1xn2;

        float d2 = p1p2.cross(n1) * n1xn2 * dn1xn2;

        Vector3 q1 = start + n1 * d1;
        Vector3 q2 = line.start + n2 * d2;
        if (q1.distToSq(q2) != 0.0f)
        {
            return false;
        }
        cross = q1;
        return true;
    }

}