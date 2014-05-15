#pragma once
#include "Control.h"


/** 图像控件。*/
class LZGUI_API CImage : public IControl
{
public:
    CImage(void);
    CImage(
        int id, 
        PControl parent, 
        stdstring image, 
        int x, 
        int y, 
        int w, 
        int h);
    ~CImage(void);

    void create(
        int id, 
        PControl parent, 
        stdstring image, 
        int x, 
        int y, 
        int w, 
        int h);

    virtual void update(float fElapse);
    virtual void render(IDirect3DDevice9 * pDevice);

    ///设置顶点的纹理UV。索引从左上角开始，顺时针转。
    void setTexUV(int index, float u, float v);

protected:
    virtual LPCSTR getClassName(void){ return "CImage"; }

    /** 更新顶点。重载此函数，以实现自定义绘制。*/
    virtual  void updateVertex(void);
protected:
    UIVertex    m_vertexs[4];
    float       m_texUV[4][2];//< 顶点UV坐标
};