
#ifndef H_LAZY3D_EFFECT_CONSTANT_H
#define H_LAZY3D_EFFECT_CONSTANT_H

#include "texture.h"
#include "DXConfig.h"

#include "../Math/Math.h"


namespace Lazy
{
    class EffectConstant
    {
    public:

        EffectConstant(dx::Effect * pEffect, D3DXHANDLE handle);
        ~EffectConstant();

        void bindValue(float value);
        void bindValue(const float* values, uint32 count = 1);

        void bindValue(int value);
        void bindValue(const int* values, uint32 count = 1);

        void bindValue(const Matrix& value);
        void bindValue(const Matrix* values, uint32 count, bool transposed);

        void bindValue(const Vector2& value);
        void bindValue(const Vector2* values, uint32 count = 1);

        void bindValue(const Vector3& value);
        void bindValue(const Vector3* values, uint32 count = 1);

        void bindValue(const Vector4& value);
        void bindValue(const Vector4* values, uint32 count = 1);

        void bindValue(const TexturePtr texture);

    private:

        dx::Effect *    m_pEffect;
        D3DXHANDLE      m_handle;
    };

} // end namespace Lazy

#endif //H_LAZY3D_EFFECT_CONSTANT_H
