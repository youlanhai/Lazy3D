#pragma once
#include "Form.h"
#include "Label.h"


//////////////////////////////////////////////////////////////////////////
class LZGUI_API CButton : public CForm

{
public:
    CButton(void);

    /** 构造函数。内部已调用create方法。*/
    CButton(
        int id, 
        PControl parent, 
        LPCSTR caption, 
        LPCSTR image, 
        int x, 
        int y, 
        int w, 
        int h);
    ~CButton(void);

    virtual void create(
        int id, 
        PControl parent, 
        LPCSTR caption, 
        LPCSTR image, 
        int x, 
        int y, 
        int w, 
        int h);

    virtual UINT messageProc(UINT msg, WPARAM wParam, LPARAM lParam);

    virtual LPCSTR getClassName(void){ return "CButton";}

    virtual void render(IDirect3DDevice9 * pDevice);

    virtual void onMouseMove(CPoint pt, CPoint old);
    virtual void onMouseEnter(CPoint pt);
    virtual void onMouseLeave(CPoint pt);
    virtual void onMouseLButton(bool isDown, CPoint pt);

    /** 设置尺寸。重载了父类的方法，设置了Label的尺寸。*/
    virtual void setSize(int w, int h);

    ///设置标题
    virtual void setCaption(stdstring caption);

    /** 设置按钮状态。请尽量调用该方法设置按钮状态，而不是给成员变量m_state直接赋值。*/
    virtual void setBtnState(BUTTON_STATE state);
    BUTTON_STATE getBtnState(void){ return m_state; }

    /** 获得图形名称。如果名称中含有%d，则程序根据当前按钮状态，自动解析为合适的名称。*/
    virtual stdstring getImage(void);

    /** 启禁用颜色绘制。*/
    virtual void enableColor(bool e){m_colorEnable = e; }

    /** 启禁用位置漂移*/
    virtual void enablePosMove(bool e){m_posMoveEnable = e;}

    /** 启禁用用户绘制。如果启用，则颜色绘制和位置漂移会被禁止。*/
    virtual void enableUserDraw(bool d);

    CLabel* getLabelPtr(){ return m_pLabel.get(); }

    void setStateColor(DWORD state, DWORD cr);
    DWORD getStateColor(DWORD state);
protected:
    stdstring       m_imageName;
    CColorFade      m_crFade;
    bool            m_colorEnable;
    bool            m_posMoveEnable;
    RefPtr<CLabel>  m_pLabel;

    BUTTON_STATE    m_state;
    DWORD           m_stateColor[5];
};


//////////////////////////////////////////////////////////////////////////
class CSelectGroup;
typedef CSelectGroup* PSelectGroup;

class LZGUI_API CSelect : public CButton
{
public:
    CSelect(void);
    CSelect(
        int id, 
        PControl parent, 
        PSelectGroup group, 
        LPCSTR caption, 
        LPCSTR image, 
        int x, 
        int y, 
        int w, 
        int h);
    ~CSelect(void);

    virtual void create(
        int id, 
        PControl parent, 
        PSelectGroup group, 
        LPCSTR caption, 
        LPCSTR image, 
        int x, 
        int y, 
        int w, 
        int h);

    virtual LPCSTR getClassName(void){ return "CSelect";}

    void select(bool s);
    bool isSelected(void){ return m_bSelect; }

    virtual void render(IDirect3DDevice9 * pDevice);
    virtual void onMouseLButton(bool isDown, CPoint pt);
    virtual void setBtnState(BUTTON_STATE state);
private:
    bool m_bSelect;
    PSelectGroup m_group;
};

//////////////////////////////////////////////////////////////////////////
class LZGUI_API CSelectGroup  : public CForm
{
public:
    typedef std::list<CSelect*>  SelectContainer;
    typedef SelectContainer::iterator   SelectIterator;

    CSelectGroup(void);
    CSelectGroup(
        int id, 
        PControl parent, 
        LPCSTR image, 
        int x, 
        int y, 
        int w, 
        int h);

    virtual void create(
        int id, 
        PControl parent, 
        LPCSTR image, 
        int x, 
        int y, 
        int w, 
        int h);

    virtual LPCSTR getClassName(void){ return "CSelectGroup";}

    void addSelect(CSelect* ctrl);
    CSelect* getSelected(void);

    void select(CSelect* ctrl);
protected:
    SelectContainer  m_selects;
};
