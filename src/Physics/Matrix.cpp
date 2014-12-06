#include "stdafx.h"
#include "Matrix.h"
#include "Vector.h"
#include "Quaternion.h"

namespace Lazy
{
    const Matrix matIdentity(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    Matrix::Matrix()
    {
    }

    Matrix::Matrix(
        float m11, float m12, float m13, float m14,
        float m21, float m22, float m23, float m24,
        float m31, float m32, float m33, float m34,
        float m41, float m42, float m43, float m44
    )
    {
        _11 = m11; _12 = m12; _13 = m13; _14 = m14;
        _21 = m21; _22 = m22; _23 = m23; _24 = m24;
        _31 = m31; _32 = m32; _33 = m33; _34 = m34;
        _41 = m31; _42 = m42; _43 = m43; _44 = m44;
    }

    void Matrix::makeZero()
    {
        memset(this, 0, sizeof(Matrix));
    }

    void Matrix::makeIdentity()
    {
        memcpy(this, &matIdentity, sizeof(Matrix));
    }

    void Matrix::makeTranslate(const Vector3 & v)
    {
        makeIdentity();
        setRow(3, v);
    }

    void Matrix::makeTranslate(float x, float y, float z)
    {
        makeIdentity();
        _41 = x;
        _42 = y;
        _43 = z;
    }

    void Matrix::makeScale(const Vector3 & v)
    {
        makeScale(v.x, v.y, v.z);
    }

    void Matrix::makeScale(float x, float y, float z)
    {
        makeIdentity();
        _11 = x;
        _22 = y;
        _33 = z;
    }


    void Matrix::makeRatateX(float angle)
    {
        makeIdentity();
        float sina = sinf(angle);
        float cosa = cosf(angle);

        _22 = cosa;     _23 = sina;
        _32 = -sina;    _33 = cosa;
    }

    void Matrix::makeRatateY(float angle)
    {
        makeIdentity();
        float sina = sinf(angle);
        float cosa = cosf(angle);

        _11 = cosa;    _13 = sina;
        _31 = sina;    _33 = cosa;
    }

    void Matrix::makeRatateZ(float angle)
    {
        makeIdentity();
        float sina = sinf(angle);
        float cosa = cosf(angle);

        _11 = cosa;     _12 = sina;
        _21 = -sina;    _22 = cosa;
    }

    void Matrix::makeRatateYawPitchRoll(float yaw, float pitch, float roll)
    {
        D3DXMatrixRotationYawPitchRoll(this, yaw, pitch, roll);
    }

    void Matrix::makeRatateAxis(const Vector3 & v, float angle)
    {
        D3DXMatrixRotationAxis(this, &v, angle);
    }

    void Matrix::makeLookAt(const Vector3 & target, const Vector3 & position, const Vector3 & up)
    {
        Vector3 N = target - position;
        N.normalize();

        Vector3 U = up.cross(N);
        U.normalize();

        Vector3 V = N.cross(U);
        N.normalize();

        setCol(0, U);
        setCol(1, V);
        setCol(2, N);
        setCol(3, MathConst::vec3Zero);

        _41 = -position.dot(U);
        _42 = -position.dot(V);
        _43 = -position.dot(N);
        _44 = 1.0f;
    }

    void Matrix::setRotationQuaternion(const Quaternion & q)
    {
        D3DXMatrixRotationQuaternion(this, &q);
    }

    void Matrix::getRow(int i, Vector3 & v) const
    {
        assert(i >= 0 && i < 4);
        v.x = m[i][0];
        v.y = m[i][1];
        v.z = m[i][2];
    }

    void Matrix::setRow(int i, const Vector3 & v)
    {
        assert(i >= 0 && i < 4);
        m[i][0] = v.x;
        m[i][1] = v.y;
        m[i][2] = v.z;
    }

    void Matrix::getCol(int i, Vector3 & v) const
    {
        assert(i >= 0 && i < 4);
        v.x = m[0][i];
        v.y = m[1][i];
        v.z = m[2][i];
    }

    void Matrix::setCol(int i, const Vector3 & v)
    {
        assert(i >= 0 && i < 4);
        m[0][i] = v.x;
        m[1][i] = v.y;
        m[2][i] = v.z;
    }

    void Matrix::transpose()
    {
        D3DXMatrixTranspose(this, this);
    }

    void Matrix::invert()
    {
        D3DXMatrixInverse(this, 0, this);
    }

    void Matrix::getInvert(Matrix & out) const
    {
        D3DXMatrixInverse(&out, 0, this);
    }

    Vector3 & Matrix::operator [](int i)
    {
        assert(i >= 0 && i < 4);
        return reinterpret_cast<Vector3&>(m[i]);
    }

    float Matrix::yaw() const
    {
        return 0.0f;
    }

    float Matrix::pitch() const
    {
        return 0.0f;
    }

    float Matrix::roll() const
    {
        return 0.0f;
    }

    void Matrix::makeOrtho(float w, float h, float zn, float zf)
    {
        D3DXMatrixOrthoLH(this, w, h, zn, zf);
    }

    void Matrix::makeOrthoOffCenter(float left, float right, float bottom, float top, float zn, float zf)
    {
        D3DXMatrixOrthoOffCenterLH(this, left, right, bottom, top, zn, zf);
    }

    void Matrix::makePerspective(float fovy, float aspect, float zn, float zf)
    {
        D3DXMatrixPerspectiveFovLH(this, fovy, aspect, zn, zf);
    }


    const Matrix & Matrix::operator = (const D3DXMATRIX & right)
    {
        memcpy(this, &right, sizeof(D3DXMATRIX));
        return *this;
    }

    const Matrix &  Matrix::operator = (const Matrix & right)
    {
        memcpy(this, &right, sizeof(Matrix));
        return *this;
    }

    Matrix Matrix::operator + (const Matrix & right) const
    {
        Matrix temp;
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                temp.m[r][c] = m[r][c] + right.m[r][c];
        return temp;
    }

    Matrix Matrix::operator - (const Matrix & right) const
    {
        Matrix temp;
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                temp.m[r][c] = m[r][c] - right.m[r][c];
        return temp;
    }

    Matrix Matrix::operator * (const Matrix & right) const
    {
        Matrix temp;
        D3DXMatrixMultiply(&temp, this, &right);
        return temp;
    }

    const Matrix & Matrix::operator += (const Matrix & right)
    {
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                m[r][c] += right.m[r][c];
        return *this;
    }

    const Matrix & Matrix::operator -= (const Matrix & right)
    {
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                m[r][c] -= right.m[r][c];
        return *this;
    }

    const Matrix & Matrix::operator *= (const Matrix & right)
    {
        D3DXMatrixMultiply(this, this, &right);
        return *this;
    }

    void Matrix::decompose(Vector3 & scale, Quaternion & rotation, Vector3 & position) const
    {
        D3DXMatrixDecompose(&scale, &rotation, &position, this);
    }

    /** this = left * right */
    void Matrix::multiply(const Matrix & left, const Matrix & right)
    {
        D3DXMatrixMultiply(this, &left, &right);
    }

    /** this = left * this */
    void Matrix::preMultiply(const Matrix & left)
    {
        D3DXMatrixMultiply(this, &left, this);
    }

    /** this = this * right */
    void Matrix::postMultiply(const Matrix & right)
    {
        D3DXMatrixMultiply(this, this, &right);
    }

}//end namespace Lazy
