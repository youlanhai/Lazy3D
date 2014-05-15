#include "stdafx.h"
#include "Vector.h"
#include "Matrix.h"

namespace Math
{
    std::ostream & operator<<(std::ostream & out, const Vector2 & v)
    {
        out << "(" << v.x << ", " << v.y << ")";
        return out;
    }

    std::ostream & operator<<(std::ostream & out, const Vector3 & v)
    {
        out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return out;
    }

    std::istream & operator>>(std::istream & in, Vector3 & v)
    {
        char ch;
        do
        {
            in >> ch;
        } while (ch != '(');

        in >> v.x >> ch >> v.y >> ch >> v.z >> ch;

        return in;
    }


    //////////////////////////////////////////////////////////////////////////

    void Vector3::projectTo2D(Vector2 & p) const
    {
        float maxD = fabs(x);
        p.set(y, z);

        float t = fabs(y);
        if (maxD < t)
        {
            maxD = t;
            p.set(x, z);
        }

        t = fabs(z);
        if (maxD < t)
        {
            p.set(x, y);
        }
    }

    void Vector3::projectTo2D(int & u, int & v) const
    {
        float maxD = fabs(x);
        u = AXIS_Y;
        v = AXIS_Z;

        float t = fabs(y);
        if (maxD < t)
        {
            maxD = t;
            u = AXIS_X;
            v = AXIS_Z;
        }

        t = fabs(z);
        if (maxD < t)
        {
            u = AXIS_X;
            v = AXIS_Y;
        }

    }

    int Vector3::maxAxis() const
    {
        float maxD = fabs(x);
        int axis = AXIS_X;

        float t = fabs(y);
        if (maxD < t)
        {
            maxD = t;
            axis = AXIS_Y;
        }

        t = fabs(z);
        if (maxD < t)
        {
            axis = AXIS_Z;
        }

        return axis;
    }

    void Vector3::applyMatrix(const Matrix4x4 & mat)
    {
        D3DXVec3TransformCoord(this, this, &mat);
    }

    void Vector3::applyNormalMatrix(const Matrix4x4 & mat)
    {
        D3DXVec3TransformNormal(this, this, &mat);
    }

    void Vector3::applyMatrix(Vector3 & out, const Matrix4x4 & mat) const
    {
        D3DXVec3TransformCoord(&out, this, &mat);
    }

    void Vector3::applyNormalMatrix(Vector3 & out, const Matrix4x4 & mat) const
    {
        D3DXVec3TransformNormal(&out, this, &mat);
    }

}//end namespace Math