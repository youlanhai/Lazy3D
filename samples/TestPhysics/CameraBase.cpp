
#include "CameraBase.h"

CameraBase::CameraBase(void)
    : m_elapse(0.0f)
    , m_dirty(true)
    , m_roll(0.0f)
    , m_pitch(D3DX_PI*0.25f)
    , m_yaw(0.0f)
    , m_look(0.0f, 0.0f, 1.0f)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_right(1.0f, 0.0f, 0.0f)
    , m_position(0.0f, 0.0f, 0.0f)
    , m_speed(100.0f)
{
    D3DXMatrixIdentity(&m_matView);
}

CameraBase::~CameraBase(void)
{
}

void CameraBase::update(float fElapse)
{
    m_elapse = fElapse;
    updateViewMatrixRotation();
    updateViewMatrixPosition();
}

void CameraBase::render(IDirect3DDevice9 * pDevice)
{
    updateViewMatrixRotation();
    updateViewMatrixPosition();
    pDevice->SetTransform(D3DTS_VIEW, &m_matView);
}

// Rotations
void CameraBase::rotYaw(float amount) // rotate around y axis
{
    m_yaw += amount;
    updateViewMatrixRotation();
}

void CameraBase::rotPitch(float amount) // rotate around x axis
{
    m_pitch += amount;
    updateViewMatrixRotation();
}

void CameraBase::rotRoll(float amount) // rotate around z axis
{
    m_roll += amount;
    updateViewMatrixRotation();
}

// Move operations
void CameraBase::moveLook(bool positive)
{
    if (positive)
    {
        m_position += m_look*(m_speed*m_elapse);
    }
    else
    {
        m_position -= m_look*(m_speed*m_elapse);
    }
    updateViewMatrixPosition();
}

void CameraBase::moveRight(float positive)
{
    if (positive)
    {
        m_position += m_right*(m_speed*m_elapse);
    }
    else
    {
        m_position -= m_right*(m_speed*m_elapse);
    }
    updateViewMatrixPosition();
}

void CameraBase::moveUp(float positive)
{
    if (positive)
    {
        m_position += m_right*(m_speed*m_elapse);
    }
    else
    {
        m_position -= m_right*(m_speed*m_elapse);
    }
    updateViewMatrixPosition();
}

void CameraBase::updateViewMatrixRotation()
{
    m_dirty = false;
    D3DXMatrixRotationYawPitchRoll(&m_matView, m_yaw, m_pitch, m_roll);
    D3DXMatrixInverse(&m_matView, 0, &m_matView);
    m_right = D3DXVECTOR3(m_matView._11, m_matView._21, m_matView._31);
    m_up    = D3DXVECTOR3(m_matView._12, m_matView._22, m_matView._32);
    m_look  = D3DXVECTOR3(m_matView._13, m_matView._23, m_matView._33);
}

void CameraBase::updateViewMatrixPosition()
{
    m_matView._41 = - D3DXVec3Dot( &m_position, &m_right); 
    m_matView._42 = - D3DXVec3Dot( &m_position, &m_up);
    m_matView._43 = - D3DXVec3Dot( &m_position, &m_look);
}

const D3DXMATRIX& CameraBase::viewMatrix() 
{
    return m_matView;
}

void CameraBase::yawPitchRollFromMatrix(D3DXMATRIX *matrix)
{
    m_yaw = atan2(matrix->_21, matrix->_11);
    m_pitch = atan2(-matrix->_31, 
        sqrtf(matrix->_32*matrix->_32 + matrix->_33*matrix->_33));
    m_roll = atan2(matrix->_32, matrix->_33);
}

void CameraBase::rotY(float amout)
{
    return;
}