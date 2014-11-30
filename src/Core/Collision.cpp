
#include "stdafx.h"
#include "Collision.h"
#include "App.h"
#include "TerrainMap.h"


//////////////////////////////////////////////////////////////////////////


namespace Lazy
{

    //////////////////////////////////////////////////////////////////////////

    void check2WithObj( CollisionPrevent & ci, SceneNodePtr pobj)
    {
        //TODO 碰撞检测
#if 0
        if(!pobj) return;

        ModelPtr model = pobj->getModel();
        if(!model || !model->getMesh())
        {
            return;
        }

        AABB aabb = pobj->getWorldBoundingBox();
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
#endif
    }


    void collisionInSpace(CollisionPrevent & ci)
    {
        ChunkPtr node = TerrainMap::instance()->getFocusChunk();
        if(!node) return;

        for (SceneNodePtr it : (*node))
        {
            check2WithObj(ci, it);
        }
    }

    bool preventCollision(CollisionPrevent & ci)
    {
        collisionInSpace(ci);
        return ci.m_isCollisiond;
    }

    //////////////////////////////////////////////////////////////////////////
    bool _pickRay(RayCollision & rc, SceneNodePtr pobj)
    {
        //TODO 碰撞检测
#if 0
        if(!pobj) return false;

        AABB aabb = pobj->getWorldBoundingBox();
        if(!aabb.intersectsRay(rc.m_start, rc.m_dir)) return false;

        ModelPtr model = pobj->getModel();
        if(!model || !model->getMesh())
        {
            return true; //如果没有模型，则只与aabb碰撞检测
        }

        Matrix world;
        pobj->getAbsModelMatrix(world);
        return rc.pick(model->getMesh(), world);
#else
        return false;
#endif
    }

    bool pickRay(RayCollision & rc)
    {
        ChunkPtr node = TerrainMap::instance()->getFocusChunk();
        if (!node) return false;

#if 0
        ObjectConstIter it;
        FOR_EACH((*node), it)
        {
            _pickRay(rc, *it);
        }
#else
        SceneNodePtr obj = node->pickItem(rc.m_start, rc.m_dir);
        _pickRay(rc, obj);
#endif

        return rc.isCollied();
    }

}//namespace Lazy