#pragma once

#include "I3DObject.h"
#include "Model.h"
#include "Pick.h"
#include "Physics.h"
#include "TopBoard.h"

namespace Lazy
{

    enum CursorMsg
    {
        CM_ENTER   = 1,
        CM_LEAVE   = 2,
        CM_LDOWN   = 3,
        CM_LUP     = 4,
        CM_RDOWN   = 5,
        CM_RUP     = 6,
        CM_FORCE_WORD = 0x7fffffff
    };


    LZDLL_API UINT convertMsg(UINT w_msg);


    class LZDLL_API IEntity : public I3DObject
    {
    public:
        IEntity();
        ~IEntity();

        virtual void update(float elapse);

        virtual void render(IDirect3DDevice9* pDevice);

        int getID(void) const { return m_id; }

        bool isActive(void) const;

        ///是否可以被鼠标选择
        virtual bool canSelect(void) const { return true; }

        ///处理鼠标消息
        virtual void focusCursor(UINT msg);

        ///接收到了鼠标消息
        virtual void onFocusCursor(UINT msg);

        ///到相机的距离
        float distToCamera(void) const;

        ///到主角的距离
        float distToPlayer(void) const;

        ///朝向某点
        void lookAtPosition(const Vector3 & pos);

        ModelPtr getModel(void) const { return m_model; }
        void setModel(ModelPtr pModel);

        ///物理系统
        void setPhysics(PhysicsPtr p);
        PhysicsPtr getPhysics(void) const { return m_physics; }

        ///头顶板
        void setTopboard(RefPtr<TopBoard> pboard) { m_topboard = pboard; }
        RefPtr<TopBoard> getTopboard(void) const { return m_topboard; }

        ///可见距离。如果超出可见范围外，将不可见。
        void setShowDistance(float dist) { m_showDistance = dist; }
        float getShowDistance(void) const { return m_showDistance; }

        ///显示包围求
        void enableSphereShow(bool s) { m_bEnableSphereShow = s; }
        bool isSphereShowEnable(void) const { return m_bEnableSphereShow; }

    protected:
        int                 m_id;
        ModelPtr            m_model;
        PhysicsPtr          m_physics;
        RefPtr<TopBoard>   m_topboard;
        float               m_showDistance;//< 最大显示距离（据离玩家）
        bool                m_bEnableSphereShow;//< 在活动时，是否显示包围球
    };

    typedef RefPtr<IEntity> EntityPtr;
//////////////////////////////////////////////////////////////////////////
    class LZDLL_API EntityMgr : public IRenderObj, public IBase
    {
    public:
        typedef VisitPool<EntityPtr>  EntityPool;
        typedef EntityPool::iterator        PoolIterator;

        ///单例
        static EntityMgr * instance();

        EntityPtr entity(int id);

        EntityPtr getActiveEntity(void) { return m_pActiveEntity; }
        void setActiveEntity(EntityPtr pEnt);

        UINT handleMouseEvent(UINT msg, WPARAM wParam, LPARAM lParam);

        EntityPool* getPool(void) { return &m_pool; }

        EntityPtr player() { return m_player; }

        void setPlayer(EntityPtr p);

    public:

        void add(EntityPtr pEnt);

        void removeById(int id);
        void remove(EntityPtr pEnt);

        void clear();

        virtual void update(float fElapse);

        virtual void render(IDirect3DDevice9 * pDevice);


    private:

        EntityPool      m_pool;
        EntityPtr       m_pActiveEntity;
        EntityPtr       m_player;

        EntityMgr(void);
        ~EntityMgr(void);
    };

    LZDLL_API bool isEntitySphereEnable();
    LZDLL_API void setEntitySphereEnable(bool s);

} // end namespace Lazy
