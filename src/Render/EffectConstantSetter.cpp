#include "stdafx.h"
#include "EffectConstantSetter.h"
#include "RenderDevice.h"
#include "Effect.h"
#include "../Core/Camera.h"

#include <algorithm>

namespace Lazy
{

    //////////////////////////////////////////////////////////////////
    ///
    //////////////////////////////////////////////////////////////////

    typedef void(*EffectApplyFun)(EffectConstant *);

    class EffectConstantProxy : public EffectConstantSetter
    {
        EffectApplyFun fun_;
    public:
        EffectConstantProxy(EffectApplyFun fun)
            : fun_(fun)
        {
            assert(fun && "EffectConstantFun - fun must not be null!");
        }

        virtual void apply(EffectConstant *pConst) override
        {
            (*fun_)(pConst);
        }
    };

    //////////////////////////////////////////////////////////////////
    ///
    //////////////////////////////////////////////////////////////////

    void effectApplyWorld(EffectConstant *pConst)
    {
        pConst->bindValue(rcDevice()->getWorld());
    }

    void effectApplyView(EffectConstant *pConst)
    {
        pConst->bindValue(rcDevice()->getView());
    }
    
    void effectApplyProj(EffectConstant *pConst)
    {
        pConst->bindValue(rcDevice()->getProj());
    }

    void effectApplyViewProj(EffectConstant *pConst)
    {
        pConst->bindValue(rcDevice()->getViewProj());
    }

    void effectApplyWorldView(EffectConstant *pConst)
    {
        pConst->bindValue(rcDevice()->getWorldView());
    }

    void effectApplyWorldViewProj(EffectConstant *pConst)
    {
        pConst->bindValue(rcDevice()->getWorldViewProj());
    }

    void effectApplyCameraPosition(EffectConstant *pConst)
    {
        pConst->bindValue(getCamera()->getPosition());
    }

    void effectApplyAmbient(EffectConstant *pConst)
    {
        /*Color color;

        LightContainerPtr lights =  renderDev()->getLightContainer();
        if(lights) color = lights->getAmbientColor();

        pConst->bindValue(color);*/
    }

    void effectApplyOmitLight(EffectConstant *pConst)
    {
    }

    void effectApplyDirLight(EffectConstant *pConst)
    {
    }

    void effectApplySpotLight(EffectConstant *pConst)
    {
    }
    
    //////////////////////////////////////////////////////////////////
    ///
    //////////////////////////////////////////////////////////////////

    /*static*/ std::map<std::string, EffectConstantSetter*> EffectConstantSetter::s_autoConstMap;

    EffectConstantSetter::EffectConstantSetter()
    {

    }

    EffectConstantSetter::~EffectConstantSetter()
    {

    }

    /*static*/ EffectConstantSetter * EffectConstantSetter::get(const std::string & name)
    {
        std::string tempName = name;
        std::transform(tempName.begin(), tempName.end(), tempName.begin(), tolower);

        auto it = s_autoConstMap.find(tempName);
        if (it != s_autoConstMap.end()) return it->second;

        return nullptr;
    }

    /*static*/ void EffectConstantSetter::set(const std::string & name, EffectConstantSetter * autoConst)
    {
        s_autoConstMap.insert(std::make_pair(name, autoConst));
    }

    /*static */void EffectConstantSetter::init()
    {
#define REG_EFFECT_CONST_FACTORY(TYPE, FACTORY) \
    EffectConstantSetter::set(TYPE, new EffectConstantProxy(FACTORY))
        
        REG_EFFECT_CONST_FACTORY("world", effectApplyWorld);
        REG_EFFECT_CONST_FACTORY("view", effectApplyView);
        REG_EFFECT_CONST_FACTORY("projection", effectApplyProj);
        REG_EFFECT_CONST_FACTORY("viewprojection", effectApplyViewProj);
        REG_EFFECT_CONST_FACTORY("worldview", effectApplyWorldView);
        REG_EFFECT_CONST_FACTORY("worldviewprojection", effectApplyWorldViewProj);
        REG_EFFECT_CONST_FACTORY("ambientcolor", effectApplyAmbient);
        REG_EFFECT_CONST_FACTORY("omitlight", effectApplyOmitLight);
        REG_EFFECT_CONST_FACTORY("dirlight", effectApplyDirLight);
        REG_EFFECT_CONST_FACTORY("spotlight", effectApplySpotLight);
        REG_EFFECT_CONST_FACTORY("cameraposition", effectApplyCameraPosition);
        
#undef REG_EFFECT_CONST_FACTORY
    }

    /*static*/ void EffectConstantSetter::fini()
    {
        for (auto it = s_autoConstMap.begin(); it != s_autoConstMap.end(); ++it)
        {
            delete it->second;
        }
        s_autoConstMap.clear();
    }


}//end namespace Lazy
