#pragma once

#include "Billboard.h"
#include "../Render/TextTexture.h"

namespace Lazy
{
    class SceneNode;

    class LZDLL_API TopBoard : public Billboard
    {
    public:
        TopBoard(void);
        ~TopBoard(void);

        bool create(const std::wstring & text,
                    const std::wstring & fontName,
                    DWORD color, float biasHeight);

        void setText(const std::wstring & text) { m_pTextTexture->setText(text); }
        const std::wstring & getText(void) { return m_pTextTexture->getText(); }

        void setFont(const std::wstring & text) { m_pTextTexture->setFont(text); }
        const std::wstring & getFont(void) const { return m_pTextTexture->getFont(); }

        void setColor(DWORD cr) { m_pTextTexture->setColor(cr); }
        DWORD getColor() const { return m_pTextTexture->getColor(); }

        void setBiasHeight(float h) { m_biasHeight = h; }
        float getBiasHeight() const { return m_biasHeight; }

        virtual void render(IDirect3DDevice9 * pDevice);

        void setSource(SceneNode *pSource) { m_pSource = pSource; }
        SceneNode * getSource() const { return m_pSource; }

    protected:
        RefPtr<TextTexture>    m_pTextTexture;
        SceneNode               *m_pSource;
        float                   m_biasHeight;
    };

} // end namespace Lazy
