
#include "stdafx.h"
#include "Model.h"
#include "Pick.h"

#include "TerrainItem.h"
#include "Entity.h"
#include "Camera.h"
#include "TerrinMap.h"
#include "../Physics/Physics.h"
#include "../Physics/LazyConllision.h"
#include "ModelFactory.h"

#include <sstream>

namespace Lazy
{

    static AABB s_defaultAABB(Vector3(-0.5, 0.0f, 0.5f), Vector3(0.5f, 1.0f, 0.5f));
    //////////////////////////////////////////////////////////////////////////
    ///
    //////////////////////////////////////////////////////////////////////////
    TerrainItem::TerrainItem(uint32 id)
        : m_id(id)
        , m_isCollid(true)
        , m_angle(0, 0, 0)
    {
        m_aabb = s_defaultAABB;
    }

    TerrainItem::~TerrainItem(void)
    {
        m_model = nullptr;
    }

    void TerrainItem::update(float elapse)
    {
        SceneNode::update(elapse);

        updateRotationAxis();

        if (!getVisible()) return;
    }

    void TerrainItem::render(IDirect3DDevice9* pDevice)
    {
        SceneNode::render(pDevice);

        //绘制物体AABB
        if (MapConfig::ShowAllItemAABB)
        {
            AABB aabb = getWorldBoundingBox();

            pDevice->SetTransform(D3DTS_WORLD, &matIdentity);
            drawAABB(pDevice, aabb, 0xffffffff);
        }

        //绘制物体八叉树
        if (MapConfig::ShowItemOctree && TerrainMap::instance()->getSelectObj() == this)
        {
            if (m_model && m_model->getMesh())
            {
                OCTreePtr tree = findOCTree(m_model->getMesh());
                if (tree)
                {
                    const Matrix & matWorld = m_model->getWorldMatrix();
                    pDevice->SetTransform(D3DTS_WORLD, &matWorld);
                    tree->render(pDevice);
                }
            }
        }
    }

    void TerrainItem::setModel(ModelPtr model)
    {
        if (m_model)
            this->delChild(m_model);

        m_model = model;

        if (m_model)
        {
            m_aabb = m_model->getWorldBoundingBox();
            this->addChild(m_model);
        }
        else
        {
            m_aabb = s_defaultAABB;
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
        Quaternion rotation;
        rotation.setYawPitchRoll(m_angle.y, m_angle.x, m_angle.z);
        setRotation(rotation);
    }

    bool TerrainItem::intersectWithCursor()
    {
        AABB aabb = getWorldBoundingBox();
        return Pick::instance()->isIntersect(aabb);
    }

    bool TerrainItem::isActive(void)
    {
        return TerrainMap::instance()->getActiveObj() == this;
    }

    void TerrainItem::focusCursor(UINT msg)
    {
        if (m_model)
        {
            if (msg == CM_ENTER || msg == CM_LUP || msg == CM_RUP)
            {
                m_model->setBBVisible(true);
            }
            else if (msg == CM_LEAVE || msg == CM_LDOWN || msg == CM_RDOWN)
            {
                m_model->setBBVisible(false);
            }
        }

        onFocusCursor(msg);
    }

    void TerrainItem::onFocusCursor(UINT)
    {

    }

    float TerrainItem::distToCamera(void)
    {
        return getPosition().distTo(getCamera()->getPosition());
    }

    bool TerrainItem::save(LZDataPtr dataPtr) const
    {
        assert(dataPtr && "TerrainItem::save");

        dataPtr->writeUint(_T("id"), m_id);
        tstring temp;
        if (m_model)
        {
            dataPtr->writeString(_T("path"), m_model->source());
        }

        writeVector3(dataPtr, _T("scale"), getScale());
        writeVector3(dataPtr, _T("pos"),    getPosition());
        writeVector3(dataPtr, _T("angle"), m_angle);
        writeVector3(dataPtr, _T("aabb/min"), m_aabb.min);
        writeVector3(dataPtr, _T("aabb/max"), m_aabb.max);

        std::wostringstream ss;
        for (TerrainChunk *p : m_chunks)
        {
            ss << p->getID() << L" ";
        }
        dataPtr->writeString(L"chunks", ss.str());
        return true;
    }

    bool TerrainItem::load(LZDataPtr dataPtr)
    {
        m_id = dataPtr->readUint(_T("id"));

        Vector3 tmp;
        readVector3(dataPtr, _T("scale"), tmp);
        setScale(tmp);

        readVector3(dataPtr, _T("pos"), tmp);
        setPosition(tmp);


        readVector3(dataPtr, _T("angle"), m_angle);
        //readVector3(dataPtr, _T("aabb/min"), m_aabb.min);
        //readVector3(dataPtr, _T("aabb/max"), m_aabb.max);

        tstring temp;
        temp = dataPtr->readString(_T("path"));
        setModel(ModelFactory::loadModel(temp));

        if (dataPtr->readBool(L"drop", false))
        {
            Vector3 pos = getPosition();
            pos.y = TerrainMap::instance()->getHeight(pos.x, pos.z);
            setPosition(pos);
        }

        updateRotationAxis();
        return true;
    }

    //获取真实的模型矩阵
    void TerrainItem::getAbsModelMatrix(Matrix & mat) const
    {
        mat = getMatrix();
    }

    /** 从chunk中删除自己。*/
    void TerrainItem::removeFromChunks()
    {
        for (TerrainChunk* p : m_chunks)
        {
            p->delItem(this);
        }

        m_chunks.clear();
    }

    void TerrainItem::addChunk(TerrainChunk *pChunk)
    {
        std::vector<TerrainChunk*>::iterator it = std::find(
            m_chunks.begin(), m_chunks.end(), pChunk);

        if (it == m_chunks.end())
            m_chunks.push_back(pChunk);
    }

} // end namespace Lazy
