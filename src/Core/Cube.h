//Cube.h
#pragma once

namespace Lazy
{

//立方体类
    class LZDLL_API CCube : public IBase
    {
    public:
        struct CubeVertex //自定义顶点格式
        {
            float x, y, z;	//未经过坐标变换后的点
            float nx, ny, nz;	//顶点法向量
            DWORD color;	//顶点颜色
            float u, v;		//纹理坐标
        };
        static DWORD D3DFVF_CUBEVERTEX;
    public:
        CCube();
        ~CCube();
        //初始化
        bool init(IDirect3DDevice9 *pDevice, float w, float h, float deep);
        bool updateVertex(IDirect3DDevice9 *pDevice);
        void render(IDirect3DDevice9 *pDevice);//渲染
    private:
        IDirect3DVertexBuffer9 *m_pVertexBuffer;//顶点缓冲区
        CubeVertex m_pVertex[36];//顶点数据
    };

} // end namespace Lazy
