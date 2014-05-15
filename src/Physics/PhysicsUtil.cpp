
#include "stdafx.h"
#include "PhysicsUtil.h"

namespace Physics
{
    float getMatrixScale(const Matrix4x4 & matrix)
    {
        Vector3 pos0, pos1;
        D3DXVec3TransformCoord(&pos0, &Vector3(0, 0, 0), &matrix);
        D3DXVec3TransformCoord(&pos1, &Vector3(10000.0f, 0, 0), &matrix);
        return D3DXVec3Length(&(pos0 - pos1))/10000.0f;
    }

    void splitAABB(Plane & p, AABB & front, AABB & back, const AABB & aabb, int axis)
    {
        ZeroMemory(&p, sizeof(p));

        front = back = aabb;

        Vector3 center;
        aabb.getCenter(center);

        if (axis == AXIS_X)
        {
            p.normal.x = 1.0f;
            back.max.x = front.min.x =  center.x;
        }
        else if (axis == AXIS_Y)
        {
            p.normal.y = 1.0f;
            back.max.y = front.min.y =  center.y;
        }
        else if (axis == AXIS_Z)
        {
            p.normal.z = 1.0f;
            back.max.z = front.min.z =  center.z;
        }
        else
        {
            assert(0 && "calSplitPlane : wrong flag.");
        }
        p.distance = D3DXVec3Dot(&center, &p.normal);


    }

    void calTrianglesAABB(AABB & aabb, const TriangleIndices & indices, const TriangleSet & triangles)
    {
        aabb.zero();
        for (TriangleIndices::const_iterator it = indices.begin();
            it != indices.end(); ++it)
        {
            aabb.addTriangle(triangles[*it]);
        }
    }

    //////////////////////////////////////////////////////////////////////////

    MeshExtractor::MeshExtractor()
        : m_isValid(false)
    {
    }

    MeshExtractor::~MeshExtractor()
    {

    }

    bool MeshExtractor::build(ID3DXMesh *pMesh)
    {
        if (NULL == pMesh)
        {
            throw std::invalid_argument("MeshExtractor::build : pMesh must not be NULL!");
        }

        m_isValid = false;

        DWORD mNbTris = pMesh->GetNumFaces();
        DWORD mNbVerts = pMesh->GetNumVertices();

        DWORD step = pMesh->GetNumBytesPerVertex();
        bool isIdx32 = pMesh->GetOptions() & D3DXMESH_32BIT;

        BYTE * pVertexBuffer = NULL;
        BYTE * pIndexBuffer = NULL;


        if(FAILED(pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVertexBuffer)))
        {
            return false;
        }

        if(FAILED(pMesh->LockIndexBuffer(D3DLOCK_READONLY, (void**)&pIndexBuffer)))
        {
            pMesh->UnlockVertexBuffer();
            return false;
        }

        m_triangles.reserve(mNbTris);
        Triangle triangle;
        DWORD i0, i1, i2;
        for (DWORD i=0; i<mNbTris; ++i)
        {
            //取出3个索引
            if (isIdx32)
            {
                i0 = *((DWORD*)pIndexBuffer + i*3);
                i1 = *((DWORD*)pIndexBuffer + i*3 + 1);
                i2 = *((DWORD*)pIndexBuffer + i*3 + 2);
            }
            else
            {
                i0 = *((WORD*)pIndexBuffer + i*3);
                i1 = *((WORD*)pIndexBuffer + i*3 + 1);
                i2 = *((WORD*)pIndexBuffer + i*3 + 2);
            }

            //取出3个顶点
            triangle.a = *((Vector3*)(pVertexBuffer + step*i0));
            triangle.b = *((Vector3*)(pVertexBuffer + step*i1));
            triangle.c = *((Vector3*)(pVertexBuffer + step*i2));

            m_triangles.push_back(triangle);
        }

        pMesh->UnlockIndexBuffer();
        pMesh->UnlockVertexBuffer();

        m_isValid = true;
        return true;
    }

    size_t MeshExtractor::getBytes() const
    {
        return sizeof(*this) + m_triangles.capacity() * sizeof(Triangle);
    }

}
