//CMesh.h
#pragma once

#include "ResMgr.h"

namespace Lazy
{
    ///静态网格
    class StaticMesh : public IResource
    {
    public:
        StaticMesh(const tstring & source);
        ~StaticMesh();

        ///加载资源
        virtual bool load() override;

        ///获得资源类型
        virtual int getType(void) const override { return rt::staticmesh; }

        ///渲染mesh
        void render();

    public:

        dx::Mesh * getMesh(void){ return m_pMesh; }

        const Math::AABB & getAABB(){ return m_aabb; }

    protected:
        std::vector<dx::Texture*>	m_vecTextures;	///< 纹理数组指针
        std::vector<dx::Material>	m_vecMaterials;	///< 材质数组
        dx::Mesh        *m_pMesh;	///< 网格
        Math::AABB   m_aabb;     ///< aabb包围盒
    };

    typedef RefPtr<StaticMesh> StaticMeshPtr;

    class StaticMeshMgr : public ResMgr<StaticMeshMgr, StaticMeshPtr>
    {
    public:
        virtual int getType(){ return rt::staticmesh; }
    };

}//end namespace Lazy