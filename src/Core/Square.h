///Square.h
/** 
* @file Square.h
* @author 游蓝海
* @data 2011-10-8
*/

#pragma once

#include "Base.h"
#include <vector>

/** 
* 正方形类:绘制正方形和正方形网格
*/
class LZDLL_API CSquare : public IBase
{
public:
    ///自定义顶点格式
    struct SquareVertex
    {
	    float x,y,z;	//< 未经过坐标变换后的点
	    float nx,ny,nz;	//< 顶点法向量
	    float u,v;		//< 纹理坐标
    };		
    static DWORD D3DFVF_SQUAREVERTEX;//<灵活顶点格式

public:
    ///构造函数
	CSquare();
    ///析构函数
	~CSquare();

    /**
    * 初始化正方形数据
    * @param [in] pDevice 为D3D设备对象
    * @param [in] r 为行数
    * @param [in] c 为列数
    * @param [in] sw 为正方形方格的宽度
    * @param [in] sh 为正方形方格的高度
    */
	bool init(IDirect3DDevice9 *pDevice,int sw,int sh,int r=1,int c=1);

    /**渲染*/
	virtual void render(IDirect3DDevice9 *pDevice); 


    /** 获得边界*/
    CRect getBounds(void);

public://属性

    /**获得行数。*/
    int getRow(void){ return m_nRows; }

    /** 获得列数。*/
    int getCol(void){ return m_nCols; }

    /** 获得网格大小。*/
    float getSquareSize(void){ return (float)m_nWidth; }


protected:
    /**更新缓冲区中的数据*/
	virtual bool updateVertex(IDirect3DDevice9 *pDevice);  

    /**释放资源*/
    void clear(void);

protected:
	IDirect3DVertexBuffer9  *m_pVertexBuffer;	//< 顶点缓冲区
	IDirect3DIndexBuffer9   *m_pIndexBuffer;	//< 索引缓冲区
	int m_nWidth;	    //< 正方形宽
	int m_nHeight;	    //< 正方形高
	int m_nRows;	    //< 正方形网格行数
	int m_nCols;	    //< 正方形网格列数
	int m_nIndexCount;	//< 索引顶点个数
	int m_nVertexCount; //< 顶点个数

    std::vector<WORD>           m_vecIndex;//<索引顶点数组
    std::vector<SquareVertex>   m_vecVertex;//<顶点数组
};
