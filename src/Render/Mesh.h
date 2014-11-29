#pragma once
/** 该文件为蒙皮骨骼动画核心实现*/

#include "ResMgr.h"
#include "Effect.h"

namespace Lazy
{
    class MeshContainer;

    ///骨骼框架
    class BoneFrame : public D3DXFRAME
    {
    public:
        BoneFrame(const char *name);
        ~BoneFrame();

        BoneFrame*  find(const char * name);
        void        updateMatrix(const Matrix & matParent);

        Matrix      CombinedTransformationMatrix;
    };

    ///蒙皮骨骼动画
    class Mesh : public IResource
    {
    public:
        Mesh(const tstring & source);
        virtual ~Mesh();

        /** 加载资源。*/
        virtual bool load()  override;

        /** 获得资源类型*/
        virtual int getType(void) const override { return rt::skinmesh; }

        /** 渲染*/
        void render();

        int getNbDrawTrangle() const { return m_dwTrangleCnt; }

        const AABB & getAABB() const { return m_aabb; }

        /** 克隆动画控制器。*/
        dx::AnimController* cloneAnimaCtrl(void);
        dx::AnimController* getAnimationContrl() const { return m_pAnimController; }

    private:
        BoneFrame*      m_bone;		///< 根骨节点
        int		        m_dwTrangleCnt; ///< 渲染的三角形数量
        AABB            m_aabb;         ///<aabb包围盒
        dx::AnimController* m_pAnimController;  ///< 动画控制器

        static EffectPtr s_skinnedEffect;
        static EffectPtr s_noskinnedEffect;

        friend class CAllocateHierarchy;


        /** 生成蒙皮网格*/
        HRESULT generateSkinnedMesh(MeshContainer *pMeshContainer);

        void drawFrame(LPD3DXFRAME pFrame);
        void drawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase);
        void drawMeshOnly(MeshContainer *pMeshContainer, BoneFrame *pFrame);

        HRESULT setupBoneMatrixPointers(LPD3DXFRAME pFrame);
        HRESULT setupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pMeshContainerBase);
    };


    typedef RefPtr<Mesh> MeshPtr;

    class MeshMgr : public ResMgr<MeshMgr, MeshPtr>
    {
    public:
        virtual int getType() { return rt::skinmesh; }
    };
}
