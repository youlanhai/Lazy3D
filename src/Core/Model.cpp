#include "stdafx.h"
#include "Model.h"
#include "Res.h"
#include "../Physics/PhysicsDebug.h"
#include "../utility/MemoryCheck.h"

namespace Lazy
{

    ///构造函数
    IModel::IModel(void)
        : m_yaw(0.0f)
        , m_pitch(0.0f)
        , m_roll(0.0f)
        , m_scale(1.0f, 1.0f, 1.0f)
        , m_bShowBound(false)
        , m_matWorld(matIdentity)
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
        Matrix mat;
        getCombinedMatrix(mat);
        pDevice->SetTransform(D3DTS_WORLD, &mat);
        pDevice->SetTexture(0, NULL);

        DWORD oldLight;
        pDevice->GetRenderState(D3DRS_LIGHTING, &oldLight);
        pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

        drawAABB(pDevice, m_aabb, 0xffffffff);

        pDevice->SetRenderState(D3DRS_LIGHTING, oldLight);
    }


    void IModel::render(IDirect3DDevice9 * pDevice)
    {
        if (m_bShowBound)
        {
            renderBoundingBox(pDevice);
        }
    }

    void IModel::getCombinedMatrix(Matrix & mat) const
    {
        Matrix rote;
        rote.makeRatateYawPitchRoll(m_yaw, m_pitch, m_roll);
        mat.makeScale(m_scale);
        mat = rote * mat * m_matWorld;
    }

    void IModel::getTransformdAABB(AABB & aabb) const
    {
        Matrix mat;
        Matrix rote;

        rote.makeRatateYawPitchRoll(m_yaw, m_pitch, m_roll);
        mat.makeScale(m_scale);

        mat = rote * mat;

        m_aabb.applyMatrix(aabb, mat);
    }

    /** 播放动画*/
    bool IModel::playAction(const std::wstring &, bool)
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
    void IModel::getActionName(std::wstring &, int) const
    {
    }

    void IModel::setAnimSpeed(float)
    {
    }

} // end namespace Lazy
