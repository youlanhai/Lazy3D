#pragma once
/** 该文件为蒙皮骨骼动画核心实现*/

#include "ResMgr.h"

namespace Lazy
{

    ///骨骼框架
    struct BoneFrame : public D3DXFRAME
    {
        BoneFrame(const char *name);
        ~BoneFrame();

        BoneFrame* find(const std::wstring & name);
        void updateMatrix(const Math::Matrix4x4 & matParent);

        std::wstring    BoneName;
        D3DXMATRIX      CombinedTransformationMatrix;
    };

    ///骨骼关联的mesh
    struct MeshContainer : public D3DXMESHCONTAINER
    {
        MeshContainer(const char* name);
        ~MeshContainer();

        std::wstring            MeshName;
        LPD3DXATTRIBUTERANGE	pAttributeTable;
        LPDIRECT3DTEXTURE9		*ppTextures;		//纹理数组
        LPD3DXMESH				pOrigMesh;		//原始网格模型
        DWORD					NumInfl;		//每个顶点最多受多少骨骼的影响
        DWORD					NumAttributeGroups;		//属性组数量,即子网格的数量
        LPD3DXBUFFER			pBoneCombinationBuf;	//骨骼组合表
        D3DXMATRIX**			ppBoneMatrixPtrs;		//存放骨骼的组合变换矩阵
        D3DXMATRIX*				pBoneOffsetMatrices;	//存放骨骼的初始变换矩阵
        DWORD					NumPaletteEntries;		//骨骼数量上限
        bool					UseSoftwareVP;			//是否使用软件顶点处理
        DWORD					iAttributeSW;
    };

    ///蒙皮处理方法
    namespace SkinMethod
    {
        const int none = 0;
        const int noIndexed = 1;
        const int indexed = 2;
        const int software = 3;
        const int hardware = 4;
    };

    ///蒙皮骨骼动画
    class SkinMesh : public IResource
    {
    public:

        SkinMesh(const tstring & source);

        virtual ~SkinMesh();

        /** 加载资源。*/
        virtual bool load()  override;

        /** 获得资源类型*/
        virtual int getType(void) const override { return rt::skinmesh; }

        /** 渲染*/
        void render();

    public:

        /** 从X文件中加载骨骼动画。*/
        bool loadFromXFile(const std::wstring & fileName);

        const Math::Matrix4x4 & getWorldMatrix(void) const { return m_worldMatrix; }

        void setWorldMatrix(const Math::Matrix4x4 & mat){ m_worldMatrix = mat; }

        int getNbDrawTrangle() const { return m_dwTrangleCnt; }

        float getBBRadius(void) const { return m_bbRadius; }

        const Math::Vector3 & getBBCenter(void) const { return m_bbCenter; }

        const Math::AABB & getAABB() const { return m_aabb; }

        /** 克隆动画控制器。*/
        dx::AnimController* cloneAnimaCtrl(void);

        dx::AnimController* getAnimationContrl() const { return m_pAnimController; }

        /** 生成蒙皮网格*/
        HRESULT generateSkinnedMesh(MeshContainer *pMeshContainer);

    protected:
        BoneFrame	    *m_bone;		///< 根骨节点
        int	            m_skinMethod;   ///< 蒙皮类型
        int		        m_dwTrangleCnt; ///< 渲染的三角形数量
        float           m_bbRadius;     ///< 包围盒半径
        Math::Vector3    m_bbCenter;     ///< 包围盒中心
        Math::AABB       m_aabb;         ///<aabb包围盒
        Math::Matrix4x4  m_worldMatrix;  ///<世界矩阵
        dx::AnimController* m_pAnimController;  ///< 动画控制器

    protected:
        void drawFrame(LPD3DXFRAME pFrame);

        HRESULT setupBoneMatrixPointers(LPD3DXFRAME pFrame);
        HRESULT setupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pMeshContainerBase);

        HRESULT generateSkinnedMeshNoneIndex(MeshContainer *pMeshContainer);
        HRESULT generateSkinnedMeshIndex(MeshContainer *pMeshContainer);
        HRESULT generateSkinnedMeshSoft(MeshContainer *pMeshContainer);

        void drawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase);
        void drawMeshContainerMeshOnly(MeshContainer *pMeshContainerBase, BoneFrame *pFrameBase);
        void drawMeshContainerNoIndex(MeshContainer *pMeshContainerBase, BoneFrame *pFrameBase);
        void drawMeshContainerIndex(MeshContainer *pMeshContainerBase, BoneFrame *pFrameBase);
        void drawMeshContainerSoft(MeshContainer *pMeshContainerBase, BoneFrame *pFrameBase);
    };


    typedef RefPtr<SkinMesh> SkinMeshPtr;

    class SkinMeshMgr : public ResMgr<SkinMeshMgr, SkinMeshPtr>
    {
    public:
        virtual int getType(){ return rt::skinmesh; }
    };
}