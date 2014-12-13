#pragma once

#include "SceneNode.h"

namespace Lazy
{

    typedef RefPtr<class IModel>  ModelPtr;

    ///模型基类
    class LZDLL_API IModel : public SceneNode
    {
    public:
        SCENE_NODE_HEADER(IModel);
        
        ///构造函数
        IModel(void);

        ///析构函数
        ~IModel();

        ///加载模型
        virtual bool load(const std::wstring & name) { m_source = name; return true; }

        virtual void render(IDirect3DDevice9 * pDevice);

        ///获取d3d mesh。
        virtual dx::Mesh * getMesh() { return NULL; }

        ///获取模型资源名
        const std::wstring & source() const { return m_source; }

        ///显示包围盒
        void setBBVisible(BOOL visible) { m_bbVisible = visible; }
        BOOL getBBVisible() const { return m_bbVisible; }

        float getHeight() const;

        object createScriptSelf();

    public://动画相关

        /** 播放动画*/
        virtual bool playAction(const std::string & name, bool loop);

        /** 停止动画*/
        virtual void stopAction(void);

        /** 获得动画个数*/
        virtual int getActionCount(void) const;

        /** 获得动画名称*/
        virtual std::string getActionName(int index) const;

        /** 设置动画播放速度*/
        virtual void setActionSpeed(float speed);

    protected:
        void renderBoundingBox(IDirect3DDevice9 * pDevice);

        std::wstring    m_source;      ///< 模型资源名
        BOOL            m_bbVisible;   ///< 是否显示包围盒
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

        LPD3DXMESH getMesh() { return m_mesh; }

        bool valid() { return m_mesh != NULL; }

    private:
        LPD3DXMESH m_mesh;
    };

} // end namespace Lazy
