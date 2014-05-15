#pragma once

#include "Collision.h"

namespace Physics
{
    void drawLine(IDirect3DDevice9* pDevice, 
        const Vector3 & a, 
        const Vector3 & b, 
        DWORD color=0xffff0000);

    void drawTriangle(LPDIRECT3DDEVICE9 pDevice, 
        const Vector3 point[3], 
        DWORD color=0xffff0000);

    void drawTriangle(LPDIRECT3DDEVICE9 pDevice, 
        const Vector3 & a,
        const Vector3 & b,
        const Vector3 & c,
        DWORD color=0xffff0000);

    void drawTriangle(LPDIRECT3DDEVICE9 pDevice, 
        const Triangle & tri, 
        DWORD color=0xffff0000);

    void drawFRect(LPDIRECT3DDEVICE9 pDevice,
        const FRect & rc,
        float y = 0.0f,
        DWORD color=0xffffffff);

    void drawFRectSolid(LPDIRECT3DDEVICE9 pDevice,
        const FRect & rc,
        float y = 0.0f,
        DWORD color=0xffffffff);

    void drawCollisionInfoTri(LPDIRECT3DDEVICE9 pDevice, 
        const Physics::CollisionInfo & info);

    void drawCollisionInfoOBB(LPDIRECT3DDEVICE9 pDevice, 
        const Physics::CollisionInfo & info, bool withRotation=true);

    void drawAABB(LPDIRECT3DDEVICE9 pDevice,
        const AABB & aabb, DWORD color=0xff00ff00);


    void drawPolygon(IDirect3DDevice9* pDevice, 
        const PolygonBase & poly, 
        DWORD color);

    void addDebugDrawTriangle(const Triangle & tri, DWORD color);
    
    void drawDebugTriangle(IDirect3DDevice9 *pDevice);
    
    void clearDrawDebugTriangle();
}