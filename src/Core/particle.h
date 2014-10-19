#pragma once

#include "Base.h"
#include "RenderInterface.h"
#include "CachedList.h"

namespace Lazy
{

    /** 粒子顶点格式*/
    struct ParticleVertex
    {
        float           x, y, z;  //坐标
        D3DCOLOR        color;  //颜色

        static DWORD    FVF ;//灵活顶点格式
        static DWORD    SIZE;

        ///构造函数
        ParticleVertex(float x_, float y_, float z_) : x(x_), y(y_), z(z_) { }
    };

    /**粒子属性*/
    struct Attribute
    {
        D3DXVECTOR3 position;           //< 位置
        D3DXVECTOR3 speed;			    //< 速度
        D3DXVECTOR3 acceleration;	    //< 加速度
        float       lifeTime;			//< 生命周期
        float       age;				//< 当前年龄
        D3DCOLOR    color;				//< 粒子颜色
        D3DCOLOR    colorFade;			//< 消失时的颜色
        bool        live;				//< 存活状态
    };

    /**粒子系统基类*/
    class LZDLL_API ParticleSystem : public IBase, IRenderable
    {
    public:
        ///构造函数
        ParticleSystem(int maxAmount, const std::wstring & texture);

        ///析构函数
        ~ParticleSystem(void);

        ///渲染定点中的粒子
        virtual void render(IDirect3DDevice9 *pDevice);

        virtual void update(float fElapse);

        ///系统中的所有粒子都死亡则返回true,否则返回false;
        bool isDead();

        ///重置所有的粒子属性
        void reset();

    protected:

        ///添加一个粒子
        virtual void addParticle();

        virtual void updateParticles(float elapse);

        virtual void updateParticle(Attribute *attribute, float elapse);

        ///重置一个粒子的属性
        virtual void resetParticle(Attribute *attribute);

        ///设置渲染状态
        virtual void setRenderState(IDirect3DDevice9 *pDevice);

        ///恢复渲染状态
        virtual void recoverRenderState(IDirect3DDevice9 *pDevice);

        ///移除已经死亡的粒子
        void removeDeadParticle();

        ///浮点数转换为DWORD类型
        DWORD floatToDWORD(float f) { return *((DWORD*)&f); }

        ///产生指定范围的随机数
        float getRandomFloat(float lowBound, float highBound);

        D3DXVECTOR3 getRandomVector(D3DXVECTOR3* min, D3DXVECTOR3* max);

    public:
        CachedList<Attribute> particles; //< 属性链表

    protected:
        IDirect3DVertexBuffer9 *m_pVertexBuffer;	//顶点缓冲区指针
        std::wstring     m_textureName;
        D3DXVECTOR3     m_beginPos;		//粒子的初始位置
        float           m_createSpeed;			//产生新粒子的速度
        float           m_Size;				//粒子大小
        int             m_maxAmount; //给定时间内系统能存活的最大粒子数
        int             m_amountToRender;
        DWORD           m_vbSize;				//给定时间内系统缓冲区能容纳的粒子数
        DWORD           m_vboffset;			//系统缓冲区偏移量
        DWORD           m_vbBatchSize;		//每次渲染的批量
    };

} // end namespace Lazy
