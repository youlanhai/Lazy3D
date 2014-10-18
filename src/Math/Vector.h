#pragma once

namespace Lazy
{
    class Matrix;

    //////////////////////////////////////////////////////////////////////////
    //2维向量
    //////////////////////////////////////////////////////////////////////////
    class Vector2 : public D3DXVECTOR2
    {
    public:
        Vector2() {}

        Vector2(float x_, float y_) : D3DXVECTOR2(x_, y_) {}

        void set(float x_, float y_)
        {
            x = x_;
            y = y_;
        }

        float dot(const Vector2 & r) const
        {
            return x * r.x + y * r.y;
        }

        float cross(const Vector2 & r) const
        {
            return x * r.y - y * r.x;
        }

    public://运算符重载
        Vector2 operator+(const Vector2 & r) const
        {
            return Vector2(x + r.x, y + r.y);
        }

        Vector2 operator-(const Vector2 & r) const
        {
            return Vector2(x - r.x, y - r.y);
        }

        const Vector2 & operator+=(const Vector2 & r)
        {
            x += r.x;
            y += r.y;
            return *this;
        }

        const Vector2 & operator-=(const Vector2 & r)
        {
            x -= r.x;
            y -= r.y;
            return *this;
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // 3D vector
    //////////////////////////////////////////////////////////////////////////
    class Vector3 : public D3DXVECTOR3
    {
    public:
        Vector3()
        {}

        Vector3(float fx, float fy, float fz)
            : D3DXVECTOR3(fx, fy, fz)
        {}

        Vector3(const D3DXVECTOR3 & v)
            : D3DXVECTOR3(v.x, v.y, v.z)
        {}

        Vector3(const Vector3 & v)
            : D3DXVECTOR3(v.x, v.y, v.z)
        {}

        inline bool isZero() const
        {
            return x == 0.0f && y == 0.0f && z == 0.0f;
        }

        ///规0
        inline void zero()
        {
            x = 0.0f; y = 0.0f; z = 0.0f;
        }

        ///求负
        inline void negative()
        {
            x = -x; y = -y; z = -z;
        }

        inline void set(float fx, float fy, float fz)
        {
            x = fx; y = fy; z = fz;
        }

        float lengthSq() const
        {
            return D3DXVec3LengthSq(this);
        }

        float length() const
        {
            return D3DXVec3Length(this);
        }

        float distToSq(const Vector3 & p) const
        {
            Vector3 temp(x - p.x, y - p.y, z - p.z);
            return D3DXVec3LengthSq(&temp);
        }

        float distTo(const Vector3 & p) const
        {
            Vector3 temp(x - p.x, y - p.y, z - p.z);
            return D3DXVec3Length(&temp);
        }

        void normalize()
        {
            D3DXVec3Normalize(this, this);
        }

        void normalize(Vector3 & out) const
        {
            D3DXVec3Normalize(&out, this);
        }

        //点乘
        float dot(const Vector3& v) const
        {
            return (*this) * v;
        }

        //差乘
        Vector3 cross(const Vector3& v) const
        {
            Vector3 temp;
            D3DXVec3Cross(&temp, this, &v);
            return temp;
        }

        //差乘
        void cross(Vector3 & out, const Vector3& v) const
        {
            D3DXVec3Cross(&out, this, &v);
        }

        void toD3DXVec3(D3DXVECTOR3 & left) const
        {
            left.x = x;
            left.y = y;
            left.z = z;
        }

        void applyMatrix(const Matrix & mat);

        void applyNormalMatrix(const Matrix & mat);

        void applyMatrix(Vector3 & out, const Matrix & mat) const;

        void applyNormalMatrix(Vector3 & out, const Matrix & mat) const;

        bool tooClose(const Vector3 & v) const
        {
            return distToSq(v) <= MIN_DIST_SQ;
        }

        void projectTo2D(Vector2 & p) const;

        void projectTo2D(int & u, int & v) const;

        int maxAxis() const;

        float minValue() const
        {
            return min2(x, min2(y, z));
        }

        //////////////////////////////////////////////////////////////////////////
        ///运算符重载
        //////////////////////////////////////////////////////////////////////////

        Vector3 operator - (const Vector3& v) const
        {
            return Vector3(x - v.x, y - v.y, z - v.z);
        }

        Vector3 operator-() const
        {
            return Vector3(-x, -y, -z);
        }

        Vector3 operator + (const Vector3& v) const
        {
            return Vector3(x + v.x, y + v.y, z + v.z);
        }

        float operator * (const Vector3& v) const
        {
            return x * v.x + y * v.y + z * v.z;
        }

        Vector3 operator * (float f) const
        {
            return Vector3(x * f, y * f, z * f);
        }

        Vector3 operator / (float f) const
        {
            return Vector3(x / f, y / f, z / f);
        }

        const Vector3 & operator=(const Vector3 & r)
        {
            set(r.x, r.y, r.z);
            return *this;
        }

        const Vector3 & operator=(const D3DXVECTOR3 & r)
        {
            set(r.x, r.y, r.z);
            return *this;
        }

        const Vector3 & operator+=(const Vector3 & r)
        {
            x += r.x; y += r.y; z += r.z;
            return *this;
        }

        const Vector3 & operator-=(const Vector3 & r)
        {
            x -= r.x; y -= r.y; z -= r.z;
            return *this;
        }

        const Vector3 & operator*=(float f)
        {
            x *= f; y *= f; z *= f;
            return *this;
        }

        const Vector3 & operator /= (float f)
        {
            x /= f; y /= f; z /= f;
            return *this;
        }

        float & operator [](int index)
        {
            assert(index >= 0 && index < 3 && "Vector3 index out of range!");
            return ((float*) this)[index];
        }

        float operator [](int index) const
        {
            assert(index >= 0 && index < 3 && "Vector3 index out of range!");
            return ((float*) this)[index];
        }

    };

    //////////////////////////////////////////////////////////////////////////
    // 4D vector
    //////////////////////////////////////////////////////////////////////////
    class Vector4 : public D3DXVECTOR4
    {
    public:
        Vector4()
        {}

        Vector4(const D3DXVECTOR4 & vec)
            : D3DXVECTOR4(vec)
        {}

        Vector4(const D3DXVECTOR3 & vec, float w)
            : D3DXVECTOR4(vec, w)
        {}

        Vector4(float x, float y, float z, float w)
            : D3DXVECTOR4(x, y, z, w)
        {}
    };

    std::ostream & operator<<(std::ostream & out, const Vector2 & v);
    std::ostream & operator<<(std::ostream & out, const Vector3 & v);
    std::istream & operator>>(std::istream & in, Vector3 & v);

    namespace MathConst
    {
        const Vector3 vec3Max = Vector3(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT);
        const Vector3 vec3Min = Vector3(MIN_FLOAT, MIN_FLOAT, MIN_FLOAT);
        const Vector3 vec3Zero = Vector3(0.0f, 0.0f, 0.0f);
        const Vector3 vec3x = Vector3(1.0f, 0.0f, 0.0f);
        const Vector3 vec3y = Vector3(0.0f, 1.0f, 0.0f);
        const Vector3 vec3z = Vector3(0.0f, 0.0f, 1.0f);
    }

}//end namespace Lazy