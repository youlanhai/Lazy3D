#pragma once

void wrapperEntity();


class PyEntity
{
public:
    EntityPtr m_entity;
    object m_physics;


    PyEntity();

    void setPhysics(object physics);
    object getPhysics(){ return m_physics; }

    ModelPtr getModel(){ return m_entity->getModel(); }
    void setModel(ModelPtr m){ m_entity->setModel(m); }

    int getID(){ return m_entity->getID(); }

    float getShowDistance(){ return m_entity->getShowDistance(); }
    void setShowDistance(float v){ m_entity->setShowDistance(v); }

    void enableSphereShow(bool v){ m_entity->enableSphereShow(v); }
    bool isSphereShowEnable(){ return m_entity->isSphereShowEnable(); }

    bool isActive(){ return m_entity->isActive(); }
    float distToCamera(){ return m_entity->distToCamera(); }
    float distToPlayer(){ return m_entity->distToPlayer(); }
    void lookAtPosition(const Math::Vector3 & v){ m_entity->lookAtPosition(v); }

    bool canSelect(){ return m_entity->canSelect(); }
    void onFocusCursor(UINT v){ m_entity->onFocusCursor(v); }
    bool isPlayer(){ return m_entity->isPlayer(); }

    const Math::Vector3 & getPosition(){ return m_entity->getPos(); }
    void setPosition(const Math::Vector3 & p){ m_entity->setPos(p); }

    const Math::Vector3 & getSpeed(){ return m_entity->getSpeed(); }
    void setSpeed(const Math::Vector3 & p){ m_entity->setSpeed(p); }

    const Math::Vector3 & getScale(){ return m_entity->getScale(); }
    void setScale(const Math::Vector3 & p){ m_entity->setScale(p); }

    void show(bool v){ return m_entity->show(v); }
    bool visible(){ return m_entity->visible(); }

    void enableLockHeight(bool v){ return m_entity->enableLockHeight(v); }
    void setLockHeight(float v){ m_entity->setLockHeight(v); }
};