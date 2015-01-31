#include "stdafx.h"
#include "EffectConstantSetter.h"
#include "RenderDevice.h"
#include "Effect.h"
#include "ShadowMap.h"

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

    // implemented in Core/Camera.cpp
    void effectApplyCameraPosition(EffectConstant *pConst);

    void effectApplyMaterialAmbient(EffectConstant *pConst)
    {
        D3DXCOLOR ambient = rcDevice()->getMaterial().Ambient;
        D3DXCOLOR emissive = rcDevice()->getMaterial().Emissive;
        pConst->bindValue(ambient + emissive);
    }

    void effectApplyMaterialDiffuse(EffectConstant *pConst)
    {
        pConst->bindValue(rcDevice()->getMaterial().Diffuse);
    }

    void effectApplyMaterialSpecular(EffectConstant *pConst)
    {
        D3DXCOLOR cr = rcDevice()->getMaterial().Specular;
        cr.a = rcDevice()->getMaterial().Power;
        pConst->bindValue(cr);
    }

    void effectApplyLightDirection(EffectConstant *pConst)
    {
        pConst->bindValue(ShadowMap::instance()->getLightDirection());
    }

    void effectApplyLightPosition(EffectConstant *pConst)
    {
        pConst->bindValue(ShadowMap::instance()->getLightPosition());
    }

    void effectApplyShadowMapMatrix(EffectConstant *pConst)
    {
        Matrix matrix;
        ShadowMap::instance()->genLightMatrix(matrix);
        pConst->bindValue(matrix);
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
        if (it != s_autoConstMap.end())
        {
            return it->second;
        }
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
        REG_EFFECT_CONST_FACTORY("VIEW_PROJECTION", effectApplyViewProj);
        REG_EFFECT_CONST_FACTORY("WORLD_VIEW", effectApplyWorldView);
        REG_EFFECT_CONST_FACTORY("WORLD_VIEW_PROJECTION", effectApplyWorldViewProj);
        REG_EFFECT_CONST_FACTORY("MATERIAL_AMBIENT", effectApplyMaterialAmbient);
        REG_EFFECT_CONST_FACTORY("MATERIAL_DIFFUSE", effectApplyMaterialDiffuse);
        REG_EFFECT_CONST_FACTORY("MATERIAL_SPECULAR", effectApplyMaterialSpecular);
        REG_EFFECT_CONST_FACTORY("LIGHT_DIRECTION", effectApplyLightDirection);
        REG_EFFECT_CONST_FACTORY("LIGHT_POSITION", effectApplyLightPosition);
        REG_EFFECT_CONST_FACTORY("SHADOWMAP_MATRIX", effectApplyShadowMapMatrix);
        REG_EFFECT_CONST_FACTORY("CAMERA_POSITION", effectApplyCameraPosition);
        
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
