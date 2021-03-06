﻿#pragma once

#include "ResMgr.h"
#include "Device.h"

#include <vector>

namespace Lazy
{
    class EffectConstant;
    class EffectConstantSetter;

    ///封装d3d特效
    class Effect : public IResource, public IDevice
    {
    public:
        Effect(const tstring & file);
        ~Effect();

        bool valid() const { return m_pEffect != nullptr;  }

        ///激活技术
        bool begin(UINT & nPass);

        bool beginPass(UINT i);
        void endPass();

        ///结束技术
        void end();

        dx::Effect * getEffect() { return m_pEffect; }

        bool setTechnique(D3DXHANDLE name);
        void setTexture(D3DXHANDLE name, dx::Texture *pTexture);
        void setMatrix(D3DXHANDLE name, const Matrix & mat);
        void setInt(D3DXHANDLE name, int value);

        EffectConstant* getConstant(const std::string & name);

    public://实现接口

        virtual void onLostDevice() override;
        virtual void onResetDevice() override;

        /// 加载资源
        virtual bool load() override;

        ///获得资源类型
        virtual int getType(void) const override { return rt::effect; };

    private:
        typedef std::vector< std::pair<EffectConstantSetter*, EffectConstant*> > AutoConstants;
        typedef std::map<std::string, EffectConstant*> ManualConstant;

        dx::Effect *        m_pEffect;
        AutoConstants       m_autoConstants;
        ManualConstant      m_manualConstants;
    };

    typedef RefPtr<Effect> EffectPtr;

    ///特效管理器
    class EffectMgr : public ResMgr<EffectMgr, EffectPtr>
    {
    public:
        virtual int getType() { return rt::effect; }

        dx::Effect * getEffect(const std::wstring & name);
    };

}//end namespace Lazy
