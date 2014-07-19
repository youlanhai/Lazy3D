//SkinMesh.cpp
#include "stdafx.h"
#include "SkinMesh.h"
#include "Texture.h"
#include "RenderDevice.h"


namespace Lazy
{
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
        UseSoftwareVP = true;			//是否使用软件顶点处理
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


    class CAllocateHierarchy : public ID3DXAllocateHierarchy
    {
    public:
        STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
        STDMETHOD(CreateMeshContainer)(
            THIS_ LPCSTR              Name,
            CONST D3DXMESHDATA*       pMeshData,
            CONST D3DXMATERIAL*       pMaterials,
            CONST D3DXEFFECTINSTANCE* pEffectInstances,
            DWORD                     NumMaterials,
            CONST DWORD *             pAdjacency,
            LPD3DXSKININFO			pSkinInfo,
            LPD3DXMESHCONTAINER		*ppNewMeshContainer);

        STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
        STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);
        CAllocateHierarchy(SkinMesh *pSkinMesh) : m_pSkinMesh(pSkinMesh) {}

    public:
        SkinMesh*		m_pSkinMesh;
    };

    ///////////////////////////////////////////////
    //			CAllocateHierarchy				///
    ///////////////////////////////////////////////

    HRESULT CAllocateHierarchy::CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame)
    {
        *ppNewFrame = new BoneFrame(Name);
        return S_OK;
    }

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
        pMeshContainer->NumMaterials = max(1, NumMaterials);

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

    HRESULT CAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree)
    {
        assert(0 && "can't be called!");
        return S_OK;
    }

    HRESULT CAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
    {
        assert(0 && "can't be called!");
        return S_OK;
    }



    //////////////////////////////////////////////////////////////////////
    // SkinMesh
    //////////////////////////////////////////////////////////////////////
    SkinMesh::SkinMesh(const tstring & source)
        : IResource(source)
    {
        m_pAnimController = NULL;
        m_bone = NULL;
        m_skinMethod = SkinMethod::indexed;

        m_dwTrangleCnt = 0;

        m_aabb.min.set(-1.0f, 0, -1.0f);
        m_aabb.max.set(1.0f, 1.0f, 1.0f);
    }

    SkinMesh::~SkinMesh()
    {
        if (m_bone) delete m_bone;

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
        assert(m_pAnimController && "SkinMesh::cloneAnimaCtrl");

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

    //////底层实现

    HRESULT SkinMesh::generateSkinnedMeshNoneIndex(MeshContainer *pMeshContainer)
    {
        assert(pMeshContainer);

        LPDIRECT3DDEVICE9 pDevice = Lazy::rcDevice()->getDevice();
        const D3DCAPS9 * pCaps = Lazy::rcDevice()->getCaps();

        HRESULT hr = pMeshContainer->pSkinInfo->ConvertToBlendedMesh(
            pMeshContainer->pOrigMesh,
            D3DXMESH_MANAGED | D3DXMESHOPT_VERTEXCACHE,
            pMeshContainer->pAdjacency,
            NULL, NULL, NULL,
            &pMeshContainer->NumInfl,
            &pMeshContainer->NumAttributeGroups,
            &pMeshContainer->pBoneCombinationBuf,
            &pMeshContainer->MeshData.pMesh);

        if (FAILED(hr)) return hr;

        //如果设备只支持2个顶点混和，ConverteToBlendMesh不能近似地转换所有的网格。这时需要把网格转换成两部分，
        //只使用2个矩阵部分和其他部分。前者使用设备的顶点处理，其他部分使用软件方式处理
        LPD3DXBONECOMBINATION rgBoneCombinations = reinterpret_cast<LPD3DXBONECOMBINATION>(
            pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
        for (pMeshContainer->iAttributeSW = 0;
            pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups;
            pMeshContainer->iAttributeSW++)
        {
            DWORD cInfl = 0;
            for (DWORD iInfl = 0; iInfl < pMeshContainer->NumInfl; iInfl++)
            {
                if (rgBoneCombinations[pMeshContainer->iAttributeSW].BoneId[iInfl] != UINT_MAX)
                {
                    ++cInfl;
                }
            }

            if (cInfl > pCaps->MaxVertexBlendMatrices)
                break;
        }

        if (pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups)
        {
            LPD3DXMESH pMeshTmp;
            hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF(
                D3DXMESH_SOFTWAREPROCESSING | pMeshContainer->MeshData.pMesh->GetOptions(),
                pMeshContainer->MeshData.pMesh->GetFVF(),
                pDevice, &pMeshTmp);

            if (FAILED(hr))
                return hr;

            pMeshContainer->MeshData.pMesh->Release();
            pMeshContainer->MeshData.pMesh = pMeshTmp;
            pMeshTmp = NULL;
        }

        return S_OK;
    }

    HRESULT SkinMesh::generateSkinnedMeshIndex(MeshContainer *pMeshContainer)
    {
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

        NumMaxFaceInfl = min2(NumMaxFaceInfl, 12);
        if (pCaps->MaxVertexBlendMatrixIndex + 1 < NumMaxFaceInfl)
        {
            pMeshContainer->NumPaletteEntries = min2(
                256, 
                pMeshContainer->pSkinInfo->GetNumBones());

            pMeshContainer->UseSoftwareVP = true;
            Flags |= D3DXMESH_SYSTEMMEM;
        }
        else
        {
            pMeshContainer->NumPaletteEntries = min2(
                (pCaps->MaxVertexBlendMatrixIndex + 1) / 2,
                pMeshContainer->pSkinInfo->GetNumBones());

            pMeshContainer->UseSoftwareVP = false;
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

    HRESULT SkinMesh::generateSkinnedMeshSoft(MeshContainer *pMeshContainer)
    {
        LPDIRECT3DDEVICE9 pDevice = Lazy::rcDevice()->getDevice();

        HRESULT hr = pMeshContainer->pOrigMesh->CloneMeshFVF(
            D3DXMESH_MANAGED,
            pMeshContainer->pOrigMesh->GetFVF(),
            pDevice,
            &pMeshContainer->MeshData.pMesh);

        if (FAILED(hr)) return hr;

        hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(NULL, &pMeshContainer->NumAttributeGroups);
        if (FAILED(hr)) return hr;

        pMeshContainer->pAttributeTable = new D3DXATTRIBUTERANGE[pMeshContainer->NumAttributeGroups];
        hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(pMeshContainer->pAttributeTable, NULL);
        if (FAILED(hr)) return hr;

        return S_OK;
    }

    HRESULT SkinMesh::generateSkinnedMesh(MeshContainer *pMeshContainer)
    {
        assert(pMeshContainer);

        if (pMeshContainer->pSkinInfo == NULL) return S_OK;

        SafeRelease(pMeshContainer->MeshData.pMesh);
        SafeRelease(pMeshContainer->pBoneCombinationBuf);

        if (m_skinMethod == SkinMethod::noIndexed)
        {
            return generateSkinnedMeshNoneIndex(pMeshContainer);
        }
        else if (m_skinMethod == SkinMethod::indexed)	//索引缓冲区模式的
        {
            return generateSkinnedMeshIndex(pMeshContainer);
        }
        else if (m_skinMethod == SkinMethod::software)
        {
            return generateSkinnedMeshSoft(pMeshContainer);
        }
        else
        {
            LOG_ERROR(L"Invalid skin method %d", m_skinMethod);
            return E_FAIL;
        }
    }

    //------------------------------------------------------------------
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

    void SkinMesh::drawMeshContainerMeshOnly(MeshContainer *pMeshContainer, BoneFrame *pFrame)
    {
        LPDIRECT3DDEVICE9 pDevice = Lazy::rcDevice()->getDevice();

        pDevice->SetTransform(D3DTS_WORLD, &pFrame->CombinedTransformationMatrix);

        for (DWORD i = 0; i < pMeshContainer->NumMaterials; ++i)
        {
            pDevice->SetMaterial(&pMeshContainer->pMaterials[i].MatD3D);
            pDevice->SetTexture(0, pMeshContainer->ppTextures[i]);
            pMeshContainer->MeshData.pMesh->DrawSubset(i);
        }

        m_dwTrangleCnt += pMeshContainer->MeshData.pMesh->GetNumFaces();
    }

    void SkinMesh::drawMeshContainerNoIndex(MeshContainer *pMeshContainer, BoneFrame *)
    {
        LPDIRECT3DDEVICE9 pDevice = Lazy::rcDevice()->getDevice();
        const D3DCAPS9 * pCaps = Lazy::rcDevice()->getCaps();

        DWORD AttribIdPrev = UNUSED32;
        DWORD NumBlend = 0;
        DWORD iMatrixIndex = 0;
        Matrix matTemp;

        LPD3DXBONECOMBINATION pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(
            pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
        for (DWORD iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
        {
            NumBlend = 0;
            for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
            {
                if (pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
                {
                    NumBlend = i;
                }
            }
            if (pCaps->MaxVertexBlendMatrices >= NumBlend + 1)
            {
                for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
                {
                    iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
                    if (iMatrixIndex != UINT_MAX)
                    {
                        D3DXMatrixMultiply(
                            &matTemp,
                            &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
                            pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
                        pDevice->SetTransform(D3DTS_WORLDMATRIX(i), &matTemp);
                    }
                }
                pDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);
                if ((AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32))
                {
                    pDevice->SetMaterial(&pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D);
                    pDevice->SetTexture(0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]);
                    AttribIdPrev = pBoneComb[iAttrib].AttribId;
                }
                pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
            }
        }
        if (pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups)
        {
            AttribIdPrev = UNUSED32;
            pDevice->SetSoftwareVertexProcessing(true);
            for (DWORD iAttrib = pMeshContainer->iAttributeSW;
                iAttrib < pMeshContainer->NumAttributeGroups;
                iAttrib++)
            {
                NumBlend = 0;
                for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
                {
                    if (pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
                    {
                        NumBlend = i;
                    }
                }
                if (pCaps->MaxVertexBlendMatrices < NumBlend + 1)
                {
                    for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
                    {
                        iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
                        if (iMatrixIndex != UINT_MAX)
                        {
                            D3DXMatrixMultiply(
                                &matTemp,
                                &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
                                pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
                            pDevice->SetTransform(D3DTS_WORLDMATRIX(i), &matTemp);
                        }
                    }
                    pDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);
                    if ((AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32))
                    {
                        pDevice->SetMaterial(&pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D);
                        pDevice->SetTexture(0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]);
                        AttribIdPrev = pBoneComb[iAttrib].AttribId;
                    }
                    pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
                }
            }
            pDevice->SetSoftwareVertexProcessing(false);
        }
        pDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);
    }

    void SkinMesh::drawMeshContainerIndex(MeshContainer *pMeshContainer, BoneFrame *)
    {
        LPDIRECT3DDEVICE9 pDevice = Lazy::rcDevice()->getDevice();
        Matrix matTemp;

        if (pMeshContainer->UseSoftwareVP)
        {
            pDevice->SetSoftwareVertexProcessing(true);
        }

        if (pMeshContainer->NumInfl == 1)
            pDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_0WEIGHTS);
        else
            pDevice->SetRenderState(D3DRS_VERTEXBLEND, pMeshContainer->NumInfl - 1);

        if (pMeshContainer->NumInfl)
        {
            pDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);
        }

        LPD3DXBONECOMBINATION pBoneComb = LPD3DXBONECOMBINATION(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
        for (DWORD i = 0; i < pMeshContainer->NumAttributeGroups; ++i)
        {
            for (DWORD iPaletteEntry = 0;
                iPaletteEntry < pMeshContainer->NumPaletteEntries;
                ++iPaletteEntry)
            {
                DWORD iMatrix = pBoneComb[i].BoneId[iPaletteEntry];
                if (iMatrix != UINT_MAX)
                {
                    D3DXMatrixMultiply(&matTemp,
                        &pMeshContainer->pBoneOffsetMatrices[iMatrix],
                        pMeshContainer->ppBoneMatrixPtrs[iMatrix]);
                    pDevice->SetTransform(D3DTS_WORLDMATRIX(iPaletteEntry), &matTemp);
                }
            }

            pDevice->SetMaterial(&pMeshContainer->pMaterials[pBoneComb[i].AttribId].MatD3D);
            pDevice->SetTexture(0, pMeshContainer->ppTextures[pBoneComb[i].AttribId]);
            pMeshContainer->MeshData.pMesh->DrawSubset(i);
        }

        pDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
        pDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);

        if (pMeshContainer->UseSoftwareVP)
        {
            pDevice->SetSoftwareVertexProcessing(false);
        }
    }
    void SkinMesh::drawMeshContainerSoft(MeshContainer *pMeshContainer, BoneFrame *)
    {
        LPDIRECT3DDEVICE9 pDevice = Lazy::rcDevice()->getDevice();

        DWORD nBones = pMeshContainer->pSkinInfo->GetNumBones();

        static std::vector<Matrix> s_matrixCache;
        s_matrixCache.resize(nBones);

        for (DWORD i = 0; i < nBones; ++i)
        {
            D3DXMatrixMultiply(
                &s_matrixCache[i],
                &pMeshContainer->pBoneOffsetMatrices[i],
                pMeshContainer->ppBoneMatrixPtrs[i]
                );
        }

        Matrix  Identity;
        D3DXMatrixIdentity(&Identity);
        pDevice->SetTransform(D3DTS_WORLD, &Identity);

        PBYTE pbVerticesSrc;
        PBYTE pbVerticesDest;

        LPD3DXMESH pSrcMesh = pMeshContainer->pOrigMesh;
        LPD3DXMESH pDstMesh = pMeshContainer->MeshData.pMesh;

        if (FAILED(pSrcMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*) &pbVerticesSrc)))
            return;

        if (FAILED(pDstMesh->LockVertexBuffer(0, (LPVOID*) &pbVerticesDest)))
        {
            pSrcMesh->UnlockVertexBuffer();
            return;
        }

        pMeshContainer->pSkinInfo->UpdateSkinnedMesh(
            &s_matrixCache[0],
            NULL,
            pbVerticesSrc,
            pbVerticesDest);

        pSrcMesh->UnlockVertexBuffer();
        pDstMesh->UnlockVertexBuffer();

        for (DWORD i = 0; i < pMeshContainer->NumAttributeGroups; ++i)
        {
            DWORD attrIdx = pMeshContainer->pAttributeTable[i].AttribId;

            pDevice->SetMaterial(&pMeshContainer->pMaterials[attrIdx].MatD3D);
            pDevice->SetTexture(0, pMeshContainer->ppTextures[attrIdx]);
            pDstMesh->DrawSubset(attrIdx);
        }
    }

    void SkinMesh::drawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase)
    {
        MeshContainer *pMeshContainer = (MeshContainer*) pMeshContainerBase;
        BoneFrame *pFrame = (BoneFrame*) pFrameBase;

        if (pMeshContainer->pSkinInfo == NULL)
        {
            drawMeshContainerMeshOnly(pMeshContainer, pFrame);
        }
        else if (m_skinMethod == SkinMethod::noIndexed)
        {
            drawMeshContainerNoIndex(pMeshContainer, pFrame);
        }
        else if (m_skinMethod == SkinMethod::indexed)
        {
            drawMeshContainerIndex(pMeshContainer, pFrame);
        }
        else if (m_skinMethod == SkinMethod::software)
        {
            drawMeshContainerSoft(pMeshContainer, pFrame);
        }

        m_dwTrangleCnt += pMeshContainer->MeshData.pMesh->GetNumFaces();
    }


}//end namespace Lazy