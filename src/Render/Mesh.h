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

        //当前骨骼的世界变换矩阵
        Matrix      WorldTransformationMatrix;
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
        virtual int getType(void) const override { return rt::mesh; }

        void render();

        int getNbDrawTrangle() const { return m_dwTrangleCnt; }

        const AABB & getAABB() const { return m_aabb; }

        /** 克隆动画控制器。对于所有的Model而言，mesh是公用的（包括骨骼和mesh容器），
        *   但动画的播放进度是不共享，所以每个Model都需要自己克隆一份动画控制器。
        */
        dx::AnimController* cloneAnimaCtrl(void);

        dx::AnimController* getAnimationContrl() const { return m_pAnimController; }

        static bool initShaders();
        static void selectTechnique(const char * name);

    private:
        BoneFrame*              m_bone;		///< 根骨节点
        int		                m_dwTrangleCnt; ///< 渲染的三角形数量
        AABB                    m_aabb;         ///<aabb包围盒
        dx::AnimController*     m_pAnimController;  ///< 动画控制器

        /** 将所有的MeshContainer集中到一起渲染，避免了每次渲染时，
        *   需要遍历骨骼框架带来的多余开销。
        */
        std::vector<MeshContainer*> m_subMeshes;

        //渲染蒙皮网格的着色器
        static EffectPtr s_skinnedEffect;
        //渲染无蒙皮网格的着色器
        static EffectPtr s_noskinnedEffect;


        friend class CAllocateHierarchy;


        /** 生成蒙皮网格*/
        HRESULT generateSkinnedMesh(MeshContainer *pMeshContainer);

        void drawMeshContainer(MeshContainer *pMeshContainer);
        void drawMeshOnly(MeshContainer *pMeshContainer);

        HRESULT bindBone(BoneFrame *pFrame);
        HRESULT bindBoneToMesh(BoneFrame *pOwner, MeshContainer *pMeshContainer);
    };


    typedef RefPtr<Mesh> MeshPtr;

    class MeshMgr : public ResMgr<MeshMgr, MeshPtr>
    {
    public:
        virtual int getType() { return rt::mesh; }
    };
}
