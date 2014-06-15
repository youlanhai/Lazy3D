
#include "stdafx.h"
#include "Res.h"
#include "Model.h"
#include "Pick.h"

#include "TerrainItem.h"
#include "Entity.h"
#include "CursorCamera.h"
#include "TerrinMap.h"
#include "../Physics/Physics.h"
#include "../Physics/LazyConllision.h"
#include "ModelFactory.h"


namespace Lazy
{

    void writeVector3(LZDataPtr dataPtr, const Lazy::tstring & tag, const Physics::Vector3 & v)
    {
        Lazy::tstring str;
        formatString(str, _T("%f %f %f"), v.x, v.y, v.z);
        dataPtr->writeString(tag, str);
    }

    void readVector3(LZDataPtr dataPtr, const Lazy::tstring & tag, Physics::Vector3 & v)
    {
        Lazy::tstring str = dataPtr->readString(tag);
        if(str.empty())
        {
            v.zero();
            return;
        }

        swscanf(str.c_str(), _T("%f %f %f"), &v.x, &v.y, &v.z);
    }

}

//////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////
TerrainItem::TerrainItem(void)
    : m_collid(true)
    , m_angle(0, 0, 0)
    , m_isRef(false)
    , m_refChunk(0)
{
    Lazy::generateGUID32(m_uuid);
    m_aabb.min.set(-10, -10, -10);
    m_aabb.max.set(10, 10, 10);
}

//克隆一个副本，此副本必定是引用的。
TerrainItemPtr TerrainItem::clone() const
{
    return new TerrainItem(*this);
}

TerrainItem::~TerrainItem(void)
{
    m_model = nullptr;
}

void TerrainItem::update(float elapse)
{
    updateRotationAxis();

    if (!visible()) return;
    
    if (m_model) m_model->update(elapse);
}

void TerrainItem::render(IDirect3DDevice9* pDevice)
{
    //绘制物体本身
    if (visible() && !m_isRef && m_model)
    {
        Physics::Matrix4x4 matWorld;
        getWorldMatrix(matWorld);
        m_model->setWorldMatrix(matWorld);
        m_model->render(pDevice);
    }

    //绘制物体AABB
    if(!m_isRef && MapConfig::ShowAllItemAABB)
    {
        Physics::AABB aabb;
        getWorldAABB(aabb);

        Matrix4x4 matWorld;
        D3DXMatrixIdentity(&matWorld);
        pDevice->SetTransform(D3DTS_WORLD, &matWorld);
        Physics::drawAABB(pDevice, aabb, 0xffffffff);
    }

    //绘制物体八叉树
    if (MapConfig::ShowItemOctree && TerrainMap::instance()->getSelectObj() == this)
    {
        if (m_model && m_model->isMesh())
        {
            Physics::OCTreePtr tree = Physics::findOCTree(m_model->getMesh());
            if (tree)
            {
                Matrix4x4 matWorld;
                m_model->getCombinedMatrix(matWorld);
                pDevice->SetTransform(D3DTS_WORLD, &matWorld);
                tree->render(pDevice);
            }
        }
    }
}

void TerrainItem::setModel(ModelPtr model)
{
    m_model = model;

    if (m_model)
    {
        m_model->getTransformdAABB(m_aabb);
    }
    else
    {
        m_aabb.min.set(-10, -10, -10);
        m_aabb.max.set(10, 10, 10);
    }
}

void TerrainItem::addLookAngle(float angle)
{
    m_angle.z = angle;
}

void TerrainItem::addUpAngle(float angle)
{
    m_angle.y = angle;
}

void TerrainItem::addRightAngle(float angle)
{
    m_angle.x = angle;
}

void TerrainItem::updateRotationAxis(void)
{
    Physics::Matrix4x4 mat;
    mat.makeRatateYawPitchRoll(m_angle.y, m_angle.x, m_angle.z);

    m_vRight = mat[0];
    m_vUp = mat[1];
    m_vLook = mat[2];

    normalizeVector();
}

bool TerrainItem::intersectWithCursor()
{
    return cPick::instance()->isIntersect(this);
}

bool TerrainItem::isActive(void)
{
    return TerrainMap::instance()->getActiveObj() == this;
}

void TerrainItem::focusCursor(UINT msg)
{
    if (m_model)
    {
        if (msg==CM_ENTER || msg==CM_LUP || msg ==CM_RUP)
        {
            m_model->showBound(true);
        }
        else if (msg==CM_LEAVE || msg==CM_LDOWN || msg==CM_RDOWN)
        {
            m_model->showBound(false);
        }
    }

    onFocusCursor(msg);
}

void TerrainItem::onFocusCursor(UINT)
{

}

float TerrainItem::distToCamera(void)
{
    return m_vPos.distTo(getCamera()->position());
}

bool TerrainItem::save(Lazy::LZDataPtr dataPtr) const
{
    assert(dataPtr && "TerrainItem::save");

    if(m_isRef) return false;

    dataPtr->writeString(_T("uuid"), m_uuid);
    Lazy::tstring temp;
    if(m_model)
    {
        dataPtr->writeString(_T("path"), m_model->source());

        int type = ModelType::StaticModel;
        if(m_model->isSkeleton()) type = ModelType::SkinModel;
        dataPtr->writeInt(_T("type"), type);
    }

    Lazy::writeVector3(dataPtr, _T("scale"), m_vScale);
    Lazy::writeVector3(dataPtr, _T("pos"), m_vPos);
    Lazy::writeVector3(dataPtr, _T("angle"), m_angle);
    Lazy::writeVector3(dataPtr, _T("aabb/min"), m_aabb.min);
    Lazy::writeVector3(dataPtr, _T("aabb/max"), m_aabb.max);

    return true;
}

bool TerrainItem::load(Lazy::LZDataPtr dataPtr)
{
    if (dataPtr->readBool(_T("refrence"), false)) return false;

    m_uuid = dataPtr->readString(_T("uuid"));
    m_isRef = false;
    m_refChunk = 0;

    Lazy::readVector3(dataPtr, _T("scale"), m_vScale);
    Lazy::readVector3(dataPtr, _T("pos"), m_vPos);
    Lazy::readVector3(dataPtr, _T("angle"), m_angle);
    //Lazy::readVector3(dataPtr, _T("aabb/min"), m_aabb.min);
    //Lazy::readVector3(dataPtr, _T("aabb/max"), m_aabb.max);

    Lazy::tstring temp;
    temp = dataPtr->readString(_T("path"));
    if(!temp.empty())
    {
        int type = dataPtr->readInt(_T("type"));
        setModel(ModelFactory::loadModel(temp, type));
    }

    if (dataPtr->readBool(L"drop", false))
    {
        m_vPos.y = TerrainMap::instance()->getHeight(m_vPos.x, m_vPos.z);
    }

    updateRotationAxis();
    return true;
}

//获取真实的模型矩阵
void TerrainItem::getAbsModelMatrix(Physics::Matrix4x4 & mat) const
{
    getWorldMatrix(mat);

    if (!m_model) return;

    m_model->setWorldMatrix(mat);
    m_model->getCombinedMatrix(mat);
}
