#pragma once

#include "Model.h"
#include "../Render/Mesh.h"

namespace Lazy
{
    ///动画实例
    class LZDLL_API AnimalModel : public IModel
    {
    public:
        /** 构造函数。*/
        AnimalModel(void);

        /** 析构函数。*/
        ~AnimalModel(void);

        virtual bool load(const std::wstring & name) override;

        /** 更新*/
        virtual void update(float elapse);

        /** 渲染*/
        virtual void render(IDirect3DDevice9* pDevice);

    public:

        virtual bool playAction(const std::string & name, bool loop) override;

        virtual void stopAction(void) override;

        virtual int getActionCount(void) const override;

        virtual std::string getActionName(int index) const override;

        /** 步进动画*/
        virtual void advanceAnimation(void);

        virtual void setActionSpeed(float s) override { m_speed = s; }

    public:

        bool playAction(LPD3DXANIMATIONSET pAnimSet);

        LPD3DXANIMATIONSET getAnimSet(int n) const;

        LPD3DXANIMATIONSET getAnimSet(const std::string & name) const;

        LPD3DXANIMATIONSET findAnimSet(const std::string & name) const;

        /** 获得动画控制器。*/
        dx::AnimController * getAnimalControler(void) { return m_pAnimControler; }

    protected:
        dx::AnimController * m_pAnimControler;
        MeshPtr         m_pMesh;
        float           m_elapse;
        float           m_elapsedTime;
        BOOL            m_loop;
        float           m_speed;
    };

}// end namespace Lazy
