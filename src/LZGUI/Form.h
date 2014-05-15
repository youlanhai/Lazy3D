#pragma once

#include "Panel.h"
#include "Image.h"


class LZGUI_API CPosFade : public IUpdate
{
public:
    CPosFade(void);

    void reset(CPoint cur, CPoint dest, float speed = -1.0f);

    void setDest(CPoint dest, float speed = -1.0f);

    void update(float fElapse);

    void setCur(CPoint cur){ m_cur = cur; }
    void setSpeed(float v){ m_speed = v; }

    CPoint getCur(void){ return m_cur; }
    int fadeOver(void){ return m_fadeOver; }
private:
    CPoint  m_dest;
    CPoint  m_cur;
    float   m_speed;
    bool    m_dirvx; //速度方向。用于标记移动是否结束
    bool    m_dirvy;
    float   m_vx;
    float   m_vy;
    int     m_fadeOver; //渐变完毕。-1：未完，0：刚好完毕，1：完毕
};

//////////////////////////////////////////////////////////////////////////
class LZGUI_API CForm : public CPanel
{
public:
    CForm(void);

    CForm(int id, PControl parent, LPCSTR image, int x, int y, int w, int h);

    ~CForm();

    virtual void create(int id, PControl parent, LPCSTR image, int x, int y, int w, int h);

    virtual LPCSTR getClassName(void){ return "CForm"; }

    virtual UINT preProcessMsg(UINT msg, WPARAM wParam, LPARAM lParam);

    virtual UINT messageProc(UINT msg, WPARAM wParam, LPARAM lParam);

    //设置背景色
    virtual void setBackColor(COLORREF cr);	//设置颜色

    void setImage(stdstring img);

    CImage* getImagePtr(void){ return m_image.get();}
protected:
    RefPtr<CImage>  m_image;
};
