
#include "stdafx.h"
#include "Res.h"
#include "Model.h"
#include "Pick.h"

#include "TerrainItem.h"
#include "Entity.h"
#include "CursorCamera.h"
#include "TerrinMap.h"
#include "..\Physics\Physics.h"
#include "..\Physics\LazyConllision.h"
#include "ModelFactory.h"


template<typename T>
struct TR_PredID
{
    explicit TR_PredID(int id) : m_id(id) {}

    bool operator()(T& node)
    {
        return m_id == node.getID();
    }
private:
    int m_id;
};

template<typename T>
struct TR_PredID_Ptr
{
    explicit TR_PredID_Ptr(int id) : m_id(id) {}

    bool operator()(const T& node)
    {
        return m_id == node->getID();
    }
private:
    int m_id;
};

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

#pragma warning(push)
#pragma warning(disable: 4996)
        swscanf(str.c_str(), _T("%f %f %f"), &v.x, &v.y, &v.z);
#pragma warning(pop)
    }

}

/*static*/ cTerrainRes* cTerrainRes::m_pInstance = NULL;

cTerrainRes::cTerrainRes()
{
    m_cache.id = -1;
}

cTerrainRes::~cTerrainRes()
{
}

bool cTerrainRes::load(const std::wstring & filename)
{
    std::wstring realPath;
    if (!Lazy::getfs()->getRealPath(realPath, filename)) return false;

    std::string name;
    Lazy::wcharToChar(name, realPath);
    std::ifstream fin(name.c_str());

    if (!fin.good()) return false;

    int count;
    fin>>count;
    m_resPool.resize(count);
    for (int i=0; i<count; ++i)
    {
        fin>>m_resPool[i];
    }

    return true;
}

int cTerrainRes::getType(int id)
{
    if (id != m_cache.id)
    {
        if (NULL == find(id))
        {
            return 0;
        }
    }
    return m_cache.type;
}

const std::wstring & cTerrainRes::getPath(int id)
{
    if (id != m_cache.id)
    {
        if (NULL == find(id))
        {
            return Lazy::EmptyStr;
        }
    }
    return m_cache.path;
}

TerrainResNode* cTerrainRes::find(int id)
{
    ResIterator it = std::find_if(
        m_resPool.begin(), 
        m_resPool.end(), 
        TR_PredID<TerrainResNode>(id));
    if (it == m_resPool.end())
    {
        return NULL;
    }
    m_cache = *it;
    return &m_cache;
}

/*static*/ cTerrainRes* cTerrainRes::instance()
{
    if (NULL == m_pInstance)
    {
        m_pInstance = new cTerrainRes();
    }
    return m_pInstance;
}

/*static*/ void cTerrainRes::release()
{
    delete m_pInstance;
}

//////////////////////////////////////////////////////////////////////////
static int g_id_allocator = 100000;

TerrainItem::TerrainItem(void)
    : m_collid(true)
    , m_angle(0, 0, 0)
    , m_modeID(0)
    , m_isRef(false)
    , m_refChunk(0)
{

    m_id = g_id_allocator++;
    
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

void TerrainItem::setModelID(int id)
{
    m_modeID = id;

    TerrainResNode* pNode = cTerrainRes::instance()->find(id);
    if (NULL == pNode)
    {
        setModel(NULL);
        return;
    }

    setModel(ModelFactory::loadModel(pNode->path, pNode->type));
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

std::ostream& operator<<(std::ostream & out, TerrainItem & node )
{
    out<<node.m_modeID<<" "
        <<node.getScale()<<" "
        <<node.m_vPos.x<<" "
        <<node.m_vPos.y<<" "
        <<node.m_vPos.z<<" "
        <<node.m_angle.x<<" "
        <<node.m_angle.y<<" "
        <<node.m_angle.z<<" ";
    return out;
}

std::istream& operator>>(std::istream & in, TerrainItem & node )
{
    float scale;
    in>>node.m_modeID
        >>scale
        >>node.m_vPos.x
        >>node.m_vPos.y
        >>node.m_vPos.z
        >>node.m_angle.x
        >>node.m_angle.y
        >>node.m_angle.z;
    node.setModelID(node.m_modeID);
    node.setScale(scale);
    return in;
}

//获取真实的模型矩阵
void TerrainItem::getAbsModelMatrix(Physics::Matrix4x4 & mat) const
{
    getWorldMatrix(mat);

    if (!m_model) return;

    m_model->setWorldMatrix(mat);
    m_model->getCombinedMatrix(mat);
}
