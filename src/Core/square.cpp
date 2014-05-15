//Square.cpp
#include "stdafx.h"
#include "Square.h"

////////////////////class CSquare start///////////////////

//定义灵活顶点格式
DWORD CSquare::D3DFVF_SQUAREVERTEX = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;

CSquare::CSquare()
{
	m_pVertexBuffer=NULL;
	m_pIndexBuffer=NULL;
    m_nRows = 1;
    m_nCols = 1;
	m_nWidth=1;
	m_nHeight=1;
    m_nIndexCount = 0;	//索引顶点个数
    m_nVertexCount = 0; //顶点个数
}
CSquare::~CSquare()
{
	clear();
}

void CSquare::clear(void)
{

    SafeRelease(m_pVertexBuffer);
    SafeRelease(m_pIndexBuffer);

    m_vecVertex.clear();
    m_vecIndex.clear();
}

/**
* 初始化正方形数据
* @param [in] pDevice 为D3D设备对象
* @param [in] r 为行数
* @param [in] c 为列数
* @param [in] sw 为正方形方格的宽度
* @param [in] sh 为正方形方格的高度
*/
bool CSquare::init(IDirect3DDevice9 *pDevice,int sw,int sh,int r,int c)
{
    clear();

	m_nWidth = sw;
	m_nHeight = sh;
	m_nRows = r;
    m_nCols = c;

	m_nVertexCount=(m_nRows+1)*(m_nCols+1);//顶点总数
	m_nIndexCount=6 * m_nRows * m_nCols ;//索引顶点个数，共m*n个正方形，即2*m*n个三角形，即3*2*m*n个点
	
    m_vecVertex.resize(m_nVertexCount);
    m_vecIndex.resize(m_nIndexCount);
	

	//创建顶点缓冲区
	if(FAILED(pDevice->CreateVertexBuffer(
        sizeof(SquareVertex)*m_nVertexCount,
		0,
        D3DFVF_SQUAREVERTEX,
        D3DPOOL_MANAGED,
        &m_pVertexBuffer,
        NULL)))
    {
        return false;
    }

	//创建索引缓冲区
	if(FAILED(pDevice->CreateIndexBuffer(
        m_nIndexCount*sizeof(WORD),
		0,
        D3DFMT_INDEX16,
        D3DPOOL_MANAGED,
        &m_pIndexBuffer,
        NULL)))
    {
        return false;
    }

	updateVertex(pDevice);

	return true;
}

//更新缓冲区中的数据
bool CSquare::updateVertex(LPDIRECT3DDEVICE9)
{
    int verIndex;
    float dx = - m_nCols * m_nWidth * 0.5f;
    float dy = m_nRows * m_nHeight * 0.5f;

    //以下是创建网格数据
    for(int y=0; y<m_nRows+1; ++y)
    {
        for(int x=0;x<m_nCols+1;++x)
        {
            verIndex = y * ( m_nCols + 1 ) + x;//顶点索引
            m_vecVertex[verIndex].x = x * m_nWidth + dx;
            m_vecVertex[verIndex].y = 0.0f;
            m_vecVertex[verIndex].z = -y * m_nHeight + dy;
            m_vecVertex[verIndex].nx = 0.0f;
            m_vecVertex[verIndex].ny = 1.0f;
            m_vecVertex[verIndex].nz = 0.0f;
            m_vecVertex[verIndex].u = float(y%2);
            m_vecVertex[verIndex].v = float(x%2);
        }
    }

    //创建索引数据
    verIndex=0;
    for(int y=0; y<m_nRows; ++y)//最右边的点不遍历
    {
        for(int x=0;x<m_nCols; ++x)
        {
            m_vecIndex[verIndex] = y*(m_nCols+1)+x;
            m_vecIndex[verIndex+1] = m_vecIndex[verIndex]+1;
            m_vecIndex[verIndex+2] = (y+1)*(m_nCols+1)+x;

            m_vecIndex[verIndex+3] = m_vecIndex[verIndex]+1;
            m_vecIndex[verIndex+4] = m_vecIndex[verIndex+2]+1;
            m_vecIndex[verIndex+5] = m_vecIndex[verIndex+2];
            verIndex+=6;
        }
    }

	void* pp = NULL;
	//锁定顶点缓冲区
	if(FAILED(m_pVertexBuffer->Lock( 0,
        sizeof(SquareVertex)*m_nVertexCount,
        (void**)&pp, 0)))
    {
        return false;
    }
	memcpy(pp,&m_vecVertex[0],sizeof(SquareVertex)*m_nVertexCount);
	m_pVertexBuffer->Unlock();

	pp = NULL;
	//锁定索引缓冲区
	if(FAILED(m_pIndexBuffer->Lock(0,
        m_nIndexCount*sizeof(WORD),
        (void**)&pp,0)))
    {
        return false;
    }
	memcpy(pp,&m_vecIndex[0],m_nIndexCount*sizeof(WORD));
	m_pIndexBuffer->Unlock();

	return true;
}

/** 获得边界*/
CRect CSquare::getBounds(void)
{
    CRect rc;
    rc.left = - m_nCols * m_nWidth / 2;
    rc.top = -m_nRows * m_nWidth / 2;
    rc.right = - rc.left;
    rc.bottom = -rc.top;

    return rc;
}

//渲染图形
void CSquare::render(LPDIRECT3DDEVICE9 pDevice)
{
	//设置顶点缓冲区
	pDevice->SetStreamSource(0,m_pVertexBuffer,0,sizeof(SquareVertex));
	pDevice->SetIndices(m_pIndexBuffer);
	pDevice->SetFVF(D3DFVF_SQUAREVERTEX);//设置灵活顶点格式
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,
        m_nVertexCount,0,m_nRows*m_nCols*2);
}
////////////////////class CSquare end///////////////////