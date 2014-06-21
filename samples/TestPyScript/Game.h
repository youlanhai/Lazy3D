﻿//Game.h
#pragma once

#include "Core/Core.h"

#define USE_GUI

#ifdef USE_GUI
#include "Lzpy/Lzpy.h"

#include "Render/Texture.h"
#include "Render/Effect.h"
#include "Font/Font.h"
#include "Lzui/Lzui.h"
#endif

using namespace Lazy;

//游戏类
class CGame :public CApp
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
	
    ///消息处理
    virtual bool onEvent(const Lazy::SEvent & event) override;

public:
	/*注意：所有指针成员，需在构造函数中进行赋空值初始化。*/
	bool		        m_bGameStart;
	RefPtr<CCursorCamera>	m_pCamera;
	RefPtr<cFpsRender>	    m_pFPS;
	RefPtr<CCube>		m_pCube;
    RefPtr<CSquare>     m_pSquare;

#ifdef USE_GUI
    RefPtr<Lazy::CGUIManager> m_pGUIMgr;
#endif
};