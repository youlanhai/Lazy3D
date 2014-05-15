#include "stdafx.h"
#include "Model.h"
#include "Res.h"
#include "../Physics/PhysicsDebug.h"
#include "../utility/MemoryCheck.h"

///构造函数
iModel::iModel(void)
    : m_yaw(0.0f)
    , m_pitch(0.0f)
    , m_roll(0.0f)
    , m_scale(1.0f, 1.0f, 1.0f)
    , m_bShowBound(false)
{
    D3DXMatrixIdentity(&m_matWorld); //< 世界矩阵

    m_aabb.min.set(-10, -10, -10);
    m_aabb.max.set(10, 10, 10);

}

///析构函数
iModel::~iModel()
{
}



void iModel::renderBoundingBox(IDirect3DDevice9 * pDevice)
{
    Matrix4x4 mat;
    getCombinedMatrix(mat);
    pDevice->SetTransform(D3DTS_WORLD, &mat);
    pDevice->SetTexture(0, NULL);

    DWORD oldLight;
    pDevice->GetRenderState(D3DRS_LIGHTING, &oldLight);
    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    Physics::drawAABB(pDevice, m_aabb, 0xffffffff);

    pDevice->SetRenderState(D3DRS_LIGHTING, oldLight);
}


void iModel::render(IDirect3DDevice9 * pDevice)
{
    if (m_bShowBound)
    {
        renderBoundingBox(pDevice);
    }
}

void iModel::getCombinedMatrix(Matrix4x4 & mat) const
{
    Matrix4x4 rote;
    D3DXMatrixRotationYawPitchRoll(&rote, m_yaw, m_pitch, m_roll);
    D3DXMatrixScaling(&mat, m_scale.x, m_scale.y, m_scale.z);
    mat = rote * mat * m_matWorld;
}

void iModel::getTransformdAABB(Math::AABB & aabb) const
{
    Math::Matrix4x4 mat;
    Math::Matrix4x4 rote;

    D3DXMatrixRotationYawPitchRoll(&rote, m_yaw, m_pitch, m_roll);
    D3DXMatrixScaling(&mat, m_scale.x, m_scale.y, m_scale.z);
    mat = rote * mat;

    m_aabb.applyMatrix(aabb, mat);
}

/** 播放动画*/
bool iModel::playAction(const std::wstring &, bool)
{ 
    return false; 
}

/** 停止动画*/
void iModel::stopAction(void)
{
}

/** 获得动画个数*/
int iModel::getActionCount(void) const 
{ 
    return 0; 
}

/** 获得动画名称*/
void iModel::getActionName(std::wstring &, int) const
{
}

void iModel::setAnimSpeed(float)
{
}