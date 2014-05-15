
#include "stdafx.h"
#include "StaticModel.h"

///构造函数
StaticModel::StaticModel(void)
{
}

StaticModel::~StaticModel()
{
}

bool StaticModel::load(const std::wstring & name)
{
    iModel::load(name);

    m_mesh = Lazy::StaticMeshMgr::instance()->get(name);
    if(m_mesh) setAABB(m_mesh->getAABB());

    return (m_mesh);
}

void StaticModel::render(IDirect3DDevice9 * pDevice)
{
    if (m_mesh)
    {
        Matrix4x4 mat;
        getCombinedMatrix(mat);
        pDevice->SetTransform(D3DTS_WORLD, &mat);
        m_mesh->render();
    }

    iModel::render(pDevice);
}

//////////////////////////////////////////////////////////////////////////
