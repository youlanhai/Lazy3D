//Game.h
#pragma once

#include "Core/Core.h"
#include "Lzui/Lzui.h"

using namespace Lazy;

//游戏类
class CGame : public CApp
{
public:
	CGame();
	~CGame();

    /*注册窗口类消息。请实现该消息来修改窗口风格。*/
    void onRegisterClass(WNDCLASSEX *pwc);

    /*创建设备消息。实现该方法来修改设备。*/
    void onCreateDevice(D3DPRESENT_PARAMETERS * pParam);

    /*游戏初始化*/
    bool init(void);

    /*游戏更新*/
    void update(void);

    /*渲染*/
    void render(void);

    /*释放资源*/
    void clear(void);

	void updateCamera(float fElapse);

    virtual bool onEvent(const SEvent & event) override;

public:
	/*注意：所有指针成员，需在构造函数中进行赋空值初始化。*/
	bool		            m_bGameStart;
	RefPtr<Camera>	        m_pCamera;
    Projection              m_projection;
    bool                    m_bUseLineMode;
    GUIMgr *                m_pGuiMgr;
    RefPtr<SkyBox>          m_pSkyBox;
};
