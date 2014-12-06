#pragma once

namespace Lazy
{
    class Vector3;
    class Matrix;

    class Quaternion : public D3DXQUATERNION
    {
    public:
        Quaternion();
        Quaternion(float x, float y, float z, float w);

        void normalize();

        void setZero();
        void setIdentity();
        void setRotationAxis(const Vector3 & axis, float angle);
        void setMatrix(const Matrix & matrix);
        void setYawPitchRoll(float yaw, float pitch, float roll);

        void setSlerp(const Quaternion & a, const Quaternion & b, float t);
        void setSquad(const Quaternion & pQ1,
                      const Quaternion & a, const Quaternion & b, const Quaternion & c,
                      float t);

        void getRotationAxis(Vector3 & axis, float & angle);
        float getLength() const;
        float getLengthSq() const;

        Quaternion operator * (const Quaternion & v) const;
        const Quaternion & operator *= (const Quaternion & v);

    };

} // end namespace Lazy
