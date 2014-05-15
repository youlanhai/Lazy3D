#pragma once

#include "CameraBase.h"

class CCamera : public CameraBase
{
public:
	enum CameraType //摄像机样式
	{
		FIRST=1,	//第一人称
		THIRD,		//第三人称
        FOLLOW,     //跟随
		FREE,       //自由模式
	};
	CCamera(IDirect3DDevice9* pDevice, CameraType type = THIRD);
	~CCamera(void);

	void setup2DCamera( void );//设置2D摄像机(正交投影)

    void setup3DCamera( void );//设置3D摄像机	(透视投影)

    void setCamareType( CameraType cameraType );
    CameraType getCameraType( void ){ return m_cameraType; } 

    void setNearFar(float fNear,float fFar);

	void setDistance(float fDistance);

	float getDistance(){return m_fDistToPlayer;}

    void setDistRange(float mind, float maxd);

protected:
    void initCamera(IDirect3DDevice9* pDevice, CameraType type = THIRD);

    /** 矫正距离玩家的距离。*/
    void correctDist(void);

protected:
    
    IDirect3DDevice9* m_pd3dDevice;
    CameraType	    m_cameraType;			//照相机类型
	float		m_fNear;
	float		m_fFar;
	float		m_fDistToPlayer;	//照相机离玩家的距（跟随玩家移动用）
    float       m_distMin;
    float       m_distMax;
};
