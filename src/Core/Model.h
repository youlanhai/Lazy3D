#pragma once

#include "RenderObj.h"

typedef RefPtr<class iModel>  ModelPtr;

///模型基类
class LZDLL_API iModel : public IBase, public IRenderObj
{
public:
    ///构造函数
    iModel(void);

    ///析构函数
    ~iModel();

    ///加载模型
    virtual bool load(const std::wstring & name){ m_source = name; return true; }

    ///是否有骨骼
    virtual bool isSkeleton(void) const { return false; }

    ///是否是简单的mesh
    virtual bool isMesh(void) const { return false; }

    virtual void update(float) {}
    virtual void render(IDirect3DDevice9 * pDevice);

    ///获取d3d mesh。
    virtual dx::Mesh * getMesh(){ return NULL; }

public:
    ///获取模型资源名
    const std::wstring & source() const { return m_source; }

    ///获取最终的变换矩阵
    void getCombinedMatrix(Matrix4x4 & mat) const;

    ///获得世界矩阵
    const Matrix4x4 & getWorldMatrix( void) const { return m_matWorld; }

    ///设置世界矩阵
    void setWorldMatrix(const Matrix4x4 & pMat){ m_matWorld = pMat; }

    ///设置缩放
    void setScale(float s) { m_scale.set(s, s, s); }
    void setScale(const Vector3 & s) { m_scale = s; }
    const Vector3 & getScale() const { return m_scale; }

    void setYaw(float a){ m_yaw = a; }
    float getYaw(void) const { return m_yaw; }

    void setPitch(float a){ m_pitch = a; }
    float getPitch(void) const { return m_pitch; }

    void setRoll(float a){ m_roll = a; }
    float getRoll(void) const { return m_roll; }

//包围盒相关
public:

    ///显示包围盒
    void showBound( bool bShow ) { m_bShowBound = bShow; }
    bool isBoundVisib() const { return m_bShowBound; }

    const AABB & getAABB() const { return m_aabb; }
    void setAABB(const AABB & aabb){ m_aabb = aabb; }

    ///获得局部变换后的aabb。没有经过坐标变换，即没有乘以m_matWorld
    void getTransformdAABB(AABB & aabb) const;

public://动画相关

    /** 播放动画*/
    virtual bool playAction(const std::wstring & name, bool loop);

    /** 停止动画*/
    virtual void stopAction(void);

    /** 获得动画个数*/
    virtual int getActionCount(void) const;

    /** 获得动画名称*/
    virtual void getActionName(std::wstring & name, int index) const;

    ///设置动画播放速度
    virtual void setAnimSpeed(float speed);

protected:
    void renderBoundingBox(IDirect3DDevice9 * pDevice);

    std::wstring  m_source;      ///< 模型资源名
    Matrix4x4  m_matWorld;     ///< 世界矩阵
    bool        m_bShowBound;   ///< 是否显示包围盒
    float       m_yaw;          ///< 转角
    float       m_pitch;
    float       m_roll;
    Vector3 m_scale;        ///< 缩放系数
    AABB m_aabb;
};


class LZDLL_API MeshHolder
{
public:
    MeshHolder() : m_mesh(NULL) {}
    ~MeshHolder() { SafeRelease(m_mesh); }

    void setMesh(LPD3DXMESH mesh)
    {
        SafeRelease(m_mesh);
        m_mesh = mesh;
    }

    LPD3DXMESH getMesh(){ return m_mesh; }

    bool valid(){ return m_mesh != NULL; }

private:
    LPD3DXMESH m_mesh;
};
