#pragma once

#include "Model.h"
#include "../Render/SkinMesh.h"

///动画实例
class LZDLL_API AnimalModel : public iModel
{
public:
    /** 构造函数。*/
    AnimalModel(void);

    /** 析构函数。*/
    ~AnimalModel(void);
    
    /** 是否有骨骼*/
    virtual bool isSkeleton(void) const override { return true; }

    virtual bool load(const std::wstring & name) override;

    /** 更新*/
    virtual void update(float elapse);

    /** 渲染*/
    virtual void render(IDirect3DDevice9* pDevice);

public:

    /** 播放动画*/
    virtual bool playAction(const std::wstring & name, bool loop) override;

    /** 停止动画*/
    virtual void stopAction(void) override;

    /** 获得动画个数*/
    virtual int getActionCount(void) const override;

    /** 获得动画名称*/
    virtual void getActionName(std::wstring & name, int index) const override;

    /** 步进动画*/
    virtual void advanceAnimation(void);

    virtual void setAnimSpeed(float s) override { m_speed = s; }

public:

    bool playAction(LPD3DXANIMATIONSET pAnimSet);

    LPD3DXANIMATIONSET getAnimSet(int n) const;

    LPD3DXANIMATIONSET getAnimSet(const std::wstring & name) const;

    LPD3DXANIMATIONSET findAnimSet(const std::wstring & name) const;

    /** 获得动画控制器。*/
    LPD3DXANIMATIONCONTROLLER getAnimalControler(void){ return m_pAnimControler; }

protected:
    LPD3DXANIMATIONCONTROLLER m_pAnimControler;
    Lazy::SkinMeshPtr m_pSkinMesh;
    float           m_elapse;
    float           m_elapsedTime;
    bool            m_loop;
    float           m_speed;
};