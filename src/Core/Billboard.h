#pragma once

#include "base.h"
#include "RenderInterface.h"

#include "../Render/Texture.h"

namespace Lazy
{
    /** 公告板*/
    class LZDLL_API Billboard : public IBase, public IRenderable
    {
    public:
        Billboard(void);
        ~Billboard(void);

        bool createVertex(IDirect3DDevice9 *pDevice);

        void setSize(float w, float h);

        void setImage(const tstring & texName);
        void setTexture(TexturePtr pTex) { m_pTexture = pTex; }

        void setPos(const Vector3 & pos) { m_pos = pos; }

        virtual void update(float fElapse);
        virtual void render(IDirect3DDevice9 * pDevice);

        void show(bool s) { m_show = s; }
        bool visible() { return m_show; }

        void setMaxShowDistance(float s) { m_maxShowDistance = s; }
        float getShowMaxDistance() { return m_maxShowDistance; }

        bool isTooFar();

    protected:

        void updateVertex();

    protected:
        dx::VertexBuffer *      m_pVertex;
        TexturePtr              m_pTexture;
        Vector3                 m_pos;
        float                   m_width;
        float                   m_height;
        bool                    m_show;
        float                   m_maxShowDistance;
    };

} // end namespace Lazy
