#include "stdafx.h"
#include "I3DObject.h"

#include "App.h"
#include "CursorCamera.h"
#include "TerrinMap.h"


I3DObject::I3DObject(void)
    : m_vPos(0.0f, 0.0f, 0.0f)
    , m_vLook(0.0f, 0.0f, 1.0f)
    , m_vRight(1.0f, 0.0f, 0.0f)
    , m_vUp(0.0f, 1.0f, 0.0f)
    , m_vSpeed(100.0f, 100.0f, 100.0f)
    , m_vScale(1.0f, 1.0f, 1.0f)
    , m_vRotSpeed(1.0f, 1.0f, 1.0f)
    , m_visible(true)
    , m_lockHInMap(false)
    , m_lockHeight(5.0f)
{
    m_aabb.min.set(-20.0f, 0, -20.0f);
    m_aabb.max.set(20.0f, 100.0f, 20.0f);
}

I3DObject::~I3DObject()
{
}

void I3DObject::resetdir()
{
    m_vLook.set(0.0f, 0.0f, 1.0f);
    m_vRight.set(1.0f, 0.0f, 0.0f);
    m_vUp.set(0.0f, 1.0f, 0.0f);
}

void I3DObject::normalizeVector(void)
{
    m_vLook.normalize();

    m_vUp.cross(m_vRight, m_vLook);
    m_vRight.normalize();

    m_vLook.cross(m_vUp, m_vRight);
    m_vUp.normalize();
}

void I3DObject::moveLook(float fElapse)		//前进
{
	m_vPos += m_vLook*(m_vSpeed.z*fElapse);
}

void I3DObject::moveUp(float fElapse)			//上升
{
	m_vPos += m_vUp*(fElapse*m_vSpeed.y);
}

void I3DObject::moveRight(float fElapse)	//平移
{
	m_vPos += m_vRight*(m_vSpeed.x*fElapse);
}

void I3DObject::rotationUp(float angle)		//绕up旋转
{
	Matrix4x4 t;
	t.makeRatateAxis(m_vUp, angle);

    m_vRight.applyNormalMatrix(t);
    m_vLook.applyNormalMatrix(t);

    normalizeVector();
}
void I3DObject::rotationLook(float angle)	//绕Look旋转
{
	Matrix4x4 t;
	t.makeRatateAxis(m_vLook, angle);

    m_vRight.applyNormalMatrix(t);
    m_vUp.applyNormalMatrix(t);

    normalizeVector();
}
void I3DObject::rotationRight(float angle)	//绕right旋转
{
	Matrix4x4 t;
	t.makeRatateAxis(m_vRight, angle);

    m_vLook.applyNormalMatrix(t);
    m_vUp.applyNormalMatrix(t);

    normalizeVector();
}


void I3DObject::rotationAxis(const Vector3 & a, float angle )
{
	Matrix4x4 t;
	t.makeRatateAxis(a, angle);

    m_vLook.applyNormalMatrix(t);
    m_vUp.applyNormalMatrix(t);
    m_vRight.applyNormalMatrix(t);

    normalizeVector();
}

/*绕X、Y、Z轴旋转*/
void I3DObject::rotationX(float angle)
{
    Matrix4x4 t;
    t.makeRatateX(angle);

    m_vLook.applyNormalMatrix(t);
    m_vUp.applyNormalMatrix(t);
    m_vRight.applyNormalMatrix(t);

    normalizeVector();
}

void I3DObject::rotationY(float angle)
{
    Matrix4x4 t;
    t.makeRatateY(angle);

    m_vLook.applyNormalMatrix(t);
    m_vUp.applyNormalMatrix(t);
    m_vRight.applyNormalMatrix(t);

    normalizeVector();
}

void I3DObject::rotationZ(float angle)
{
    Matrix4x4 t;
    t.makeRatateZ(angle);
    
    m_vLook.applyNormalMatrix(t);
    m_vUp.applyNormalMatrix(t);
    m_vRight.applyNormalMatrix(t);

    normalizeVector();
}

void I3DObject::getWorldMatrix(Matrix4x4 & mat) const//获得世界矩阵
{
    mat._11 = m_vRight.x;   mat._12 = m_vRight.y;   mat._13 = m_vRight.z;   mat._14 = 0.0f;
    mat._21 = m_vUp.x;      mat._22 = m_vUp.y;      mat._23 = m_vUp.z;      mat._24 = 0.0f;
    mat._31 = m_vLook.x;    mat._32 = m_vLook.y;    mat._33 = m_vLook.z;    mat._34 = 0.0f;
	mat._41 = m_vPos.x;     mat._42 = m_vPos.y;     mat._43 = m_vPos.z;     mat._44 = 1.0f;
    
    Matrix4x4 matScale;
    matScale.makeScale(m_vScale);
    mat = matScale * mat;
}


void I3DObject::getCameraMatrix(Matrix4x4 & mat) const
{
    float x = -D3DXVec3Dot(&m_vPos, &m_vRight);
    float y = -D3DXVec3Dot(&m_vPos, &m_vUp);
    float z = -D3DXVec3Dot(&m_vPos, &m_vLook);

    mat._11 = m_vRight.x;	mat._12 = m_vUp.x;	 mat._13 = m_vLook.x;	mat._14 = 0.0f;
    mat._21 = m_vRight.y;	mat._22 = m_vUp.y;	 mat._23 = m_vLook.y;	mat._24 = 0.0f;
    mat._31 = m_vRight.z;	mat._32 = m_vUp.z;	 mat._33 = m_vLook.z;	mat._34 = 0.0f;
    mat._41 = x;			mat._42 = y;		 mat._43 = z;			mat._44 = 1.0f;
}

Matrix4x4 I3DObject::getRotationMatrix()
{
    Matrix4x4 mat;
    getWorldMatrix(mat);
    return mat;
}

Matrix4x4 I3DObject::getRotationMatrix2()
{
    Matrix4x4 mat;
    getCameraMatrix(mat);
    return mat;
}


void I3DObject::getWorldAABB(AABB & aabb) const
{
    Matrix4x4 mat;
    getWorldMatrix(mat);
    m_aabb.applyMatrix(aabb, mat);
}

//////////////////////////////////////////////////////////////////////////

I3DObjectFPS::I3DObjectFPS(void)
{
    m_fRotAngLook = 0.0f;//绕look方向的旋转角度
    m_fRotAngRight = 0.0f;
    m_fRotAngUp = 0.0f;
    m_fRotAngX = 0.0f;
    m_fRotAngY = 0.0f;
    m_fRotAngZ = 0.0f;
}

void I3DObjectFPS::addAngleLook(float angle)
{
    m_fRotAngLook += angle;
}

void I3DObjectFPS::addAngleRight(float angle)
{
    m_fRotAngRight += angle;

    float bound = D3DX_PI/3; //旋转界限
    if (m_fRotAngRight > bound)
    {
        m_fRotAngRight = bound;
    }
    else if (m_fRotAngRight < -bound)
    {
        m_fRotAngRight = -bound;
    }
}

void I3DObjectFPS::addAngleUp(float angle)
{
    m_fRotAngUp += angle;
}

void I3DObjectFPS::addAngleX(float angle)
{
    m_fRotAngX += angle;
}

void I3DObjectFPS::addAngleY(float angle)
{
    m_fRotAngY += angle;
}

void I3DObjectFPS::addAngleZ(float angle)
{
    m_fRotAngZ += angle;
}


void I3DObjectFPS::updateMatrix(void)
{
    m_vLook = Vector3(0.0f, 0.0f, 1.0f);
    m_vRight = Vector3(1.0f, 0.0f, 0.0f);
    m_vUp = Vector3(0.0f, 1.0f, 0.0f);
    rotationLook(m_fRotAngLook);
    rotationRight(m_fRotAngRight);
    rotationUp(m_fRotAngUp);
    rotationX(m_fRotAngX);
    rotationY(m_fRotAngY);
    rotationZ(m_fRotAngZ);
}
