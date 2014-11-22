
#ifndef H_LAZY3D_EFFECT_CONSTANT_H
#define H_LAZY3D_EFFECT_CONSTANT_H

#include "texture.h"
#include "DXConfig.h"


namespace Lazy
{
    class EffectConstant
    {
    public:

        EffectConstant(dx::Effect * pEffect, D3DXHANDLE handle, LPCSTR name);
        ~EffectConstant();

        LPCSTR getName() const { return m_name; }

        void bindValue(float value);
        void bindValue(const float* values, uint32 count = 1);

        void bindValue(int value);
        void bindValue(const int* values, uint32 count = 1);

        void bindValue(const D3DXMATRIX & value);
        void bindValue(const D3DXMATRIX * values, uint32 count, bool transposed = false);

        void bindValue(const D3DXVECTOR2 & value);
        void bindValue(const D3DXVECTOR2 * values, uint32 count = 1);

        void bindValue(const D3DXVECTOR3 & value);
        void bindValue(const D3DXVECTOR3 * values, uint32 count = 1);

        void bindValue(const D3DXVECTOR4 & value);
        void bindValue(const D3DXVECTOR4 * values, uint32 count = 1);

        void bindValue(const D3DCOLORVALUE & value);
        void bindValue(const D3DCOLORVALUE * values, uint32 count = 1);

        void bindValue(const D3DXCOLOR & value);
        void bindValue(const D3DXCOLOR * values, uint32 count = 1);

        void bindValue(const TexturePtr texture);
        void bindValue(dx::Texture * texture);

    private:

        dx::Effect *    m_pEffect;
        D3DXHANDLE      m_handle;
        LPCSTR          m_name;
    };

} // end namespace Lazy

#endif //H_LAZY3D_EFFECT_CONSTANT_H
