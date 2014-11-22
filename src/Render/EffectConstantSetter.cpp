#include "stdafx.h"
#include "EffectConstantSetter.h"
#include "RenderDevice.h"
#include "Effect.h"

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

    void effectApplyWorldViewProj(EffectConstant *pConst)
    {
        pConst->bindValue(rcDevice()->getWorldViewProj());
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
        auto it = s_autoConstMap.find(name);
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
        
        REG_EFFECT_CONST_FACTORY("WORLD", effectApplyWorld);
        REG_EFFECT_CONST_FACTORY("VIEW", effectApplyView);
        REG_EFFECT_CONST_FACTORY("PROJECTION", effectApplyProj);
        REG_EFFECT_CONST_FACTORY("VIEWPROJECTION", effectApplyViewProj);
        REG_EFFECT_CONST_FACTORY("WORLDVIEWPROJECTION", effectApplyWorldViewProj);
        REG_EFFECT_CONST_FACTORY("AmbientColor", effectApplyAmbient);
        REG_EFFECT_CONST_FACTORY("OmitLight", effectApplyOmitLight);
        REG_EFFECT_CONST_FACTORY("DirLight", effectApplyDirLight);
        REG_EFFECT_CONST_FACTORY("SpotLight", effectApplySpotLight);
        
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
