#pragma once

#include "RenderObj.h"

class CameraBase : public IBase, public IRenderObj
{
public:
    CameraBase(void);
    virtual ~CameraBase(void);

    virtual void update(float fElapse);
    virtual void render(IDirect3DDevice9 * pDevice);

    // Rotations
    void rotYaw(float amount); // rotate around y axis
    void rotPitch(float amount); // rotate around x axis
    void rotRoll(float amount); // rotate around z axis

    // Move operations
    void moveLook(bool positive);
    void moveRight(float positive);
    void moveUp(float positive);

    void resetdir();

public:
    const Matrix4x4& viewMatrix(){ return m_matView; }

    const Matrix4x4 & projMatrix() const { return m_projection; }

    const Vector3& position() const { return m_position; }

    void setPosition(const Vector3& pos);

    float yaw(){ return m_yaw; }
    float pitch(){ return m_pitch; }
    float roll(){ return m_roll; }
    float speed(){ return m_speed; }
    void setSpeed(float speed){ m_speed = speed; }

    const Vector3& look() const { return m_look; }
    const Vector3& up() const { return m_up; }
    const Vector3& right() const { return m_right; }

    void setup2DCamera(void);//设置2D摄像机(正交投影)

    void setup3DCamera(void);//设置3D摄像机	(透视投影)

    void setNearFar(float fNear, float fFar);

protected:
    
    //计算观察矩阵
    void updateViewMatrixRotation();

    void updateViewMatrixPosition();

protected:
    float       m_elapse;
    float       m_roll;
    float       m_pitch;
    float       m_yaw;
    float       m_speed;
    float		m_fNear;
    float		m_fFar;

    Vector3     m_look;
    Vector3     m_up;
    Vector3     m_right;
    Vector3     m_position;

    Matrix4x4   m_matView;
    Matrix4x4   m_projection;
};
