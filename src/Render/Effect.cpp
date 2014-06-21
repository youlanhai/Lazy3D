#include "stdafx.h"

#include "Effect.h"
#include "RenderDevice.h"


namespace Lazy
{
    static Effect *s_pCurrentEffect = nullptr;

    Effect::Effect(const tstring & file)
        : m_pEffect(nullptr)
        , IResource(file)
    {

    }


    Effect::~Effect()
    {
        SAFE_DELREF_COM(m_pEffect);
        m_pEffect = nullptr;
    }

    bool Effect::load()
    {
        bool result = true;

        if (m_pEffect)
        {
            throw(std::runtime_error("Effect::load has been loaded!"));
        }

        tstring realPath;
        if (!getfs()->getRealPath(realPath, m_source))
        {
            LOG_ERROR(L"Can't find effect file '%s'", m_source.c_str());
            return false;
        }


        LPD3DXBUFFER pError = nullptr;
        HRESULT hr = D3DXCreateEffectFromFile(rcDevice()->getDevice(),
                                              realPath.c_str(), nullptr, nullptr, 0, nullptr, &m_pEffect, &pError);

        if (FAILED(hr))
        {
            result = false;
            LOG_ERROR(_T("create effect from file '%s' failed!0x%x"), realPath.c_str(), hr);
        }

        if (pError)
        {
            std::string buffer((const char*) pError->GetBufferPointer(), pError->GetBufferSize());
            debugMessageA("ERROR: %s", buffer.c_str());
        }
        SAFE_DELREF_COM(pError);

        return result;
    }

    bool Effect::begin(UINT & nPass)
    {
        assert(s_pCurrentEffect == nullptr && "Effect::begin");

        if (SUCCEEDED(m_pEffect->Begin(&nPass, 0)))
        {
            s_pCurrentEffect = this;
            return true;
        }

        return false;
    }

    bool Effect::beginPass(UINT i)
    {
        assert(s_pCurrentEffect == this && "Effect::beginPass");

        return SUCCEEDED(m_pEffect->BeginPass(i));
    }

    void Effect::endPass()
    {
        assert(s_pCurrentEffect == this && "Effect::beginPass");

        m_pEffect->EndPass();
    }

    void Effect::end()
    {
        assert(s_pCurrentEffect == this && "Effect::end");

        s_pCurrentEffect = nullptr;
        m_pEffect->End();
    }


    void Effect::onLostDevice()
    {
        m_pEffect->OnLostDevice();
    }

    void Effect::onResetDevice()
    {
        m_pEffect->OnResetDevice();
    }

    bool Effect::setTechnique(D3DXHANDLE name)
    {
        return SUCCEEDED(m_pEffect->SetTechnique(name));
    }

    void Effect::setTexture(D3DXHANDLE name, dx::Texture *pTexture)
    {
        m_pEffect->SetTexture(name, pTexture);
    }

    void Effect::setMatrix(D3DXHANDLE name, const Matrix & mat)
    {
        m_pEffect->SetMatrix(name, &mat);
    }

    ///////////////////////////////////////////////////////////////////
    dx::Effect * EffectMgr::getEffect(const std::wstring & name)
    {
        EffectPtr ptr = get(name);
        if (ptr) return ptr->getEffect();

        return nullptr;
    }
}//end namespace Lazy