//Cube.cpp
#include "stdafx.h"
#include "Cube.h"

DWORD CCube::D3DFVF_CUBEVERTEX = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1;

CCube::CCube()
{
	m_pVertexBuffer=NULL;
}

CCube::~CCube()
{
	SafeRelease(m_pVertexBuffer);
}

/*
*初始化立方体数据
*pDevice为D3D设备对象
*x,y,z为立方体外切园的圆心坐标
*h为立方体的高度
*/
bool CCube::init(IDirect3DDevice9 *pDevice,float w,float h,float deep)
{
    float x = 0, y = 0, z = 0;
	float dw=w/2.0f;
	float dh=h/2.0f;
	float dd=deep/2.0f;
	//12*3=36个点。
	CubeVertex pVer[]=
	{//全部采用顺时针方向绘制
		//0
        //0
        x-dw,y+dh,z-dd,0.0f,0.0f,-1.0f,0xffffffff,0.0f,0.0f,
        x+dw,y+dh,z-dd,0.0f,0.0f,-1.0f,0xffffffff,1.0f,0.0f,
        x-dw,y-dh,z-dd,0.0f,0.0f,-1.0f,0xffffffff,0.0f,1.0f,
        //1
        x-dw,y-dh,z-dd,0.0f,0.0f,-1.0f,0xffffffff,0.0f,1.0f,
        x+dw,y+dh,z-dd,0.0f,0.0f,-1.0f,0xffffffff,1.0f,0.0f,
        x+dw,y-dh,z-dd,0.0f,0.0f,-1.0f,0xffffffff,1.0f,1.0f,
        //2
        x+dw,y+dh,z-dd,1.0f,0.0f,0.0f,0xffffffff,0.0f,0.0f,
        x+dw,y+dh,z+dd,1.0f,0.0f,0.0f,0xffffffff,1.0f,0.0f,
        x+dw,y-dh,z-dd,1.0f,0.0f,0.0f,0xffffffff,0.0f,1.0f,
        //3
        x+dw,y-dh,z-dd,1.0f,0.0f,0.0f,0xffffffff,0.0f,1.0f,
        x+dw,y+dh,z+dd,1.0f,0.0f,0.0f,0xffffffff,1.0f,0.0f,
        x+dw,y-dh,z+dd,1.0f,0.0f,0.0f,0xffffffff,1.0f,1.0f,
        //4
        x+dw,y+dh,z+dd,0.0f,0.0f,1.0f,0xffffffff,0.0f,0.0f,
        x-dw,y+dh,z+dd,0.0f,0.0f,1.0f,0xffffffff,1.0f,0.0f,	
        x+dw,y-dh,z+dd,0.0f,0.0f,1.0f,0xffffffff,0.0f,1.0f,
        //5
        x+dw,y-dh,z+dd,0.0f,0.0f,1.0f,0xffffffff,0.0f,1.0f,
        x-dw,y+dh,z+dd,0.0f,0.0f,1.0f,0xffffffff,1.0f,0.0f,
        x-dw,y-dh,z+dd,0.0f,0.0f,1.0f,0xffffffff,1.0f,1.0f,
        //6
        x-dw,y+dh,z+dd,-1.0f,0.0f,0.0f,0xffffffff,0.0f,0.0f,
        x-dw,y+dh,z-dd,-1.0f,0.0f,0.0f,0xffffffff,1.0f,0.0f,
        x-dw,y-dh,z+dd,-1.0f,0.0f,0.0f,0xffffffff,0.0f,1.0f,
        //7
        x-dw,y-dh,z+dd,-1.0f,0.0f,0.0f,0xffffffff,0.0f,1.0f,
        x-dw,y+dh,z-dd,-1.0f,0.0f,0.0f,0xffffffff,1.0f,0.0f,
        x-dw,y-dh,z-dd,-1.0f,0.0f,0.0f,0xffffffff,1.0f,1.0f,
        //8
        x-dw,y+dh,z-dd,0.0f,1.0f,0.0f,0xffffffff,0.0f,0.0f,
        x-dw,y+dh,z+dd,0.0f,1.0f,0.0f,0xffffffff,1.0f,0.0f,
        x+dw,y+dh,z-dd,0.0f,1.0f,0.0f,0xffffffff,0.0f,1.0f,
        //9
        x+dw,y+dh,z-dd,0.0f,1.0f,0.0f,0xffffffff,0.0f,1.0f,
        x-dw,y+dh,z+dd,0.0f,1.0f,0.0f,0xffffffff,1.0f,0.0f,
        x+dw,y+dh,z+dd,0.0f,1.0f,0.0f,0xffffffff,1.0f,1.0f,
        //10
        x-dw,y-dh,z-dd,0.0f,-1.0f,0.0f,0xffffffff,0.0f,0.0f,
        x+dw,y-dh,z-dd,0.0f,-1.0f,0.0f,0xffffffff,1.0f,0.0f,
        x-dw,y-dh,z+dd,0.0f,-1.0f,0.0f,0xffffffff,0.0f,1.0f,
        //11
        x-dw,y-dh,z+dd,0.0f,-1.0f,0.0f,0xffffffff,0.0f,1.0f,
        x+dw,y-dh,z-dd,0.0f,-1.0f,0.0f,0xffffffff,1.0f,0.0f,
        x+dw,y-dh,z+dd,0.0f,-1.0f,0.0f,0xffffffff,1.0f,1.0f,
	};
	memcpy(m_pVertex,pVer,sizeof(pVer));
	//锁定顶点缓冲区
	if(FAILED(pDevice->CreateVertexBuffer(sizeof(pVer),
		0,D3DFVF_CUBEVERTEX,D3DPOOL_MANAGED,&m_pVertexBuffer,NULL)))
    {
        return false;
    }

	updateVertex(pDevice);

	return true;
}
bool CCube::updateVertex(IDirect3DDevice9 *)
{
	void* pp=NULL;
	if(FAILED(m_pVertexBuffer->Lock(0,sizeof(m_pVertex),(void**)&pp,0)))
    {
        return false;
    }
	memcpy(pp,m_pVertex,sizeof(m_pVertex));
	m_pVertexBuffer->Unlock();
	return true;
}

void CCube::render(IDirect3DDevice9 *pDevice)
{
	//设置顶点缓冲区
	pDevice->SetStreamSource(0,m_pVertexBuffer,0,sizeof(CubeVertex));
	pDevice->SetFVF(D3DFVF_CUBEVERTEX);//设置灵活顶点格式
	for(int i=0;i<6;++i)
	{
		pDevice->DrawPrimitive(D3DPT_TRIANGLELIST,i*6,2);//三角形带绘制
	}
}
