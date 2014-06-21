#pragma once

#include "Model.h"
#include "../Render/StaticMesh.h"

namespace Lazy
{
    class LZDLL_API StaticModel : public IModel
    {
    public:
        StaticModel(void);

        ~StaticModel();

        virtual bool isMesh(void) const override { return true; }

        virtual bool load(const std::wstring & name) override;

        virtual void render(IDirect3DDevice9 * pDevice);

        dx::Mesh* getMesh() { return m_mesh->getMesh(); }

    protected:
        StaticMeshPtr    m_mesh;
    };

} // end namespace Lazy
