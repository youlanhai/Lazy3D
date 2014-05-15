///SceneFog.h 
/** 
* @file SceneFog.h
* @author 游蓝海
* @data 2011-10-8
* <b>All Right Reserved.</b>
*/

#pragma once

#include "Base.h"

/** 
* 场景雾化类
*/
class LZDLL_API CSceneFog : public IBase
{
public:
    ///雾化类型
    enum FOGTYPE
    {
        FT_LINEAR = 1,
        FT_EXP = 2,
        FT_EXP2 = 3,
        FT_PIXEL = 16,//像素雾化
        FT_FOCE_DWORD = 0x7fffffff
    };
public:
    ///构造函数
    CSceneFog(void);

    ///析构函数
    ~CSceneFog(void);

    ///初始化
    void init(IDirect3DDevice9 *pDevice);

    ///启用雾化
    void setFog(void);

    ///禁用雾化
    void closeFog(void);

    void show(bool s){ m_visible = s; }

    bool visible(){ return m_visible; }

    void toggle(){ show(!visible()); }
public://属性

    ///设置雾化类型
    void setFogType(DWORD ft);

    ///设置雾化范围
    void setStartEnd(float start,float end){ m_fStart = start; m_fEnd = end; }

    ///启用范围雾化
    void setUseRange(bool range){ m_bRange = range; }

    ///设置雾化颜色
    void setColor(DWORD cr){ m_crColor = cr; }

    ///设置密度
    void setDensity(float density){ m_fDensity = density; }

protected:
	void setLinearVertexFog(void);
	void setExpVertexFog(void);
	void setExp2VertexFog(void);
	void setLinearPixelFog(void);
	void setExpPixelFog(void);
	void setExp2PixelFog(void);

private:
    DWORD               m_ftType;//雾化类型
    IDirect3DDevice9    *m_pDevice;
    float               m_fStart;
    float               m_fEnd;
    DWORD               m_crColor;
    bool                m_bRange;
    float               m_fDensity;//密度
    bool                m_visible;
};
