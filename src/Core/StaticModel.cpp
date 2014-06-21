
#include "stdafx.h"
#include "StaticModel.h"

namespace Lazy
{
///构造函数
    StaticModel::StaticModel(void)
    {
    }

    StaticModel::~StaticModel()
    {
    }

    bool StaticModel::load(const std::wstring & name)
    {
        IModel::load(name);

        m_mesh = StaticMeshMgr::instance()->get(name);
        if(m_mesh) setAABB(m_mesh->getAABB());

        return (m_mesh);
    }

    void StaticModel::render(IDirect3DDevice9 * pDevice)
    {
        if (m_mesh)
        {
            Matrix mat;
            getCombinedMatrix(mat);
            pDevice->SetTransform(D3DTS_WORLD, &mat);
            m_mesh->render();
        }

        IModel::render(pDevice);
    }

//////////////////////////////////////////////////////////////////////////

} // end namespace Lazy
