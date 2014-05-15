#include "stdafx.h"
#include "Matrix.h"
#include "Vector.h"

namespace Math
{
    const Matrix4x4 matIdentity(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
        );

    Matrix4x4::Matrix4x4()
    {
    }

    Matrix4x4::Matrix4x4(
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

    Matrix4x4::~Matrix4x4()
    {
    }

    void Matrix4x4::makeZero()
    {
        memset(this, 0, sizeof(Matrix4x4));
    }

    void Matrix4x4::makeIdentity()
    {
        memcpy(this, &matIdentity, sizeof(Matrix4x4));
    }

    void Matrix4x4::makeTranslate(const Vector3 & v)
    {
        makeIdentity();
        setRow(3, v);
    }

    void Matrix4x4::makeTranslate(float x, float y, float z)
    {
        makeIdentity();
        _41 = x;
        _42 = y;
        _43 = z;
    }

    void Matrix4x4::makeScale(const Vector3 & v)
    {
        makeScale(v.x, v.y, v.z);
    }

    void Matrix4x4::makeScale(float x, float y, float z)
    {
        makeIdentity();
        _11 = x;
        _22 = y;
        _33 = z;
    }


    void Matrix4x4::makeRatateX(float angle)
    {
        makeIdentity();
        float sina = sinf(angle);
        float cosa = cosf(angle);

        _22 = cosa;     _23 = sina;
        _32 = -sina;    _33 = cosa;
    }

    void Matrix4x4::makeRatateY(float angle)
    {
        makeIdentity();
        float sina = sinf(angle);
        float cosa = cosf(angle);

        _11 = cosa;    _13 = sina;
        _31 = sina;    _33 = cosa;
    }

    void Matrix4x4::makeRatateZ(float angle)
    {
        makeIdentity();
        float sina = sinf(angle);
        float cosa = cosf(angle);

        _11 = cosa;     _12 = sina;
        _21 = -sina;    _22 = cosa;
    }

    void Matrix4x4::makeRatateYawPitchRoll(float yaw, float pitch, float roll)
    {
        D3DXMatrixRotationYawPitchRoll(this, yaw, pitch, roll);
    }

    void Matrix4x4::makeRatateAxis(const Vector3 & v, float angle)
    {
        D3DXMatrixRotationAxis(this, &v, angle);
    }

    void Matrix4x4::makeLookAt(const Vector3 & target, const Vector3 & position, const Vector3 & up)
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

    void Matrix4x4::getRow(int i, Vector3 & v) const
    {
        assert(i >= 0 && i < 4);
        v.x = m[i][0];
        v.y = m[i][1];
        v.z = m[i][2];
    }

    void Matrix4x4::setRow(int i, const Vector3 & v)
    {
        assert(i >= 0 && i < 4);
        m[i][0] = v.x;
        m[i][1] = v.y;
        m[i][2] = v.z;
    }

    void Matrix4x4::getCol(int i, Vector3 & v) const
    {
        assert(i >= 0 && i < 4);
        v.x = m[0][i];
        v.y = m[1][i];
        v.z = m[2][i];
    }

    void Matrix4x4::setCol(int i, const Vector3 & v)
    {
        assert(i >= 0 && i < 4);
        m[0][i] = v.x;
        m[1][i] = v.y;
        m[2][i] = v.z;
    }

    void Matrix4x4::transpose()
    {
        D3DXMatrixTranspose(this, this);
    }

    void Matrix4x4::invsert()
    {
        D3DXMatrixInverse(this, 0, this);
    }

    Vector3 & Matrix4x4::operator [](int i)
    {
        assert(i >= 0 && i < 4);
        return reinterpret_cast<Vector3&>(m[i]);
    }

    float Matrix4x4::yaw() const
    {
        return 0.0f;
    }

    float Matrix4x4::pitch() const
    {
        return 0.0f;
    }

    float Matrix4x4::roll() const
    {
        return 0.0f;
    }

    void Matrix4x4::makeOrtho(float w, float h, float zn, float zf)
    {
        D3DXMatrixOrthoLH(this, w, h, zn, zf);
    }

    void Matrix4x4::makeOrthoOffCenter(float left, float right, float bottom, float top, float zn, float zf)
    {
        D3DXMatrixOrthoOffCenterLH(this, left, right, bottom, top, zn, zf);
    }

    void Matrix4x4::makePerspective(float fovy, float aspect, float zn, float zf)
    {
        D3DXMatrixPerspectiveFovLH(this, fovy, aspect, zn, zf);
    }

}//end namespace Math