
#include "stdafx.h"
#include "Collision.h"
#include "App.h"
#include "TerrinMap.h"


//////////////////////////////////////////////////////////////////////////


namespace Lazy
{

    //////////////////////////////////////////////////////////////////////////

    void check2WithObj( CollisionPrevent & ci, TerrainItemPtr pobj)
    {
        if(!pobj) return;

        ModelPtr model = pobj->getModel();
        if(!model || !model->isMesh())
        {
            return;
        }

        AABB aabb;
        pobj->getWorldAABB(aabb);
        if(!aabb.intersect(ci.m_aabb)) return;

        Matrix world;
        pobj->getAbsModelMatrix(world);

#if 0
        //二叉树碰撞检测
        BspTreePtr tree = getModelBspTree(model->source());
        if (tree)
        {
            tree->collision(ci, world);
        }

#else

        //Physics库实现碰撞检测
        ci.preventCollision(model->getMesh(), world);

#endif

    }


    void collisionInSpace(CollisionPrevent & ci)
    {
        ChunkPtr node = TerrainMap::instance()->currentNode();
        if(!node) return;

        TerrainChunk::ItemConstIter it;
        FOR_EACH((*node), it)
        {
            check2WithObj(ci, *it);
        }
    }

    bool preventCollision(CollisionPrevent & ci)
    {
        collisionInSpace(ci);
        return ci.m_isCollisiond;
    }

    //////////////////////////////////////////////////////////////////////////
    bool _pickRay(RayCollision & rc, TerrainItemPtr pobj)
    {
        if(!pobj) return false;

        AABB aabb;
        pobj->getWorldAABB(aabb);
        if(!aabb.intersectsRay(rc.m_start, rc.m_dir)) return false;

        ModelPtr model = pobj->getModel();
        if(!model || !model->isMesh())
        {
            return true; //如果没有模型，则只与aabb碰撞检测
        }

        Matrix world;
        pobj->getAbsModelMatrix(world);
        return rc.pick(model->getMesh(), world);
    }

    bool pickRay(RayCollision & rc)
    {
        ChunkPtr node = TerrainMap::instance()->currentNode();
        if (!node) return false;

#if 0
        ObjectConstIter it;
        FOR_EACH((*node), it)
        {
            _pickRay(rc, *it);
        }
#else
        TerrainItemPtr obj = node->pickItem(rc.m_start, rc.m_dir);
        _pickRay(rc, obj);
#endif

        return rc.isCollied();
    }

}//namespace Lazy