#pragma once

#include "Base.h"
#include "RenderObj.h"

#include "../Physics/PathFind.h"

/** 物理状态*/
enum PHYSICS_STATE
{
    PS_FREE     = 0,    //休闲
    PS_MOVE     = 1,    //移动
    PS_ROTATE   = 2,    //旋转
    PS_ATTACK   = 3,    //攻击
    PS_SKILL    = 4,    //释放技能
    PS_BE_HIT   = 5,    //受击
    PS_DEAD     = 6,    //死亡
    PS_RUN      = 7,    //奔跑
    PS_RIDE     = 8,    //骑马
    PS_JUMP     = 9,    //跳跃
    PS_SWIM     = 10,   //游泳
    PS_FORCE_DWORD = 0x7fffffff
};

class I3DObject;

/** 物理类*/
class LZDLL_API iPhysics : public IBase, public IRenderObj
{
public:
    iPhysics(void);

    explicit iPhysics(I3DObject* pSource);

    ~iPhysics(void);

    virtual void update(float elapse);

    virtual void render(IDirect3DDevice9 * pDevice);

    ///根据路线更新坐标等信息
    void updateByPath();

    void updatePlayer(float elapse);

    ///朝向某点
    void faceTo(const Math::Vector3 & dest);

    ///朝向某个方向
    void faceToDir(const Math::Vector3 & dir);

    ///移动到某点
    void moveTo(const Math::Vector3 & pos);

    ///移动到entity
    void moveToEntity(I3DObject* taget);

    ///寻路
    bool searchPathEx(Physics::WpPtrArray & way, const Math::Vector3 & dest);

    ///寻路
    ///TODO: 使用异步方式计算寻路。
    bool searchPath(const Math::Vector3 & dest);

    ///移动完毕
    virtual void onMoveToFinished(bool succed);

    virtual void onMoveToEntityFinish(bool succed);

    virtual void onSearchToFinished(bool succed);

    ///状态发生变化
    virtual void onStateChange(DWORD oldState);

    ///是否是玩家的物理
    virtual bool isPlayers();

    void enableAI(bool e){ m_aiEnable = e; }

    bool isAIEnable(){ return m_aiEnable; }

    ///触发AI。
    virtual void onAITrigger(){  }

    void setAIInterval(float interval){ m_aiInterval = interval; }

    float getAIInterval(){ return m_aiInterval; }
public:

    void setSource(I3DObject* pSource){ m_pSource = pSource; }

    I3DObject* getSource(void){ return m_pSource; }

    void enable(bool e){ m_enable = e; }

    bool isEnabled(void){ return m_enable; }

    void setState(DWORD state);

    DWORD getState(void){ return m_state; }

    void breakAutoMove();

    //锁定高度
    void enableLockHeight(bool l){ m_lockHInMap = l; }
    void setLockHeight(float h){ m_lockHeight = h ;}
protected:

    void updateAutoMove(float elapse);

    void updateHeight(float elapse);

protected:
    bool            m_enable;       //< 物理是否可用
    I3DObject       *m_pSource;
    DWORD           m_state;        //< 状态
    //自动行走相关
    bool            m_autoMove;     //< 是否在自动行走
    bool            m_bMoveToEntity; //< 是否移动到entity
    I3DObject       *m_pMoveToTarget;
    Math::Vector3     m_nextPos;      //< 下一个目标点
    Math::Vector3     m_prevPos;      //< 前一个点

    bool    m_aiEnable;
    float   m_aiElapse;
    float   m_aiInterval;

    bool    m_collid;
    float   m_jumpingTime;
    bool    m_isJumping;
    bool    m_isFaling;

    bool    m_lockHInMap;
    float   m_lockHeight;

    bool    m_moveByPath;///<是否根据路线移动
    Physics::Vector3Array m_wayPath;///<寻路路线
};

typedef RefPtr<iPhysics> PhysicsPtr;

LZDLL_API float getMaxDistToNpc();
LZDLL_API void setMaxDistToNpc(float dist);

LZDLL_API void enableDrawCollision(bool e);
LZDLL_API bool isDrawCollisionEnable();