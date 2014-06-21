
#include "stdafx.h"
#include "TerrinMap.h"

#include "TopBoard.h"
#include "App.h"

namespace Lazy
{
    TopBoard::TopBoard(void)
        : m_pSource(nullptr)
        , m_biasHeight(2.0f)
        , m_pTextTexture(new TextTextureEx())
    {

        getApp()->addUpdateRender(this);
    }

    TopBoard::~TopBoard(void)
    {
        m_pTextTexture = NULL;

        getApp()->removeUpdateRender(this);
    }

    bool TopBoard::create(const std::wstring & text,
                           const std::wstring & font, DWORD color, float biasHeight)
    {
        m_pTextTexture->setText(text);
        m_pTextTexture->setFont(font);
        m_pTextTexture->setColor(color);
        m_biasHeight = biasHeight;

        return true;
    }

    void TopBoard::render(IDirect3DDevice9 * pDevice)
    {
        if (m_pSource)
        {
            m_pos = m_pSource->getPos();
            m_pos.y += m_biasHeight;
        }

        m_pTextTexture->loadTexture();

        setTexture(m_pTextTexture->getTexture());
        m_width = float(m_pTextTexture->getWidth());
        m_height = float(m_pTextTexture->getHeight());

        __super::render(pDevice);
    }

} // end namespace Lazy
