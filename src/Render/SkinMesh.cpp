//SkinMesh.cpp
#include "stdafx.h"
#include "SkinMesh.h"
#include "Texture.h"
#include "RenderDevice.h"
#include "EffectConstant.h"

namespace Lazy
{
    const int MaxNumBone = 32;
    static Matrix g_pBoneMatrices[MaxNumBone];

    //////////////////////////////////////////////////////////////////////
    // MeshContainer
    //////////////////////////////////////////////////////////////////////

    ///骨骼关联的mesh
    struct MeshContainer : public D3DXMESHCONTAINER
    {
        MeshContainer(const char* name);
        ~MeshContainer();

        std::wstring            MeshName;
        LPD3DXATTRIBUTERANGE	pAttributeTable;
        LPDIRECT3DTEXTURE9 *    ppTextures;		//纹理数组
        LPD3DXMESH				pOrigMesh;		//原始网格模型
        DWORD					NumInfl;		//每个顶点最多受多少骨骼的影响
        DWORD					NumAttributeGroups;		//属性组数量,即子网格的数量
        LPD3DXBUFFER			pBoneCombinationBuf;	//骨骼组合表
        Matrix**			    ppBoneMatrixPtrs;		//存放骨骼的组合变换矩阵
        Matrix*				    pBoneOffsetMatrices;	//存放骨骼的初始变换矩阵
        DWORD					NumPaletteEntries;		//骨骼数量上限
        BOOL					UseSoftwareVP;			//是否使用软件顶点处理
        DWORD					iAttributeSW;
    };

    MeshContainer::MeshContainer(const char * name)
    {
        Name = nullptr;
        if (name)
        {
            Lazy::charToWChar(MeshName, name);

            int n = strlen(name) + 1;
            Name = new char[n];
            strcpy_s(Name, n, name);

            ZeroMemory(&MeshData, sizeof(MeshData));
        }

        NumMaterials = 0;
        pMaterials = nullptr; //材质数组
        ppTextures = nullptr; //纹理数组
        pEffects = nullptr;
        pAdjacency = nullptr;
        pSkinInfo = nullptr;

        pAttributeTable = nullptr;
        pOrigMesh = nullptr;		//原始网格模型
        NumInfl = 0;		//每个顶点最多受多少骨骼的影响
        NumAttributeGroups = 0;		//属性组数量,即子网格的数量
        pBoneCombinationBuf = nullptr;	//骨骼组合表
        ppBoneMatrixPtrs = nullptr;		//存放骨骼的组合变换矩阵
        pBoneOffsetMatrices = nullptr;	//存放骨骼的初始变换矩阵
        NumPaletteEntries = 0;		//骨骼数量上限
        UseSoftwareVP = TRUE;			//是否使用软件顶点处理
        iAttributeSW = 0;
    }

    MeshContainer::~MeshContainer()
    {
        SafeDeleteArray(Name);
        SafeDeleteArray(pAdjacency);
        SafeDeleteArray(pMaterials);
        SafeDeleteArray(pBoneOffsetMatrices);

        SafeDeleteArray(ppTextures);//纹理不需要释放，有纹理管理器管理。
        SafeDeleteArray(ppBoneMatrixPtrs);
        SafeRelease(pBoneCombinationBuf);
        SafeRelease(MeshData.pMesh);
        SafeRelease(pSkinInfo);
        SafeRelease(pOrigMesh);

        if (pNextMeshContainer)
        {
            delete (MeshContainer*) pNextMeshContainer;
            pNextMeshContainer = nullptr;
        }
    }

    //////////////////////////////////////////////////////////////////////
    // BoneFrame
    //////////////////////////////////////////////////////////////////////

    BoneFrame::BoneFrame(const char* name)
    {
        Name = nullptr;
        if (name)
        {
            Lazy::charToWChar(BoneName, name);

            int n = strlen(name) + 1;
            Name = new char[n];
            strcpy_s(Name, n, name);
        }

        pMeshContainer = nullptr;
        pFrameSibling = nullptr;
        pFrameFirstChild = nullptr;


        D3DXMatrixIdentity(&TransformationMatrix);
        D3DXMatrixIdentity(&CombinedTransformationMatrix);
    }

    BoneFrame::~BoneFrame()
    {
        SafeDeleteArray(Name);

        if (pMeshContainer)
        {
            delete (MeshContainer*) pMeshContainer;
            pMeshContainer = nullptr;
        }

        if (pFrameSibling)
        {
            delete (BoneFrame*) pFrameSibling;
            pFrameSibling = nullptr;
        }

        if (pFrameFirstChild)
        {
            delete (BoneFrame*) pFrameFirstChild;
            pFrameFirstChild = nullptr;
        }

    }

    BoneFrame* BoneFrame::find(const std::wstring & name)
    {
        if (name == BoneName) return this;

        BoneFrame * pNode = nullptr;
        if (pFrameSibling)
            pNode = ((BoneFrame*) pFrameSibling)->find(name);

        if (pNode) return pNode;

        if (pFrameFirstChild)
            pNode = ((BoneFrame*) pFrameFirstChild)->find(name);

        return pNode;
    }

    void BoneFrame::updateMatrix(const Matrix & matParent)
    {
        D3DXMatrixMultiply(&CombinedTransformationMatrix, &TransformationMatrix, &matParent);

        if (pFrameSibling != NULL)
        {
            ((BoneFrame*) pFrameSibling)->updateMatrix(matParent);
        }

        if (pFrameFirstChild != NULL)
        {
            ((BoneFrame*) pFrameFirstChild)->updateMatrix(static_cast<Matrix&>(CombinedTransformationMatrix));
        }
    }

    //////////////////////////////////////////////////////////////////////
    // CAllocateHierarchy
    //////////////////////////////////////////////////////////////////////

    class CAllocateHierarchy : public ID3DXAllocateHierarchy
    {
    public:
        CAllocateHierarchy(SkinMesh *pSkinMesh)
            : m_pSkinMesh(pSkinMesh)
        {}

        STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame)
        {
            *ppNewFrame = new BoneFrame(Name);
            return S_OK;
        }

        STDMETHOD(CreateMeshContainer)(
            THIS_ LPCSTR              Name,
            CONST D3DXMESHDATA*       pMeshData,
            CONST D3DXMATERIAL*       pMaterials,
            CONST D3DXEFFECTINSTANCE* pEffectInstances,
            DWORD                     NumMaterials,
            CONST DWORD *             pAdjacency,
            LPD3DXSKININFO			pSkinInfo,
            LPD3DXMESHCONTAINER		*ppNewMeshContainer);

        STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree)
        {
            assert(0 && "can't be called!");
            return S_OK;
        }

        STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase)
        {
            assert(0 && "can't be called!");
            return S_OK;
        }

    public:
        SkinMesh*		m_pSkinMesh;
    };

    LRESULT CAllocateHierarchy::CreateMeshContainer(
        THIS_ LPCSTR              Name,
        CONST D3DXMESHDATA*       pMeshData,
        CONST D3DXMATERIAL*       pMaterials,
        CONST D3DXEFFECTINSTANCE* pEffectInstances,
        DWORD                     NumMaterials,
        CONST DWORD *             pAdjacency,
        LPD3DXSKININFO			pSkinInfo,
        LPD3DXMESHCONTAINER		*ppNewMeshContainer)

    {
        HRESULT hr = S_OK;
        *ppNewMeshContainer = NULL;

        if (pMeshData->Type != D3DXMESHTYPE_MESH)
        {
            return E_FAIL;
        }

        LPD3DXMESH pMesh = pMeshData->pMesh;
        if (pMesh->GetFVF() == 0)
        {
            return E_FAIL;
        }

        //为网格容器分配内存
        MeshContainer *pMeshContainer = new MeshContainer(Name);

        pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

        //复制mesh数据
        if (!(pMesh->GetFVF() & D3DFVF_NORMAL))//没有法线
        {
            LPD3DXMESH pClone = NULL;
            LPDIRECT3DDEVICE9 pd3dDevice = NULL;
            pMesh->GetDevice(&pd3dDevice);

            hr = pMesh->CloneMeshFVF(
                pMesh->GetOptions(),
                pMesh->GetFVF() | D3DFVF_NORMAL,
                pd3dDevice,
                &pClone);

            SafeRelease(pd3dDevice);

            if (FAILED(hr))
            {
                delete pMeshContainer;
                return hr;
            }

            D3DXComputeNormals(pClone, NULL);
            pMeshContainer->MeshData.pMesh = pClone;
            pMesh = pClone;
        }
        else
        {
            pMeshContainer->MeshData.pMesh = pMesh;
            pMeshContainer->MeshData.pMesh->AddRef();
        }


        DWORD NumFaces = pMesh->GetNumFaces();
        pMeshContainer->NumMaterials = max<DWORD>(1, NumMaterials);

        pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
        pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
        pMeshContainer->pAdjacency = new DWORD[NumFaces * 3];

        //复制邻接信息
        memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces * 3);

        //复制材质，创建纹理。
        if (NumMaterials > 0)
        {
            memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials);
        }
        else
        {
            ZeroMemory(&pMeshContainer->pMaterials[0].MatD3D, sizeof(D3DXMATERIAL));
            D3DCOLORVALUE color = { 0.5f, 0.5f, 0.5f, 1.0f };
            pMeshContainer->pMaterials[0].MatD3D.Diffuse = color;
        }

        for (DWORD i = 0; i < pMeshContainer->NumMaterials; ++i)
        {
            //复制材质
            LPD3DXMATERIAL destMaterial = pMeshContainer->pMaterials + i;
            destMaterial->MatD3D.Ambient = destMaterial->MatD3D.Diffuse;
            destMaterial->MatD3D.Specular = destMaterial->MatD3D.Diffuse;
            destMaterial->MatD3D.Power = 1.0f;

            //提取纹理
            pMeshContainer->ppTextures[i] = NULL;
            if (destMaterial->pTextureFilename != NULL)
            {
                std::wstring name;
                Lazy::charToWChar(name, destMaterial->pTextureFilename);
                Lazy::removeFilePath(name);
                if (Lazy::getfs()->searchFile(name, name, L""))
                {
                    TexturePtr ptr = TextureMgr::instance()->get(name);
                    if (ptr) pMeshContainer->ppTextures[i] = ptr->getTexture();
                }
                else
                {
                    LOG_ERROR(L"Texture '%s' was not found!", name.c_str());
                }
            }
        }

        if (pSkinInfo != NULL)
        {
            //复制蒙皮信息
            pMeshContainer->pSkinInfo = pSkinInfo;
            pMeshContainer->pSkinInfo->AddRef();

            //复制网格信息
            pMeshContainer->pOrigMesh = pMesh;
            pMeshContainer->pOrigMesh->AddRef();

            //复制骨骼信息
            DWORD nBones = pSkinInfo->GetNumBones();
            pMeshContainer->pBoneOffsetMatrices = new Matrix[nBones];
            for (DWORD i = 0; i < nBones; ++i)
            {
                pMeshContainer->pBoneOffsetMatrices[i] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i));
            }

            //生成蒙皮网格模型
            hr = m_pSkinMesh->generateSkinnedMesh(pMeshContainer);

            if (FAILED(hr))
            {
                delete pMeshContainer;
                return hr;
            }
        }

        *ppNewMeshContainer = pMeshContainer;
        return hr;
    }

    //////////////////////////////////////////////////////////////////////
    // SkinMesh
    //////////////////////////////////////////////////////////////////////
    /*static*/ EffectPtr SkinMesh::s_skinnedEffect;
    /*static*/ EffectPtr SkinMesh::s_noskinnedEffect;

    SkinMesh::SkinMesh(const tstring & source)
        : IResource(source)
        , m_pAnimController(nullptr)
        , m_bone(nullptr)
        , m_dwTrangleCnt(0)
    {
        static bool firstTime = true;
        if (firstTime)
        {
            firstTime = false;
            s_skinnedEffect = EffectMgr::instance()->get(_T("shader/skinned_mesh.fx"));
            s_noskinnedEffect = EffectMgr::instance()->get(_T("shader/noskinned.fx"));
        }

        m_aabb.min.set(-1.0f, 0, -1.0f);
        m_aabb.max.set(1.0f, 1.0f, 1.0f);
    }

    SkinMesh::~SkinMesh()
    {
        if (m_bone)
            delete m_bone;

        SAFE_RELEASE(m_pAnimController);
    }

    /** 加载资源。*/
    bool SkinMesh::load()
    {
        assert(m_bone == nullptr && "Loaded more than once!");

        tstring realPath;
        if (!getfs()->getRealPath(realPath, m_source))
        {
            LOG_ERROR(L"SkinMesh '%s' was not found!", m_source.c_str());
            return false;
        }

        CAllocateHierarchy Alloc(this);
        HRESULT hr = D3DXLoadMeshHierarchyFromX(
            realPath.c_str(),
            D3DXMESH_MANAGED,
            Lazy::rcDevice()->getDevice(),
            &Alloc,
            NULL,
            (LPD3DXFRAME*) &m_bone,
            &m_pAnimController);

        if (FAILED(hr))
        {
            LOG_ERROR(L"Load SkinMesh '%s' failed! hr=0x%x, code=0x%x",
                realPath.c_str(), hr, GetLastError());
            return false;
        }

        if (FAILED(setupBoneMatrixPointers(m_bone)))
        {
            LOG_ERROR(L"Load SkinMesh '%s' failed! hr=0x%x, code=0x%x",
                realPath.c_str(), hr, GetLastError());
            return false;
        }

        //更新包围盒
        m_bone->updateMatrix(matIdentity);

        Vector3 bbCenter;
        float bbRadius;
        if (FAILED(D3DXFrameCalculateBoundingSphere(m_bone, &bbCenter, &bbRadius)))
        {
            bbCenter.set(0.0f, 0.0f, 0.0f);
            bbRadius = 1.0f;
        }

        float r = sin(D3DX_PI / 4.0f) * bbRadius;
        Vector3 extend(r, r, r);
        m_aabb.min = bbCenter - extend;
        m_aabb.max = bbCenter + extend;
        return true;
    }

    void SkinMesh::render()
    {
        if (!m_bone) return;

        dx::Device* pDevice = rcDevice()->getDevice();

        m_dwTrangleCnt = 0;

        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
        pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

        m_bone->updateMatrix(rcDevice()->getWorld());
        drawFrame(m_bone);

        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    }

    /** 克隆动画控制器。*/
    dx::AnimController* SkinMesh::cloneAnimaCtrl(void)
    {
        if (nullptr == m_pAnimController)
            return nullptr;

        /*  克隆原始AC。克隆对象用来操作这个mesh，原始AC除了用来克隆外不做其余用途。*/
        dx::AnimController* pControl = NULL;
        if (FAILED(m_pAnimController->CloneAnimationController(
            m_pAnimController->GetMaxNumAnimationOutputs(),
            m_pAnimController->GetMaxNumAnimationSets(),
            m_pAnimController->GetMaxNumTracks(),
            m_pAnimController->GetMaxNumEvents(),
            &pControl)))
        {
            return NULL;
        }
        return pControl;
    }


    HRESULT SkinMesh::generateSkinnedMesh(MeshContainer *pMeshContainer)
    {
        assert(pMeshContainer);

        if (pMeshContainer->pSkinInfo == NULL) 
            return S_OK;

        SafeRelease(pMeshContainer->MeshData.pMesh);
        SafeRelease(pMeshContainer->pBoneCombinationBuf);
    
        const D3DCAPS9 * pCaps = Lazy::rcDevice()->getCaps();

        DWORD NumMaxFaceInfl;
        DWORD Flags = D3DXMESHOPT_VERTEXCACHE;
        //获取网格模型的索引缓冲区
        LPDIRECT3DINDEXBUFFER9 pIB;
        HRESULT hr = pMeshContainer->pOrigMesh->GetIndexBuffer(&pIB);
        if (FAILED(hr))
            return hr;

        //影响一个面的矩阵不会超过12个，自己画就晓得了
        hr = pMeshContainer->pSkinInfo->GetMaxFaceInfluences(
            pIB,
            pMeshContainer->pOrigMesh->GetNumFaces(),
            &NumMaxFaceInfl);
        pIB->Release();
        if (FAILED(hr))
            return hr;

        NumMaxFaceInfl = min<DWORD>(NumMaxFaceInfl, 12);
        if (pCaps->MaxVertexBlendMatrixIndex + 1 < NumMaxFaceInfl)
        {
            pMeshContainer->NumPaletteEntries = min<DWORD>(
                256, 
                pMeshContainer->pSkinInfo->GetNumBones());

            pMeshContainer->UseSoftwareVP = TRUE;
            Flags |= D3DXMESH_SYSTEMMEM;
        }
        else
        {
            pMeshContainer->NumPaletteEntries = min2(
                (pCaps->MaxVertexBlendMatrixIndex + 1) / 2,
                pMeshContainer->pSkinInfo->GetNumBones());

            pMeshContainer->UseSoftwareVP = FALSE;
            Flags |= D3DXMESH_MANAGED;
        }

        //生成蒙皮网格模型
        hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh(
            pMeshContainer->pOrigMesh,
            Flags,
            pMeshContainer->NumPaletteEntries,
            pMeshContainer->pAdjacency,
            NULL,
            NULL,
            NULL,
            &pMeshContainer->NumInfl,
            &pMeshContainer->NumAttributeGroups,
            &pMeshContainer->pBoneCombinationBuf,
            &pMeshContainer->MeshData.pMesh);

        return hr;
    }


    //desc	调用SetupBoneMatrixPointersOnMesh递归为各框架安置组合变换矩阵
    HRESULT SkinMesh::setupBoneMatrixPointers(LPD3DXFRAME pFrame)
    {
        assert(pFrame);

        HRESULT hr;
        if (pFrame->pMeshContainer != NULL)
        {
            hr = setupBoneMatrixPointersOnMesh(pFrame->pMeshContainer);
            if (FAILED(hr)) return hr;
        }

        //如果存在兄弟骨骼
        if (pFrame->pFrameSibling != NULL)
        {
            hr = setupBoneMatrixPointers(pFrame->pFrameSibling);
            if (FAILED(hr)) return hr;
        }

        //如果存在子骨骼
        if (pFrame->pFrameFirstChild != NULL)
        {
            hr = setupBoneMatrixPointers(pFrame->pFrameFirstChild);
            if (FAILED(hr)) return hr;
        }

        return S_OK;
    }

    //-----------------------------------------------------------
    //desc: 设置每个骨骼的组合变换矩阵
    HRESULT SkinMesh::setupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pMeshContainerBase)
    {
        //先转换成新的类
        MeshContainer *pMeshContainer = (MeshContainer*) pMeshContainerBase;
        LPD3DXSKININFO pSkinInfo = pMeshContainer->pSkinInfo;

        //只有蒙皮网格模型才有骨骼矩阵
        if (NULL == pSkinInfo) return S_OK;

        //得到骨骼数量
        DWORD nBones = pSkinInfo->GetNumBones();
        pMeshContainer->ppBoneMatrixPtrs = new Matrix*[nBones];

        std::wstring tempName;
        for (DWORD i = 0; i < nBones; ++i)
        {
            Lazy::charToWChar(tempName, pSkinInfo->GetBoneName(i));

            BoneFrame *pFrame = m_bone->find(tempName);
            if (pFrame != NULL)
            {
                pMeshContainer->ppBoneMatrixPtrs[i] = &pFrame->CombinedTransformationMatrix;
            }
            else
            {
                pMeshContainer->ppBoneMatrixPtrs[i] = NULL;
            }
        }
        return S_OK;
    }



    //-----------------------------------------------------
    // desc: 绘制框架: 还是和安置矩阵一样递归渲染各框架
    void SkinMesh::drawFrame(LPD3DXFRAME pFrame)
    {
        LPD3DXMESHCONTAINER pMeshContainer;

        pMeshContainer = pFrame->pMeshContainer;
        while (pMeshContainer != NULL)
        {
            drawMeshContainer(pMeshContainer, pFrame);
            pMeshContainer = pMeshContainer->pNextMeshContainer;
        }

        if (pFrame->pFrameSibling != NULL)
        {
            drawFrame(pFrame->pFrameSibling);
        }

        if (pFrame->pFrameFirstChild != NULL)
        {
            drawFrame(pFrame->pFrameFirstChild);
        }
    }

    void SkinMesh::drawMeshOnly(MeshContainer *pMeshContainer, BoneFrame *pFrame)
    {
        LPDIRECT3DDEVICE9 pDevice = Lazy::rcDevice()->getDevice();
        EffectPtr effect = s_noskinnedEffect;
        
        rcDevice()->pushWorld(pFrame->CombinedTransformationMatrix);

        for (DWORD AttribID = 0; AttribID < pMeshContainer->NumMaterials; ++AttribID)
        {
            EffectConstant *pTexture = effect->getConstant("g_texture");
            if (!pTexture || NULL == pMeshContainer->ppTextures[AttribID])
                continue;

            pTexture->bindValue(pMeshContainer->ppTextures[AttribID]);

            EffectConstant *pMaterialDiffuse = effect->getConstant("MaterialDiffuse");
            if (pMaterialDiffuse)
                pMaterialDiffuse->bindValue(pMeshContainer->pMaterials[AttribID].MatD3D.Diffuse);

            EffectConstant *pMaterialAmbient = effect->getConstant("MaterialAmbient");
            if (pMaterialAmbient)
            {
                // Sum of all ambient and emissive contribution
                D3DXCOLOR color1(pMeshContainer->pMaterials[AttribID].MatD3D.Ambient);
                color1 += D3DXCOLOR(pMeshContainer->pMaterials[AttribID].MatD3D.Emissive);

                pMaterialAmbient->bindValue(color1);
            }

            UINT numPasses;
            if (effect->begin(numPasses))
            {
                for (UINT iPass = 0; iPass < numPasses; iPass++)
                {
                    effect->beginPass(iPass);
                    pMeshContainer->MeshData.pMesh->DrawSubset(AttribID);
                    effect->endPass();
                }
                effect->end();
            }
        }

        rcDevice()->popWorld();
        m_dwTrangleCnt += pMeshContainer->MeshData.pMesh->GetNumFaces();
    }

    void SkinMesh::drawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase)
    {
        MeshContainer *pMeshContainer = (MeshContainer*) pMeshContainerBase;
        BoneFrame *pFrame = (BoneFrame*) pFrameBase;

        if (pMeshContainer->pSkinInfo == NULL)
        {
            drawMeshOnly(pMeshContainer, pFrame);
            return;
        }
     
        if (pMeshContainer->NumPaletteEntries > MaxNumBone)
            return;

        EffectPtr effect = s_skinnedEffect;
        EffectConstant *pConst = effect->getConstant("mWorldMatrixArray");
        if (!pConst)
            return;

        EffectConstant *pMaterialDiffuse = effect->getConstant("MaterialDiffuse");
        EffectConstant *pMaterialAmbient = effect->getConstant("MaterialAmbient");
        EffectConstant *pCurNumBones = effect->getConstant("CurNumBones");
        EffectConstant *pTexture = effect->getConstant("g_texture");

        LPDIRECT3DDEVICE9 pd3dDevice = Lazy::rcDevice()->getDevice();
        if (pMeshContainer->UseSoftwareVP)
        {
            pd3dDevice->SetSoftwareVertexProcessing(TRUE);
        }

        LPD3DXBONECOMBINATION pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(
            pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
        for (DWORD iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
        {
            DWORD AttribID = pBoneComb[iAttrib].AttribId;
            if (!pTexture || NULL == pMeshContainer->ppTextures[AttribID])
                continue;

            // first calculate all the world matrices
            for (DWORD iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
            {
                DWORD iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
                if (iMatrixIndex != UINT_MAX)
                {
                    D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry],
                        &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
                        pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
                }
            }

            pConst->bindValue(g_pBoneMatrices, pMeshContainer->NumPaletteEntries);
            
            // set material color properties 
            if (pMaterialDiffuse)
                pMaterialDiffuse->bindValue(pMeshContainer->pMaterials[AttribID].MatD3D.Diffuse);

            if (pMaterialAmbient)
            {
                // Sum of all ambient and emissive contribution
                D3DXCOLOR color1(pMeshContainer->pMaterials[AttribID].MatD3D.Ambient);
                color1 += D3DXCOLOR(pMeshContainer->pMaterials[AttribID].MatD3D.Emissive);
                pMaterialAmbient->bindValue(color1);
            }

            // setup the material of the mesh subset - REMEMBER to use the original pre-skinning attribute id to get the correct material id
            pTexture->bindValue(pMeshContainer->ppTextures[AttribID]);

            // Set CurNumBones to select the correct vertex shader for the number of bones
            if (pCurNumBones)
                pCurNumBones->bindValue(int(pMeshContainer->NumInfl) - 1);

            // Start the effect now all parameters have been updated
            UINT numPasses;
            if (effect->begin(numPasses))
            {
                for (UINT iPass = 0; iPass < numPasses; iPass++)
                {
                    effect->beginPass(iPass);

                    // draw the subset with the current world matrix palette and material state
                    pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);

                    effect->endPass();
                }
                effect->end();
            }
        }

        // remember to reset back to hw vertex processing if software was required
        if (pMeshContainer->UseSoftwareVP)
        {
            pd3dDevice->SetSoftwareVertexProcessing(FALSE);
        }

        m_dwTrangleCnt += pMeshContainer->MeshData.pMesh->GetNumFaces();
    }

}//end namespace Lazy