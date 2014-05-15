#pragma once

#include "Billboard.h"
#include "TextTexture.h"
#include "I3DObject.h"

class LZDLL_API cTopBoard : public cBillboard
{
public:
    cTopBoard(void);
    ~cTopBoard(void);

    bool create(const std::wstring & text, 
        const std::wstring & fontName,
        DWORD color, float biasHeight);

    void setText(const std::wstring & text){ m_pTextTexture->setText(text); }
    const std::wstring & getText(void){ return m_pTextTexture->getText(); }

    void setFont(const std::wstring & text){ m_pTextTexture->setFont(text); }
    const std::wstring & getFont(void) const { return m_pTextTexture->getFont(); }

    void setColor(DWORD cr){ m_pTextTexture->setColor(cr); }
    DWORD getColor() const { return m_pTextTexture->getColor(); }

    void setBiasHeight(float h){ m_biasHeight = h; }
    float getBiasHeight() const { return m_biasHeight; }

    virtual void render(IDirect3DDevice9 * pDevice);

    void setSource(I3DObject *pSource){ m_pSource = pSource; }
    I3DObject * getSource() const { return m_pSource; }

protected:
    RefPtr<TextTextureEx>    m_pTextTexture;
    I3DObject               *m_pSource;
    float                   m_biasHeight;
};
