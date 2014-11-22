﻿
#include "stdafx.h"
#include "EffectConstant.h"
#include "Effect.h"

#include "../utility/Log.h"

namespace Lazy
{

    EffectConstant::EffectConstant(dx::Effect * pEffect, D3DXHANDLE handle)
        : m_pEffect(pEffect)
        , m_handle(handle)
    {
    }

    EffectConstant::~EffectConstant()
    {
    }

    void EffectConstant::bindValue(float value)
    {
        m_pEffect->SetFloat(m_handle, value);
    }

    void EffectConstant::bindValue(const float* values, uint32 count)
    {
        m_pEffect->SetFloatArray(m_handle, values, count);
    }

    void EffectConstant::bindValue(int value)
    {
        m_pEffect->SetInt(m_handle, value);
    }

    void EffectConstant::bindValue(const int* values, uint32 count)
    {
        m_pEffect->SetIntArray(m_handle, values, count);
    }

    void EffectConstant::bindValue(const Matrix& value)
    {
        m_pEffect->SetMatrix(m_handle, &value);
    }

    void EffectConstant::bindValue(const Matrix* values, uint32 count, bool transposed)
    {
        if (transposed)
            m_pEffect->SetMatrixArray(m_handle, values, count);
        else
            m_pEffect->SetMatrixTransposeArray(m_handle, values, count);
    }

    void EffectConstant::bindValue(const Vector2& value)
    {
        m_pEffect->SetVector(m_handle, &Vector4(value.x, value.y, 0.f, 0.f));
    }

    void EffectConstant::bindValue(const Vector2* values, uint32 count)
    {
        assert(count > 0);

        if (count == 1)
        {
            bindValue(*values);
            return;
        }

        Vector4 * pVector = (Vector4*) _aligned_malloc(count * sizeof(Vector4), 8);
        for (uint32 i = 0; i < count; ++i)
            pVector[i].set(values[i].x, values[i].y, 0.f, 0.f);

        m_pEffect->SetVectorArray(m_handle, pVector, count);

        _aligned_free(pVector);
    }

    void EffectConstant::bindValue(const Vector3& value)
    {
        m_pEffect->SetVector(m_handle, &Vector4(value, 0.f));
    }

    void EffectConstant::bindValue(const Vector3* values, uint32 count)
    {
        assert(count > 0);

        if (count == 1)
        {
            bindValue(*values);
            return;
        }

        Vector4 * pVector = (Vector4*) _aligned_malloc(count * sizeof(Vector4), 8);
        for (uint32 i = 0; i < count; ++i)
            pVector[i].set(values[i].x, values[i].y, values[i].z, 0.f);

        m_pEffect->SetVectorArray(m_handle, pVector, count);

        _aligned_free(pVector);
    }

    void EffectConstant::bindValue(const Vector4& value)
    {
        m_pEffect->SetVector(m_handle, &value);
    }

    void EffectConstant::bindValue(const Vector4* values, uint32 count)
    {
        m_pEffect->SetVectorArray(m_handle, values, count);
    }

    void EffectConstant::bindValue(TexturePtr texture)
    {
        if (texture)
            m_pEffect->SetTexture(m_handle, texture->getTexture());
    }

    void EffectConstant::bindValue(dx::Texture * texture)
    {
        if (texture)
            m_pEffect->SetTexture(m_handle, texture);
    }
}
