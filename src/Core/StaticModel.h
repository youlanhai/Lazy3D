#pragma once

#include "Model.h"
#include "../Render/StaticMesh.h"

class LZDLL_API StaticModel : public iModel
{
public:
    StaticModel(void);

    ~StaticModel();

    virtual bool isMesh(void) const override { return true; }

    virtual bool load(const std::wstring & name) override;

    virtual void render(IDirect3DDevice9 * pDevice);

    dx::Mesh* getMesh(){ return m_mesh->getMesh(); }

protected:
    Lazy::StaticMeshPtr    m_mesh;
};