#include "stdafx.h"
#include "Model.h"
#include "../Physics/PhysicsDebug.h"
#include "../utility/MemoryCheck.h"

namespace Lazy
{

    ///构造函数
    IModel::IModel(void)
        : m_bbVisible(FALSE)
    {
        m_aabb.min.set(-10, -10, -10);
        m_aabb.max.set(10, 10, 10);
    }

    ///析构函数
    IModel::~IModel()
    {
    }


    void IModel::renderBoundingBox(IDirect3DDevice9 * pDevice)
    {
        pDevice->SetTransform(D3DTS_WORLD, &getWorldMatrix());
        pDevice->SetTexture(0, NULL);

        RSHolder rsHolder(pDevice, D3DRS_LIGHTING, FALSE);
        drawAABB(pDevice, m_aabb, 0xffffffff);
    }


    void IModel::render(IDirect3DDevice9 * pDevice)
    {
        Base::render(pDevice);

        if (m_bbVisible)
        {
            renderBoundingBox(pDevice);
        }
    }

    /** 播放动画*/
    bool IModel::playAction(const std::string &, bool)
    {
        return false;
    }

    /** 停止动画*/
    void IModel::stopAction(void)
    {
    }

    /** 获得动画个数*/
    int IModel::getActionCount(void) const
    {
        return 0;
    }

    /** 获得动画名称*/
    std::string IModel::getActionName(int) const
    {
        return "";
    }

    void IModel::setActionSpeed(float)
    {
    }

    float IModel::getHeight() const
    {
        AABB aabb = getWorldBoundingBox();
        aabb.normalization();
        return aabb.max.y - aabb.min.y;
    }

} // end namespace Lazy
