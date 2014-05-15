
#include "..\LZData\lzdheader.h"
#include "..\LZ3DEngine\LZ3DEngine.h"
#include "..\lz3dengine\math3d.h"
#include "..\LZ3DEngine\Collision.h"
#include "..\LZ3DEngine\FileTool.h"
#include "..\LZData\LZData.h"
#include "..\LZ3DEngine\BSP.h"
#include "..\lzgui\LZGUI.h"

#include "Sample.h"
#include "ui.h"

#include "Opcode\Opcode.h"

LPD3DXMESH g_simpleMesh = NULL;
bool g_testLoaded = false;


class OpcodeMeshData : public Opcode::MeshInterface
{
public:
	OpcodeMeshData(ID3DXMesh *pMesh)
		: mMeshWraper(pMesh)
	{
		mNbTris = mMeshWraper.getNbFace();
		mNbVerts = mMeshWraper.getNbVertex();
	}

	virtual bool IsValid() const
	{
		return true;
	}

	virtual	 void GetTriangle(Opcode::VertexPointers& vp, udword index)	const
	{
		DWORD i = index * 3;
		DWORD i0 = mMeshWraper.getIndex(i++);
		DWORD i1 = mMeshWraper.getIndex(i++);
		DWORD i2 = mMeshWraper.getIndex(i);

		const BYTE* pV = mMeshWraper.getVertices();

		vp.Vertex[0] = (Point*)(pV + i0*mMeshWraper.getVertexStride());
		vp.Vertex[1] = (Point*)(pV + i1*mMeshWraper.getVertexStride());
		vp.Vertex[2] = (Point*)(pV + i2*mMeshWraper.getVertexStride());
	}

	virtual bool RemapClient(udword nb_indices, const udword* permutation)	const
	{
		return false;
	}
	

public:

	MeshWrap mMeshWraper;

};

OpcodeMeshData *g_pTestMeshData = NULL;

//////////////////////////////////////////////////////////////////////////

Sample::Sample(void)
{
}

Sample::~Sample(void)
{
}

void Sample::update(float fElapse)
{

}


void Sample::render(IDirect3DDevice9 * pDevice)
{

    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    D3DXMATRIX matWorld;
    D3DXMatrixIdentity(&matWorld);
    pDevice->SetTransform(D3DTS_WORLD, &matWorld);

    CMaterial::setMaterial(pDevice,
        D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),
        D3DXCOLOR(1.0f,1.0f,1.0f,1.0f));

#if 0
    Line lineA(
        Vector3(-100.0f, -100.0f, 100.0f),
        Vector3(-100.0f, 100.0f, 100.0f));

    Vector3 p0(0, 0, 100.0f);
    Vector3 p1(0, 20, 100.0f);

    Line lineB(
        Vector3(-200, 50.0f, 100.0f),
        Vector3(100.0f, 50.0f, 100.0f));

    Line lineC(
        Vector3(100, 0, 200),
        Vector3(0, 200, 200)
        );

    drawLine(pDevice, lineA.start, lineA.end, 0xffff0000);
    drawLine(pDevice, p0, lineA.project(p0), 0xff00ff00);
    drawLine(pDevice, p1, lineA.project(p1), 0xff00ff00);

    drawLine(pDevice, lineB.start, lineB.end, 0xffff0000);
    Vector3 cross;
    if (lineA.intersect(cross, lineB))
    {
        drawLine(pDevice, Vector3(0, 0, 0), cross, 0xffffff00);
    }

#endif

    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


    math3d::CollisionInfo coll(
        math3d::Vector3(-100, -20, -100),
        math3d::Vector3(100, -20, 100),
        50, 50);

    pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    //coll.drawDebug(pDevice);

    math3d::Triangle tri(
        math3d::Vector3(-200, 0, 0),
        math3d::Vector3(0, 200, 0),
        math3d::Vector3(200, 0, 0)
        );

    math3d::Polygon polyA(tri);
    drawPolygon(pDevice, polyA, 0x7f000000);

    pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

    coll.clip(polyA);
    drawPolygon(pDevice, polyA, 0x7fff0000);

#if 0
    BspTreePtr tree = loadBspTree("../../bin/res/bsp/小木屋.bsp");
    if(tree)
    {
        D3DXMATRIX world;
        D3DXMatrixIdentity(&world);
        //D3DXMatrixScaling(&world, 0.5f, 0.5f, 0.5f);

        int time = GetTickCount();
        tree->collision(coll, world);
        time = GetTickCount() - time;
        getUI()->setCollisionTime(time);

        //coll.resetPoints();
        coll.drawDebug(pDevice);
        tree->drawDebug(pDevice, &world);
    }
#endif

	cResMesh* pMesh = getResMgr()->getMesh("../../bin/res/mode/小木屋.x");
	if(pMesh)
	{
		Matrix3x3 obbRot;
		obbRot.Identity();
		OBB box(Point(0, 0, 0), Point(100, 20, 20), obbRot);

		//pMesh->render(pDevice);

		OpcodeMeshData meshInteracce(pMesh->getMesh());

		Opcode::OPCODECREATE opc;
		opc.mIMesh = &meshInteracce;
		opc.mQuantized = false;
		opc.mNoLeaf = true;
		opc.mKeepOriginal = true;
		opc.mCanRemap = false;

		Opcode::Model model;
		model.SetMeshInterface(&meshInteracce);
		bool Status = model.Build(opc);

		Opcode::OBBCollider Collider;
		Collider.SetFirstContact(true);
		Collider.SetPrimitiveTests(true);
		Collider.SetTemporalCoherence(true);
		//mSettings.SetupCollider(Collider);

		Opcode::OBBCache mCache;
		//Opcode::OpcodeSettings	mSettings;

		Status = Collider.Collide(mCache, box, model, null, null);
		
		if(Status)
		{
			if(Collider.GetContactStatus())
			{
				udword NbTris = Collider.GetNbTouchedPrimitives();
				const udword* Indices = Collider.GetTouchedPrimitives();

				getUI()->setCollisionTime(NbTris);

				math3d::Triangle tri;
				for(udword i=0; i<NbTris; ++i)
				{
					meshInteracce.mMeshWraper.getTriangle(Indices[i], tri);
					drawTriangle(pDevice, tri, 0xffff0000);
				}
			}
		}
	}

    //////////////////////////////////////////////////////////////////////////
    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

}