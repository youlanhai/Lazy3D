#pragma once


namespace Math
{
    class Vector3;

    class Matrix4x4 : public D3DXMATRIX
    {
    public:
        Matrix4x4();
        Matrix4x4(
            float m11, float m12, float m13, float m14,
            float m21, float m22, float m23, float m24,
            float m31, float m32, float m33, float m34,
            float m41, float m42, float m43, float m44
            );
        ~Matrix4x4();

        void makeZero();
        void makeIdentity();

        void makeTranslate(const Vector3 & v);
        void makeTranslate(float x, float y, float z);

        void makeScale(const Vector3 & v);
        void makeScale(float x, float y, float z);

        void makeRatateX(float angle);
        void makeRatateY(float angle);
        void makeRatateZ(float angle);
        void makeRatateYawPitchRoll(float yaw, float pitch, float roll);
        void makeRatateAxis(const Vector3 & v, float angle);

        void makeLookAt(const Vector3 & target, const Vector3 & position, const Vector3 & up);

        void makeOrtho(float w, float h, float zn, float zf);
        void makeOrthoOffCenter(float left, float right, float bottom, float top, float zn, float zf);
        void makePerspective(float fovy, float aspect, float zn, float zf);

        void getRow(int i, Vector3 & v) const;
        void setRow(int i, const Vector3 & v);

        void getCol(int i, Vector3 & v) const;
        void setCol(int i, const Vector3 & v);

        void transpose();

        void invsert();

        float yaw() const;
        float pitch() const;
        float roll() const;

    public://运算符重载

        Vector3 & operator [](int i);

        Matrix4x4 operator * (const Matrix4x4 & right) const
        {
            Matrix4x4 temp;
            D3DXMatrixMultiply(&temp, this, &right);
            return temp;
        }

        const Matrix4x4 & operator *= (const Matrix4x4 & right)
        {
            D3DXMatrixMultiply(this, this, &right);
            return *this;
        }

        Matrix4x4 operator + (const Matrix4x4 & right) const
        {
            Matrix4x4 temp;
            for (int r = 0; r < 4; ++r)
                for (int c = 0; c < 4; ++c)
                    temp.m[r][c] = m[r][c] + right.m[r][c];
            return temp;
        }

        const Matrix4x4 & operator += (const Matrix4x4 & right)
        {
            for (int r = 0; r < 4; ++r)
                for (int c = 0; c < 4; ++c)
                    m[r][c] += right.m[r][c];
            return *this;
        }

        Matrix4x4 operator - (const Matrix4x4 & right) const
        {
            Matrix4x4 temp;
            for (int r = 0; r < 4; ++r)
                for (int c = 0; c < 4; ++c)
                    temp.m[r][c] = m[r][c] - right.m[r][c];
            return temp;
        }

        const Matrix4x4 & operator -= (const Matrix4x4 & right)
        {
            for (int r = 0; r < 4; ++r)
                for (int c = 0; c < 4; ++c)
                    m[r][c] -= right.m[r][c];
            return *this;
        }
    };


    extern const Matrix4x4 matIdentity;

}//end namespace Math
