
#include "stdafx.h"

#include "Entity.h"
#include "TerrinMap.h"

#include "Pick.h"
#include "App.h"


//////////////////////////////////////////////////////////////////////////
LZDLL_API cPick* getPick(void)
{
    return cPick::instance();
}

//////////////////////////////////////////////////////////////////////////
/*static*/ cPick* cPick::instance()
{
    static cPick s_pick;
    return &s_pick;
}

cPick::cPick(void)
    : m_pos(0, 0, 0)
    , m_dir(0, 1, 0)
    , m_intersectWithTerrain(false)
    , m_terrainObjEnable(true)
{
}

cPick::~cPick()
{
}

UINT cPick::handleMouseEvent(UINT msg, WPARAM , LPARAM lParam)
{
    if (msg == WM_MOUSEMOVE)
    {
        CPoint pt(lParam);
        convertCursor(pt.x, pt.y);

        queryWithEntity();

        if (m_terrainObjEnable)
        {
            queryWithTerrainObj();
        }
    }
    else if(msg == WM_LBUTTONUP)
    {
        CPoint pt(lParam);
        convertCursor(pt.x, pt.y);

        m_intersectWithTerrain = queryWithTerrain();
    }
    return 0;
}

void cPick::convertCursor(int x, int y)
{
    m_cursor.set(x, y);
    
    FLOAT Rx = 0.0f;
    FLOAT Ry = 0.0f;
    FLOAT mx = 0.0f;
    FLOAT my = 0.0f;

    IDirect3DDevice9* pDevice = getApp()->getDevice();

    D3DVIEWPORT9 mv;
    Matrix4x4   mProj;
    Matrix4x4   mView;
    pDevice->GetViewport(&mv);
    pDevice->GetTransform(D3DTS_PROJECTION, &mProj);

    mx = mProj._11;
    my = mProj._22;
    Rx = (FLOAT)((x * 2.0) - mv.Width) / (mv.Width * mx);
    Ry = (FLOAT)(mv.Height - (y * 2.0)) / (mv.Height * my);

    //相机坐标系下的，原点和射线方向。
    m_pos.set(0.0f, 0.0f, 0.0f);
    m_dir.set(Rx, Ry, 1.0);

    //求相机变换矩阵的逆矩阵。也就是要将坐标从相机空间转换到世界空间。
    pDevice->GetTransform(D3DTS_VIEW, &mView);
    D3DXMatrixInverse(&mView, NULL, &mView);

    //转换到世界坐标系
    m_pos.applyMatrix(mView);
    m_dir.applyNormalMatrix(mView);
    m_dir.normalize();
}

bool cPick::isIntersectWithSphere(const Math::Vector3 & center, float radius)
{
    Math::Vector3 v = m_pos - center;
    float b = 2.0f * m_dir.dot(v);
    float c = v.lengthSq() - radius * radius;

    float del = b * b - 4 * c;
    if (del > 0.0)
    {
        del = sqrtf(del);
        float x1 = ( del - b) / 2.0f;
        float x2 = (-del - b) / 2.0f;
        if ((x1 < 0.0f) && (x2 < 0.0f))
        {
            return false;
        }
        return true;
    }
    return false;
}

bool cPick::isIntersect(const Math::AABB & aabb)
{
    return aabb.intersectsRay(m_pos, m_dir);
}

bool cPick::isIntersect(I3DObject * pObj)
{
    if (!pObj) return false;

    Math::AABB aabb;
    pObj->getWorldAABB(aabb);

    return isIntersect(aabb);
}

bool cPick::queryWithEntity()
{
    EntityPtr selEntity;
    float minDistance = Math::MAX_FLOAT;

    EntityMgr::EntityPool *pool = EntityMgr::instance()->getPool();
    
    pool->lock();
    for (EntityPtr ent : (*pool))
    {
        if (!ent) continue;

        if (!ent->canSelect()) continue;
        
        if (!isIntersect(ent.get())) continue;

        float distSq = m_pos.distToSq(ent->m_vPos);
        
        if (minDistance > distSq)
        {
            selEntity = ent;
            minDistance = distSq;
        }
    }
    pool->unlock();

    EntityMgr::instance()->setActiveEntity(selEntity);

    return bool(selEntity);
}

bool cPick::queryWithTerrain()
{
    return TerrainMap::instance()->intersect(m_posOnTerrain);
}

bool cPick::queryWithTerrainObj()
{
    return TerrainMap::instance()->intersectWithCursor();
}
