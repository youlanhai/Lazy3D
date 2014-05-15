#pragma once
#include "Button.h"


//////////////////////////////////////////////////////////////////////////
class LZGUI_API CSilider : public CButton
{
public:
    CSilider();

    CSilider(int id, PControl parent, LPCSTR image, int x, int y, int w, int h);

    void create(int id, PControl parent, LPCSTR image, int x, int y, int w, int h);

    virtual LPCSTR getClassName(void){ return "CSilider";}

    void drag(CPoint cur, CPoint old);
    void onDragEnd(void);
    void onMouseLeave(CPoint pt);
    void onMouseWheel(int zDelta, CPoint pt);

    void setRangePos(int low, int high);
    
    float getPos(void);
    void setPos(float percent);

    void setVertical(bool v);

    void setStep(float percent);

    void dec(void);

    void inc(void);

protected:

    void roll(int dt);

protected:
    bool    m_bVertical;
    int     m_lowPos;
    int     m_highPos;
    int     m_nStep;
};

//////////////////////////////////////////////////////////////////////////
class LZGUI_API CScroll : public CForm
{
public:
    CScroll();

    CScroll(int id, PControl parent, LPCSTR image, int x, int y, int w, int h);

    void create(int id, PControl parent, LPCSTR image, int x, int y, int w, int h);

    virtual LPCSTR getClassName(void){ return "CScroll";}

    void setSiliderSize(int w, int h);
    void setVertical(bool vertical);
    void setForeImage(LPCSTR);

    void setRate(float percent);
    float getRate(void);

    /** 设置步长。[0, 1.0]*/
    void setStep(float percent);

    void onMouseWheel(int zDelta, CPoint pt);
    void onScroll(int id, float pos);

    CSilider* getSilider(){ return m_silider.get();}
protected:
    RefPtr<CSilider>    m_silider; //滑块
    int                 m_step;
    bool                m_vertical;//是否垂直滚动条
};
