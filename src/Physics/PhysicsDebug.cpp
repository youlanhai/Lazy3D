
#include "stdafx.h"
#include "PhysicsDebug.h"

namespace Physics
{

    struct CustomVertex
    {
        Vector3 pos;
        DWORD color;

        static DWORD FVF;
        static DWORD SIZE;
    };
    DWORD CustomVertex::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE;
    DWORD CustomVertex::SIZE = sizeof(CustomVertex);

    void drawLine(IDirect3DDevice9* pDevice, 
        const Vector3 & a, 
        const Vector3 & b, 
        DWORD color/*=0xffff0000*/)
    {
        CustomVertex vertex [2];
        vertex[0].pos = a;
        vertex[0].color = color;
        vertex[1].pos = b;
        vertex[1].color = color;

        pDevice->SetTexture(0 , NULL);
        pDevice->SetFVF(CustomVertex::FVF);
        pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, vertex, CustomVertex::SIZE);
    }

    void drawTriangle(LPDIRECT3DDEVICE9 pDevice, 
        const Vector3 point[3], 
        DWORD color/*=0xffff0000*/)
    {
        CustomVertex vertex [3];
        for(int i=0; i<3; ++i)
        {
            vertex[i].pos = point[i];
            vertex[i].color = color;
        }
        pDevice->SetTexture(0 , NULL);
        pDevice->SetFVF(CustomVertex::FVF);
        pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, vertex, CustomVertex::SIZE);
    }

    void drawTriangle(LPDIRECT3DDEVICE9 pDevice, 
        const Vector3 & a,
        const Vector3 & b,
        const Vector3 & c,
        DWORD color/*=0xffff0000*/)
    {
        Vector3 point[3] = {a, b, c};
        drawTriangle(pDevice, point, color);
    }

    void drawTriangle(LPDIRECT3DDEVICE9 pDevice, 
        const Triangle & tri, 
        DWORD color/*=0xffff0000*/)
    {
        drawTriangle(pDevice, tri.a, tri.b, tri.c, color);
    }

    void drawFRect(LPDIRECT3DDEVICE9 pDevice,
        const FRect & rc,
        float y /*= 0.0f*/,
        DWORD color/*=0xffffffff*/)
    {
        CustomVertex vertex [5];
        vertex[0].pos.set(rc.left, y, rc.top);
        vertex[1].pos.set(rc.right, y, rc.top);
        vertex[2].pos.set(rc.right, y, rc.bottom);
        vertex[3].pos.set(rc.left, y, rc.bottom);
        vertex[4].pos = vertex[0].pos;
        
        for (int i=0; i<5; ++i)
        {
            vertex[i].color = color;
        }

        pDevice->SetTexture(0 , NULL);
        pDevice->SetFVF(CustomVertex::FVF);
        pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, vertex, CustomVertex::SIZE);
    }

    void drawFRectSolid(LPDIRECT3DDEVICE9 pDevice,
        const FRect & rc,  float y, DWORD color)
    {
        CustomVertex vertex[4];
        vertex[0].pos.set(rc.left, y, rc.top);
        vertex[1].pos.set(rc.right, y, rc.top);
        vertex[2].pos.set(rc.left, y, rc.bottom);
        vertex[3].pos.set(rc.right, y, rc.bottom);
        
        for (int i=0; i<4; ++i)
        {
            vertex[i].color = color;
        }

        pDevice->SetTexture(0 , NULL);
        pDevice->SetFVF(CustomVertex::FVF);
        pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, CustomVertex::SIZE);
    }

    void drawCollisionInfoTri(LPDIRECT3DDEVICE9 pDevice, 
        const Physics::CollisionInfo & info)
    {
        for (Physics::TriangleSet::const_iterator it = info.hitTriangles.begin();
            it != info.hitTriangles.end(); ++it)
        {
            drawTriangle(pDevice, it->a, it->b, it->c);
        }
    }

    void drawCollisionInfoOBB(LPDIRECT3DDEVICE9 pDevice, 
        const Physics::CollisionInfo & info, bool withRotation/*=true*/)
    {
        if (withRotation)
        {
            Vector3 dir = info.newPos - info.oldPos;

            float angle = -atan2(dir.z, dir.x);
            Matrix4x4 matWord, matTrans;
            D3DXMatrixRotationY(&matWord, angle);
            D3DXMatrixTranslation(&matTrans, info.center.x, info.center.y, info.center.z);
            matWord *= matTrans;
            pDevice->SetTransform(D3DTS_WORLD, &matWord);
        }

        float dx = info.extends.x ;
        float dy = info.extends.y ;
        float dz = info.extends.z ;

        Vector3 ptMax(dx, dy, dz);
        Vector3 ptMin(-dx, -dy, -dz);

        drawLine(pDevice, ptMin, ptMax, 0xff00ff00);

        dx *= 2;
        dy *= 2;
        dz *= 2;

        Vector3 pt[8];
        pt[0] = ptMin;

        pt[1] = pt[0];
        pt[1].y += dy;

        pt[2] = pt[1];
        pt[2].z += dz;

        pt[3] = pt[0];
        pt[3].z += dz;

        for(int i=0; i<4; ++i)
        {
            pt[i+4] = pt[i];
            pt[i+4].x += dx;
        }

        for (int i=0; i<4; ++i)
        {
            drawLine(pDevice, pt[i], pt[(i+1) % 4], 0xff00ff00);
        }

        for (int i=0; i<4; ++i)
        {
            drawLine(pDevice, pt[i+4], pt[(i+1) % 4 + 4], 0xff00ff00);
        }

        for (int i=0; i<4; ++i)
        {
            drawLine(pDevice, pt[i], pt[i + 4], 0xff00ff00);
        }
    }

    void drawAABB(LPDIRECT3DDEVICE9 pDevice,
        const AABB & aabb, DWORD color)
    {
        //drawLine(pDevice, aabb.min, aabb.max, 0xffff0000);

        Vector3 diameter;
        aabb.getDiameter(diameter);

        Vector3 pt[8];
        pt[0] = aabb.min;

        pt[1] = pt[0];
        pt[1].y += diameter.y;

        pt[2] = pt[1];
        pt[2].z += diameter.z;

        pt[3] = pt[0];
        pt[3].z += diameter.z;

        for(int i=0; i<4; ++i)
        {
            pt[i+4] = pt[i];
            pt[i+4].x += diameter.x;
        }

        for (int i=0; i<4; ++i)
        {
            drawLine(pDevice, pt[i], pt[(i+1) % 4], color);
        }

        for (int i=0; i<4; ++i)
        {
            drawLine(pDevice, pt[i+4], pt[(i+1) % 4 + 4], color);
        }

        for (int i=0; i<4; ++i)
        {
            drawLine(pDevice, pt[i], pt[i + 4], color);
        }
    }

    void drawPolygon(IDirect3DDevice9* pDevice, const PolygonBase & poly, DWORD color)
    {
        size_t n = poly.size();
        if (n < 3)
        {
            return;
        }
        std::vector<CustomVertex> vertex(n);
        for (size_t i=0; i<n; ++i)
        {
            vertex[i].pos = poly[i];
            vertex[i].color = color;
        }

        pDevice->SetTexture(0, NULL);
        pDevice->SetFVF(CustomVertex::FVF);
        pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, n-2, (void*)&vertex[0], CustomVertex::SIZE);
    }


    struct DebugTriangle
    {
        Triangle triangle;
        DWORD color;
    };

    std::vector<DebugTriangle> g_debugTriangles;

    void addDebugDrawTriangle(const Triangle & tri, DWORD color)
    {
        g_debugTriangles.push_back(DebugTriangle());
        g_debugTriangles.back().triangle = tri;
        g_debugTriangles.back().color = color;
    }
    void drawDebugTriangle(IDirect3DDevice9 *pDevice)
    {
        Matrix4x4 world;
        D3DXMatrixIdentity(&world);
        pDevice->SetTransform(D3DTS_WORLD, &world);

        for (size_t i=0; i<g_debugTriangles.size(); ++i)
        {
            drawTriangle(pDevice, g_debugTriangles[i].triangle, g_debugTriangles[i].color);
        }
    }
    void clearDrawDebugTriangle()
    {
        g_debugTriangles.clear();
    }
}