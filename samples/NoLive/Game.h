//Game.h
#pragma once

#include "../LZ3DEngine/LZ3DEngine.h"

struct IGun;
class CPlayer;

//游戏类
class CGame :public CApp
{
public:
	CGame();
	~CGame();

    /**注册窗口类消息。请实现该消息来修改窗口风格。*/
    void onRegisterClass(WNDCLASSEX *pwc);

    /**创建设备消息。实现该方法来修改设备。*/
    void onCreateDevice(D3DPRESENT_PARAMETERS * pParam);

    /**游戏初始化*/
    bool init(void);

    /**游戏更新*/
    void update(void);

    /**渲染*/
    void render(void);

    /**释放资源*/
    void clear(void);

    /**渲染文字*/
	void renderText();

    /**更新摄像机*/
	void updateCamera(float fElapse);

    ///绘制靶心
    void renderTarget();

public:
	/*注意：所有指针成员，需在构造函数中进行赋空值初始化。*/
	bool		        m_bGameStart;
	RefPtr<CCamera>	    m_pCamera;
	RefPtr<CFont2D>	    m_pFont;
	RefPtr<CFPS>	    m_pFPS;
	RefPtr<CKeyboard>	m_pKeyboard;//键盘
	RefPtr<CCube>		m_pCube;//立方体
    RefPtr<CTerrain>    m_pSquare;//地面
    RefPtr<IGun>        m_pGun;//枪支
    RefPtr<CPlayer>     m_pPlayer;//玩家
    RefPtr<C2DFace>     m_pTarget;//靶心
    RefPtr<CSnow>       m_pSnow;
    RefPtr<CSceneFog>   m_pSceneFog;
    RefPtr<CParticleGun> m_pParticleGun;
    RefPtr<CSpritManager> m_pSprManager;
};