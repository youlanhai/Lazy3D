//Game.h
#pragma once

#include "Core/Core.h"

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

    /**窗口过程。不需要调用默认窗口处理过程，如果未处理消息请返回0，否则请返回非0。*/
    virtual LRESULT wndProc(HWND ,UINT ,WPARAM ,LPARAM );

    void generateWayPoint();

    void generateWayPointMany(Lazy::LZDataPtr maps);

    void searchPath();

private:
	/*注意：所有指针成员，需在构造函数中进行赋空值初始化。*/
	bool		        m_bGameStart;
	RefPtr<CCursorCamera>	m_pCamera;
	RefPtr<cFpsRender>	    m_pFPS;


    //是否要计算路点
    bool m_generateWp;

    //生成路点耗时
    float m_generateUseTime;

    //寻路耗时
    float m_searchTime;

    //路径数据
    Physics::WpPtrArray m_pathData;

    //寻路目标点
    Physics::Vector3 m_destPos;

    //需要显示的chunk id
    int m_focusWayChunk;

    //玩家
    EntityPtr m_player;

};