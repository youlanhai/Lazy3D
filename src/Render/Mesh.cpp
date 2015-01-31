//Mesh.cpp
#include "stdafx.h"
#include "Mesh.h"
#include "Texture.h"
#include "RenderDevice.h"
#include "EffectConstant.h"
#include "ShadowMap.h"

namespace Lazy
{
    //蒙皮mesh在一次draw的过程中，所依赖的最大骨骼数量。
    //这个数量必须与shader中的数量保存一致。
    const int MaxNumSkinBone = 16;
    static Matrix g_bonePalette[MaxNumSkinBone];

    //////////////////////////////////////////////////////////////////////
    // MeshContainer
    //////////////////////////////////////////////////////////////////////

    ///骨骼关联的mesh
    class MeshContainer : public D3DXMESHCONTAINER
    {
    public:
        MeshContainer(const char* name);
        ~MeshContainer();

        LPD3DXATTRIBUTERANGE	pAttributeTable;
        TexturePtr *            ppTextures;		//纹理数组
        DWORD					NumInfl;		//每个顶点最多受多少骨骼的影响
        DWORD					NumAttributeGroups;		//属性组数量,即子网格的数量
        LPD3DXBUFFER			pBoneCombinationBuf;	//骨骼组合表
        Matrix**			    ppBoneMatrixPtrs;		//存放骨骼的组合变换矩阵
        Matrix*				    pBoneOffsetMatrices;	//存放骨骼的初始变换矩阵
        DWORD					NumPaletteEntries;		//骨骼数量上限
    };

    MeshContainer::MeshContainer(const char * name)
    {
        Name = _strdup(name ? name : "NUL");

        ZeroMemory(&MeshData, sizeof(MeshData));

        NumMaterials = 0;
        pMaterials = nullptr; //材质数组
        ppTextures = nullptr; //纹理数组
        pEffects = nullptr;
        pAdjacency = nullptr;
        pSkinInfo = nullptr;

        pAttributeTable = nullptr;
        NumInfl = 0;		//每个顶点最多受多少骨骼的影响
        NumAttributeGroups = 0;		//属性组数量,即子网格的数量
        pBoneCombinationBuf = nullptr;	//骨骼组合表
        ppBoneMatrixPtrs = nullptr;		//存放骨骼的组合变换矩阵
        pBoneOffsetMatrices = nullptr;	//存放骨骼的初始变换矩阵
        NumPaletteEntries = 0;		//骨骼数量上限
    }

    MeshContainer::~MeshContainer()
    {
        free(Name);

        SafeDeleteArray(pAdjacency);
        SafeDeleteArray(pMaterials);
        SafeDeleteArray(pBoneOffsetMatrices);

        SafeDeleteArray(ppTextures);//纹理不需要释放，有纹理管理器管理。
        SafeDeleteArray(ppBoneMatrixPtrs);

        SafeRelease(pBoneCombinationBuf);
        SafeRelease(MeshData.pMesh);
        SafeRelease(pSkinInfo);

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
        Name = _strdup(name ? name : "NUL");

        pMeshContainer = nullptr;
        pFrameSibling = nullptr;
        pFrameFirstChild = nullptr;

        D3DXMatrixIdentity(&TransformationMatrix);
        D3DXMatrixIdentity(&WorldTransformationMatrix);
    }

    BoneFrame::~BoneFrame()
    {
        free(Name);

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

    BoneFrame* BoneFrame::find(const char * name)
    {
        if (strcmp(name, Name) == 0 ) return this;

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
        D3DXMatrixMultiply(&WorldTransformationMatrix, &TransformationMatrix, &matParent);

        if (pFrameSibling != NULL)
        {
            ((BoneFrame*) pFrameSibling)->updateMatrix(matParent);
        }

        if (pFrameFirstChild != NULL)
        {
            ((BoneFrame*) pFrameFirstChild)->updateMatrix(static_cast<Matrix&>(WorldTransformationMatrix));
        }
    }

    //////////////////////////////////////////////////////////////////////
    // CAllocateHierarchy
    //////////////////////////////////////////////////////////////////////

    class CAllocateHierarchy : public ID3DXAllocateHierarchy
    {
    public:
        CAllocateHierarchy(Mesh *pMesh)
            : m_pMesh(pMesh)
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
        Mesh*		m_pMesh;
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
            //D3DXComputeNormals(pMesh, NULL);
            pMeshContainer->MeshData.pMesh = pMesh;
            pMeshContainer->MeshData.pMesh->AddRef();
        }


        DWORD NumFaces = pMesh->GetNumFaces();
        pMeshContainer->NumMaterials = max<DWORD>(1, NumMaterials);

        pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
        pMeshContainer->ppTextures = new TexturePtr[pMeshContainer->NumMaterials];
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
            D3DCOLORVALUE color = { 1.0f, 1.0f, 1.0f, 1.0f };
            pMeshContainer->pMaterials[0].MatD3D.Diffuse = color;
            pMeshContainer->pMaterials[0].MatD3D.Specular = color;
            pMeshContainer->pMaterials[0].MatD3D.Diffuse = color;
            pMeshContainer->pMaterials[0].MatD3D.Power = 64.0f;
        }

        for (DWORD i = 0; i < pMeshContainer->NumMaterials; ++i)
        {
            //复制材质
            LPD3DXMATERIAL destMaterial = pMeshContainer->pMaterials + i;
            D3DXCOLOR diffuse = destMaterial->MatD3D.Diffuse;
            destMaterial->MatD3D.Ambient = diffuse * 0.25f;
            destMaterial->MatD3D.Specular = diffuse;
            destMaterial->MatD3D.Power = max(destMaterial->MatD3D.Power, 1.0f) * 10.f;

            //提取纹理
            if (destMaterial->pTextureFilename != NULL)
            {
                std::wstring name;
                Lazy::charToWChar(name, destMaterial->pTextureFilename);
                Lazy::removeFilePath(name);
                if (Lazy::getfs()->searchFile(name, name, L""))
                {
                    pMeshContainer->ppTextures[i] = TextureMgr::instance()->get(name);
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

            //生成蒙皮网格模型
            hr = m_pMesh->generateSkinnedMesh(pMeshContainer);

            if (FAILED(hr))
            {
                LOG_ERROR(_T("Failed generateSkinnedMesh: %x"), hr);
                delete pMeshContainer;
                return hr;
            }
        }

        m_pMesh->m_subMeshes.push_back(pMeshContainer);
        *ppNewMeshContainer = pMeshContainer;
        return hr;
    }

    //////////////////////////////////////////////////////////////////////
    // Mesh
    //////////////////////////////////////////////////////////////////////
    /*static*/ EffectPtr Mesh::s_skinnedEffect;
    /*static*/ EffectPtr Mesh::s_noskinnedEffect;

    Mesh::Mesh(const tstring & source)
        : IResource(source)
        , m_pAnimController(nullptr)
        , m_bone(nullptr)
        , m_dwTrangleCnt(0)
    {
        m_aabb.min.set(-1.0f, 0, -1.0f);
        m_aabb.max.set(1.0f, 1.0f, 1.0f);
    }

    Mesh::~Mesh()
    {
        if (m_bone)
            delete m_bone;

        SAFE_RELEASE(m_pAnimController);
    }

    /** 加载资源。*/
    bool Mesh::load()
    {
        assert(m_bone == nullptr && "Loaded more than once!");

        tstring realPath;
        if (!getfs()->getRealPath(realPath, m_source))
        {
            LOG_ERROR(L"Mesh '%s' was not found!", m_source.c_str());
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
            LOG_ERROR(L"Load Mesh '%s' failed! hr=0x%x, code=0x%x",
                realPath.c_str(), hr, GetLastError());
            return false;
        }

        hr = bindBone(m_bone);
        if (FAILED(hr))
        {
            LOG_ERROR(L"Failed bindBone Mesh '%s' failed! hr=0x%x, code=0x%x",
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

    void Mesh::render()
    {
        if (!m_bone) return;

        dx::Device* pDevice = rcDevice()->getDevice();

        m_dwTrangleCnt = 0;

        RSHolder holder1(pDevice, D3DRS_CULLMODE, D3DCULL_CCW);
        //RSHolder holder2(pDevice, D3DRS_ALPHABLENDENABLE, FALSE);

        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

        m_bone->updateMatrix(rcDevice()->getWorld());
        for (MeshContainer *pMesh : m_subMeshes)
        {
            drawMeshContainer(pMesh);
        }

        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    }

    /** 克隆动画控制器。*/
    dx::AnimController* Mesh::cloneAnimaCtrl(void)
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


    HRESULT Mesh::generateSkinnedMesh(MeshContainer *pMeshContainer)
    {
        assert(pMeshContainer);

        LPD3DXSKININFO pSkinInfo = pMeshContainer->pSkinInfo;
        assert(pSkinInfo != NULL);

        //复制骨骼信息
        const DWORD nBones = pSkinInfo->GetNumBones();
        pMeshContainer->pBoneOffsetMatrices = new Matrix[nBones];
        for (DWORD i = 0; i < nBones; ++i)
        {
            pMeshContainer->pBoneOffsetMatrices[i] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i));
        }

        //转换成蒙皮网格
        HRESULT hr;
        LPD3DXMESH pOrigMesh = pMeshContainer->MeshData.pMesh;
        pMeshContainer->MeshData.pMesh = NULL;
        SafeRelease(pMeshContainer->pBoneCombinationBuf);
        do
        {
            DWORD NumMaxFaceInfl;

            //获取网格模型的索引缓冲区
            LPDIRECT3DINDEXBUFFER9 pIB;
            hr = pOrigMesh->GetIndexBuffer(&pIB);
            if (FAILED(hr))
                break;

            //影响一个面的矩阵不会超过12个，自己画就晓得了
            hr = pSkinInfo->GetMaxFaceInfluences(
                pIB,
                pOrigMesh->GetNumFaces(),
                &NumMaxFaceInfl);

            pIB->Release();
            if (FAILED(hr))
                break;

            NumMaxFaceInfl = min<DWORD>(NumMaxFaceInfl, 12);
            pMeshContainer->NumPaletteEntries = min<DWORD>(MaxNumSkinBone, nBones);

            //生成蒙皮网格模型
            hr = pSkinInfo->ConvertToIndexedBlendedMesh(
                pOrigMesh,
                D3DXMESHOPT_VERTEXCACHE | D3DXMESH_MANAGED,
                pMeshContainer->NumPaletteEntries,
                pMeshContainer->pAdjacency,
                NULL,
                NULL,
                NULL,
                &pMeshContainer->NumInfl,
                &pMeshContainer->NumAttributeGroups,
                &pMeshContainer->pBoneCombinationBuf,
                &pMeshContainer->MeshData.pMesh);

        } while (0);
        pOrigMesh->Release();
        return hr;
    }

    HRESULT Mesh::bindBone(BoneFrame *pFrame)
    {
        assert(pFrame);

        HRESULT hr;
         
        LPD3DXMESHCONTAINER pMeshContainer = pFrame->pMeshContainer;
        while (pMeshContainer != NULL)
        {
            hr = bindBoneToMesh(pFrame, (MeshContainer*) pMeshContainer);
            if (FAILED(hr)) return hr;

            pMeshContainer = pMeshContainer->pNextMeshContainer;
        }

        //如果存在兄弟骨骼
        if (pFrame->pFrameSibling != NULL)
        {
            hr = bindBone( (BoneFrame*) pFrame->pFrameSibling );
            if (FAILED(hr)) return hr;
        }

        //如果存在子骨骼
        if (pFrame->pFrameFirstChild != NULL)
        {
            hr = bindBone( (BoneFrame*) pFrame->pFrameFirstChild );
            if (FAILED(hr)) return hr;
        }

        return S_OK;
    }

    HRESULT Mesh::bindBoneToMesh(BoneFrame *pOwner, MeshContainer *pMeshContainer)
    {
        LPD3DXSKININFO pSkinInfo = pMeshContainer->pSkinInfo;

        if (pSkinInfo != NULL)
        {
            DWORD nBones = pSkinInfo->GetNumBones();
            pMeshContainer->ppBoneMatrixPtrs = new Matrix*[nBones];

            for (DWORD i = 0; i < nBones; ++i)
            {
                BoneFrame *pFrame = m_bone->find(pSkinInfo->GetBoneName(i));
                if (pFrame != NULL)
                {
                    pMeshContainer->ppBoneMatrixPtrs[i] = &pFrame->WorldTransformationMatrix;
                }
                else
                {
                    pMeshContainer->ppBoneMatrixPtrs[i] = NULL;
                }
            }
        }
        else
        {
            pMeshContainer->ppBoneMatrixPtrs = new Matrix*[1];
            pMeshContainer->ppBoneMatrixPtrs[0] = &( pOwner->WorldTransformationMatrix );
        }

        return S_OK;
    }

    void Mesh::drawMeshOnly(MeshContainer *pMeshContainer)
    {
        EffectPtr effect = s_noskinnedEffect;
        if (!effect)
            return;

        rcDevice()->pushWorld( *(pMeshContainer->ppBoneMatrixPtrs[0]) );

        for (DWORD AttribID = 0; AttribID < pMeshContainer->NumMaterials; ++AttribID)
        {
            EffectConstant *pTexture = effect->getConstant("g_texture");
            if (!pTexture || NULL == pMeshContainer->ppTextures[AttribID])
                continue;

            pTexture->bindValue(pMeshContainer->ppTextures[AttribID]);
            rcDevice()->setMaterial(pMeshContainer->pMaterials[AttribID].MatD3D);

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

    void Mesh::drawMeshContainer(MeshContainer *pMeshContainer)
    {
        if (pMeshContainer->pSkinInfo == NULL)
        {
            drawMeshOnly(pMeshContainer);
            return;
        }
     
        if (pMeshContainer->NumPaletteEntries > MaxNumSkinBone)
            return;

        EffectPtr effect = s_skinnedEffect;
        if (!effect)
            return;

        EffectConstant *pMatrixArray = effect->getConstant("mWorldMatrixArray");
        if (!pMatrixArray)
            return;

        EffectConstant *pCurNumBones = effect->getConstant("CurNumBones");
        if (!pCurNumBones)
            return;

        EffectConstant *pTexture = effect->getConstant("g_texture");

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
                    D3DXMatrixMultiply(&g_bonePalette[iPaletteEntry],
                        &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
                        pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
                }
            }

            pMatrixArray->bindValue(g_bonePalette, pMeshContainer->NumPaletteEntries);
            pCurNumBones->bindValue(int(pMeshContainer->NumInfl) - 1);
            pTexture->bindValue(pMeshContainer->ppTextures[AttribID]);
            rcDevice()->setMaterial(pMeshContainer->pMaterials[AttribID].MatD3D);

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
        m_dwTrangleCnt += pMeshContainer->MeshData.pMesh->GetNumFaces();
    }

    bool Mesh::initShaders()
    {
        if (!s_skinnedEffect)
        {
            s_skinnedEffect = EffectMgr::instance()->get(_T("shader/skinned_mesh.fx"));
            s_noskinnedEffect = EffectMgr::instance()->get(_T("shader/static_mesh.fx"));
        }
        return s_skinnedEffect != NULL;
    }

    void Mesh::selectTechnique(const char * name)
    {
        if (s_noskinnedEffect) 
            s_noskinnedEffect->setTechnique(name);

        if (s_skinnedEffect)
            s_skinnedEffect->setTechnique(name);
    }

}//end namespace Lazy
