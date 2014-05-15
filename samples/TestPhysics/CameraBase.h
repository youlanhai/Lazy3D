#pragma once

#include "utility/Utility.h"
#include "Math/Math.h"

#include <d3dx9.h>

class CameraBase : public Lazy::IBase
{
public:
    CameraBase(void);
    virtual ~CameraBase(void);

    virtual void update(float fElapse);
    virtual void render(IDirect3DDevice9 * pDevice);

    inline void updateRotateChange()
    {
        if (m_dirty)
        { 
            updateViewMatrixRotation();
            updateViewMatrixPosition();
        }
    }
    // Rotations
    void rotYaw(float amount); // rotate around y axis
    void rotPitch(float amount); // rotate around x axis
    void rotRoll(float amount); // rotate around z axis
    void rotY(float amout);

    // Move operations
    void moveLook(bool positive);
    void moveRight(float positive);
    void moveUp(float positive);

public:
    const D3DXMATRIX& viewMatrix() ;

    const D3DXVECTOR3& position() const { return m_position; }

    void setPosition(const D3DXVECTOR3& pos){ m_position = pos; }

    float yaw(){ return m_yaw; }
    float pitch(){ return m_pitch; }
    float roll(){ return m_roll; }
    float speed(){ return m_speed; }
    void setSpeed(float speed){ m_speed = speed; }

    const D3DXVECTOR3& look() const { return m_look; }
    const D3DXVECTOR3& up() const { return m_up; }
    const D3DXVECTOR3& right() const { return m_right; }
protected:
    
    //计算观察矩阵
    void updateViewMatrixRotation();

    void updateViewMatrixPosition();

    void yawPitchRollFromMatrix(D3DXMATRIX *matrix);
protected:
    float       m_elapse;
    float       m_roll;
    float       m_pitch;
    float       m_yaw;
    D3DXVECTOR3 m_look;
    D3DXVECTOR3 m_up;
    D3DXVECTOR3 m_right;
    D3DXVECTOR3 m_position;
    D3DXMATRIX  m_matView;
    bool        m_dirty;
    float       m_speed;
};

#define IS_KEY_DOWN(vk) (GetAsyncKeyState(vk) & 0x8000)
