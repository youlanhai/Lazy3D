#include "stdafx.h"

#include "Effect.h"
#include "RenderDevice.h"

#include "EffectConstant.h"
#include "EffectConstantSetter.h"

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
        for (ManualConstant::iterator it = m_manualConstants.begin();
            it != m_manualConstants.end(); ++it)
            delete it->second;

        // EffectConstantSetter 是公用的，不需要在这里释放。
        for (AutoConstants::iterator it = m_autoConstants.begin();
            it != m_autoConstants.end(); ++it)
            delete it->second;

        SAFE_DELREF_COM(m_pEffect);
        m_pEffect = nullptr;
    }

    bool Effect::load()
    {
        if (m_pEffect)
        {
            LOG_ERROR(L"Effect '%s' has been loaded!", m_source.c_str());
            return false;
        }

        tstring realPath;
        if (!getfs()->getRealPath(realPath, m_source))
        {
            LOG_ERROR(L"Failed to find effect file '%s'", m_source.c_str());
            return false;
        }

        LPD3DXBUFFER pError = nullptr;
        HRESULT hr = D3DXCreateEffectFromFile(rcDevice()->getDevice(),
            realPath.c_str(), nullptr, nullptr, 0, nullptr, &m_pEffect, &pError);

        if (pError)
        {
            std::string buffer((const char*) pError->GetBufferPointer(), pError->GetBufferSize());
            debugMessageA("%s", buffer.c_str());
        }
        SAFE_DELREF_COM(pError);

        if (FAILED(hr))
        {
            LOG_ERROR(_T("Failed create effect from file '%s'. code: 0x%x"), realPath.c_str(), hr);
            return false;
        }

        D3DXEFFECT_DESC desc;
        m_pEffect->GetDesc(&desc);
        for (UINT i = 0; i < desc.Parameters; ++i)
        {
            D3DXPARAMETER_DESC parameter;
            D3DXHANDLE hParameter = m_pEffect->GetParameter(NULL, i);
            m_pEffect->GetParameterDesc(hParameter, &parameter);

            if (parameter.Semantic)
            {
                std::pair<EffectConstantSetter*, EffectConstant*> val;
                val.first = EffectConstantSetter::get(parameter.Semantic);
                if (!val.first)
                {
                    LOG_ERROR(_T("Can't find Semantic '%S' for Effect %s"),
                        parameter.Semantic, m_source.c_str());
                }
                else
                {
                    val.second = new EffectConstant(m_pEffect, hParameter);
                    m_autoConstants.push_back(val);
                }
            }
            else
            {
                std::pair<std::string, EffectConstant*> val;
                val.first = parameter.Name;
                val.second = new EffectConstant(m_pEffect, hParameter);
                m_manualConstants.insert(val);
            }
        }
        return true;
    }

    bool Effect::begin(UINT & nPass)
    {
        assert(s_pCurrentEffect == nullptr && "Effect::begin");

        if (FAILED(m_pEffect->Begin(&nPass, 0)))
            return false;

         s_pCurrentEffect = this;

        // 设置自动变量
        for (AutoConstants::iterator it = m_autoConstants.begin();
            it != m_autoConstants.end(); ++it)
            it->first->apply(it->second);

        return true;
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

    EffectConstant* Effect::getConstant(const std::string & name)
    {
        ManualConstant::iterator it = m_manualConstants.find(name);
        if (it != m_manualConstants.end())
            return it->second;

        return nullptr;
    }

    ///////////////////////////////////////////////////////////////////
    dx::Effect * EffectMgr::getEffect(const std::wstring & name)
    {
        EffectPtr ptr = get(name);
        if (ptr) return ptr->getEffect();

        return nullptr;
    }

}//end namespace Lazy