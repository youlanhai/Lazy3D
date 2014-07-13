#pragma once


namespace Lazy
{
    class Vector3;
    class Quaternion;

    class Matrix : public D3DXMATRIX
    {
    public:
        Matrix();
        Matrix(
            float m11, float m12, float m13, float m14,
            float m21, float m22, float m23, float m24,
            float m31, float m32, float m33, float m34,
            float m41, float m42, float m43, float m44
        );

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
        void setRotationQuaternion(const Quaternion & q);

        void makeLookAt(const Vector3 & target, const Vector3 & position, const Vector3 & up);

        void makeOrtho(float w, float h, float zn, float zf);
        void makeOrthoOffCenter(float left, float right, float bottom, float top, float zn, float zf);
        void makePerspective(float fovy, float aspect, float zn, float zf);

        void decompose(Vector3 & scale, Quaternion & rotation, Vector3 & position) const;

        void getRow(int i, Vector3 & v) const;
        void setRow(int i, const Vector3 & v);

        void getCol(int i, Vector3 & v) const;
        void setCol(int i, const Vector3 & v);

        void transpose();

        void invert();
        void getInvert(Matrix & out) const;

        float yaw() const;
        float pitch() const;
        float roll() const;

    public://运算符重载

        Vector3 & operator [](int i);
        const Matrix & operator = (const D3DXMATRIX & right);
        const Matrix & operator = (const Matrix & right);

        Matrix operator + (const Matrix & right) const;
        Matrix operator - (const Matrix & right) const;
        Matrix operator * (const Matrix & right) const;

        const Matrix & operator *= (const Matrix & right);
        const Matrix & operator += (const Matrix & right);
        const Matrix & operator -= (const Matrix & right);
    };


    extern const Matrix matIdentity;

}//end namespace Lazy
