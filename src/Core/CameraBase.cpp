
#include "stdafx.h"
#include "CameraBase.h"
#include "../Render/RenderDevice.h"

CameraBase::CameraBase(void)
    : m_elapse(0.0f)
    , m_roll(0.0f)
    , m_pitch(D3DX_PI*0.25f)
    , m_yaw(0.0f)
    , m_position(0.0f, 2.0f, -2.0f)
    , m_speed(100.0f)
    , m_fNear(1.0f)
    , m_fFar(1000.0f)
{
    updateViewMatrixRotation();
    setup3DCamera();
}

CameraBase::~CameraBase(void)
{
}

void CameraBase::resetdir()
{
    m_yaw = 0.0f;
    m_roll = 0.0f;
    m_pitch = D3DX_PI*0.25f;

    updateViewMatrixRotation();
}

void CameraBase::update(float fElapse)
{
    m_elapse = fElapse;
}

void CameraBase::render(IDirect3DDevice9 * pDevice)
{
    pDevice->SetTransform(D3DTS_VIEW, &m_matView);
    pDevice->SetTransform(D3DTS_PROJECTION, &m_projection);
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

void CameraBase::setPosition(const Vector3& pos)
{ 
    m_position = pos; 
    updateViewMatrixPosition();
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
    m_matView.makeRatateYawPitchRoll(m_yaw, m_pitch, m_roll);

    m_matView.getRow(0, m_right);
    m_matView.getRow(1, m_up);
    m_matView.getRow(2, m_look);

    m_matView.transpose();

    updateViewMatrixPosition();
}

void CameraBase::updateViewMatrixPosition()
{
    m_matView._41 = - m_position.dot(m_right); 
    m_matView._42 = - m_position.dot(m_up); 
    m_matView._43 = - m_position.dot(m_look);
    m_matView._44 = 1.0f;
}


void CameraBase::setNearFar(float fNear, float fFar)
{
    if (Math::amostZero(m_fNear - fNear) && Math::amostZero(m_fFar - fFar)) return;

    m_fNear = fNear;
    m_fFar = fFar;

    setup3DCamera();
}


void CameraBase::setup3DCamera(void)
{
    float aspect = float(Lazy::rcDevice()->windowWidth()) / Lazy::rcDevice()->windowHeight();

    m_projection.makePerspective(D3DX_PI * 0.25f, aspect, m_fNear, m_fFar);

}

void CameraBase::setup2DCamera(void)
{
    int width = Lazy::rcDevice()->windowWidth();
    int height = Lazy::rcDevice()->windowHeight();

    m_projection.makeOrtho((float)width, (float)height, m_fNear, m_fFar);
}
