#include "stdafx.h"
#include "Quaternion.h"

#include "Vector.h"
#include "Matrix.h"

namespace Lazy
{

    Quaternion::Quaternion()
    {}

    Quaternion::Quaternion(float x, float y, float z, float w)
        : D3DXQUATERNION(x, y, z, w)
    {}

    void Quaternion::normalize()
    {
        D3DXQuaternionNormalize(this, this);
    }

    void Quaternion::setZero()
    {
        x = y = z = w = 0.0f;
    }

    void Quaternion::setIdentity()
    {
        D3DXQuaternionIdentity(this);
    }

    void Quaternion::setRotationAxis(const Vector3 & axis, float angle)
    {
        D3DXQuaternionRotationAxis(this, &axis, angle);
    }

    void Quaternion::setMatrix(const Matrix & matrix)
    {
        D3DXQuaternionRotationMatrix(this, &matrix);
    }

    void Quaternion::setYawPitchRoll(float yaw, float pitch, float roll)
    {
        D3DXQuaternionRotationYawPitchRoll(this, yaw, pitch, roll);
    }

    void Quaternion::setSlerp(const Quaternion & a, const Quaternion & b, float t)
    {
        D3DXQuaternionSlerp(this, &a, &b, t);
    }

    void Quaternion::setSquad(const Quaternion & pQ1,
                              const Quaternion & a, const Quaternion & b, const Quaternion & c,
                              float t)
    {
        D3DXQuaternionSquad(this, &pQ1, &a, &b, &c, t);
    }

    void Quaternion::getRotationAxis(Vector3 & axis, float & angle)
    {
        D3DXQuaternionToAxisAngle(this, &axis, &angle);
    }

    float Quaternion::getLength() const
    {
        return D3DXQuaternionLength(this);
    }

    float Quaternion::getLengthSq() const
    {
        return D3DXQuaternionLengthSq(this);
    }

    Quaternion Quaternion::operator * (const Quaternion & v) const
    {
        Quaternion temp = *this;
        temp *= v;
        return temp;
    }

    const Quaternion & Quaternion::operator *= (const Quaternion & v)
    {
        D3DXQuaternionMultiply(this, this, &v);
        return *this;
    }

}// end namespace Lazy
