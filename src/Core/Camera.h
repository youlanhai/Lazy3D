#pragma once

#include "CameraBase.h"

class I3DObject;

class LZDLL_API CCamera : public CameraBase
{
public:
	enum CameraType //摄像机样式
	{
		FIRST = 1,	//第一人称
		THIRD = 2,	//第三人称
		FREE  = 3,  //自由模式
	};

    explicit CCamera(CameraType type = THIRD);
	~CCamera(void);

    void setCamareType( CameraType cameraType );
    CameraType getCameraType( void ){ return m_cameraType; } 

	void setDistance(float fDistance);

	float getDistance(){return m_fDistToPlayer;}

    void setDistRange(float mind, float maxd);

    void setSource(I3DObject *p){ m_pSource = p; }

protected:

    void initCamera(CameraType type = THIRD);

    /** 矫正距离玩家的距离。*/
    void correctDist(void);

protected:
    
    CameraType  m_cameraType;			//照相机类型
    I3DObject*  m_pSource;
	float		m_fDistToPlayer;	//照相机离玩家的距（跟随玩家移动用）
    float       m_distMin;
    float       m_distMax;
};
