#pragma once

#include "particle.h"

namespace Lazy
{
    class SceneNode;

    /**雪花类*/
    class LZDLL_API Snow : public ParticleSystem
    {
    public:
        /**构造函数
        * @param [in] numParticles : 粒子数量
        */
        Snow(int maxAmount, const std::wstring & texture, SceneNode* source);

        ///析构函数
        ~Snow(void);

        virtual void render(IDirect3DDevice9 *pDevice);

        virtual void update(float fElapse);

        void setSource(SceneNode* source) { m_pSource = source; }

        void setRange(const Vector3 & min_, const Vector3 & max_);

        void show(bool s) { m_visible = s; }

        bool visible() { return m_visible; }

    protected:

        virtual void updateParticle(Attribute *attribute, float elapse) ;

        ///重置一个粒子的属性
        virtual void resetParticle(Attribute *attribute) ;

        ///设置渲染状态
        virtual void setRenderState(IDirect3DDevice9 *pDevice);

    private:
        SceneNode*  m_pSource;
        Vector3     m_rangeMin;
        Vector3     m_rangeMax;
        bool        m_visible;
    };


} // end namespace Lazy
