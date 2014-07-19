//Mesh.cpp
#include "stdafx.h"
#include "StaticMesh.h"


#include "RenderDevice.h"
#include "Texture.h"

namespace Lazy
{

    StaticMesh::StaticMesh(const tstring & source)
        : IResource(source)
        , m_pMesh(nullptr)
    {

        m_aabb.max.set(1.f, 1.f, 1.f);
        m_aabb.min.set(-1.f, -1.f, -1.f);
    }

    StaticMesh::~StaticMesh()
    {
        SAFE_RELEASE(m_pMesh);
    }


    /** 加载资源。*/
    bool StaticMesh::load()
    {
        dx::Device *pDevice = Lazy::rcDevice()->getDevice();

        tstring realPath;
        if (!getfs()->getRealPath(realPath, m_source))
        {
            LOG_ERROR(L"StaticMesh '%s' was not found!", m_source.c_str());
            return false;
        }

        //从x文件中加载mesh
        ID3DXBuffer * pBuffer = NULL;
        DWORD numbers = 0;//材质数

        HRESULT hr = D3DXLoadMeshFromX(
                         realPath.c_str(),
                         D3DXMESH_MANAGED,
                         pDevice,
                         NULL,
                         &pBuffer,
                         NULL,
                         &numbers,
                         &m_pMesh);

        if (FAILED(hr))
        {
            LOG_ERROR(L"Load mesh '%s' failed! hr=0x%x, code=0x%x", realPath.c_str(), hr, GetLastError());
            return false;
        }

        //获取材质，创建纹理
        D3DXMATERIAL* pMat = (D3DXMATERIAL*) pBuffer->GetBufferPointer();
        m_vecMaterials.resize(numbers);
        m_vecTextures.resize(numbers);
        for (DWORD i = 0; i < numbers; ++i)
        {
            m_vecMaterials[i] = pMat[i].MatD3D;
            m_vecMaterials[i].Ambient = m_vecMaterials[i].Diffuse;
            m_vecTextures[i] = NULL;

            if (pMat[i].pTextureFilename != NULL)
            {
                //TODO 规范化mesh路径，以去除此处的搜索。
                std::wstring name;
                Lazy::charToWChar(name, pMat[i].pTextureFilename);
                Lazy::removeFilePath(name);

                if (Lazy::getfs()->searchFile(name, name, L""))
                {
                    TexturePtr ptr = TextureMgr::instance()->get(name);
                    if (ptr) m_vecTextures[i] = ptr->getTexture();
                }
                else
                {
                    LOG_ERROR(L"Texture '%s' was not found!", name.c_str());
                }
            }
        }

        //     m_pMesh->OptimizeInplace( D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE,
        //         NULL, NULL, NULL, NULL );

        DWORD FVF = m_pMesh->GetFVF();
        if ((FVF & D3DFVF_NORMAL) == 0)
        {
            //TODO 通过复制fvf，让mesh拥有法线

            D3DXComputeNormals(m_pMesh, NULL);
        }
        pBuffer->Release();

        LPVOID pVertex = NULL;
        if (SUCCEEDED(m_pMesh->LockVertexBuffer(0, &pVertex)))
        {
            D3DXComputeBoundingBox(
                (D3DXVECTOR3*) pVertex,
                m_pMesh->GetNumVertices(),
                m_pMesh->GetNumBytesPerVertex(),
                &m_aabb.min,
                &m_aabb.max
            );

            m_pMesh->UnlockVertexBuffer();
        }

        return true;
    }

    void StaticMesh::render()
    {
        if (m_pMesh == NULL) return;

        dx::Device * pDevice = rcDevice()->getDevice();

        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);

        for (DWORD i = 0; i < m_vecMaterials.size(); ++i)
        {
            pDevice->SetMaterial(&m_vecMaterials[i]);
            pDevice->SetTexture(0, m_vecTextures[i]);
            m_pMesh->DrawSubset(i);//绘制
        }

        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    }

}