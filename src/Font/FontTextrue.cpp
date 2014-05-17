#include "StdAfx.h"
#include "FontTextrue.h"

namespace Lazy
{

    //////////////////////////////////////////////////////////////////////////

    FontTextureNode::FontTextureNode(int w, int h, int dw, int dh)
        : m_texture(NULL)
        , m_width(w)
        , m_height(h)
        , m_dw(dw)
        , m_dh(dh)
        , m_index(0)
    {
        m_rows = m_width / m_dw;
        m_cols = m_height / m_dh;

        create();
    }

    FontTextureNode::~FontTextureNode()
    {
        if(m_texture && m_texture->getTexture())
        {
            static bool saved = false;
            if(!saved)
            {
                saved = true;
                D3DXSaveTextureToFile(_T("dump.font.png"), D3DXIFF_PNG, m_texture->getTexture(), NULL);
            }
        }
    }

    bool FontTextureNode::lockOne(int & x, int & y)
    {
        if (!valid() || empty())
        {
            debugMessage(_T("FontTextureNode::lockOne faild!"));
            return false;
        }

        y = m_index / m_cols * m_dh;
        x = m_index % m_cols * m_dw;

        ++m_index;
        return true;
    }

    bool FontTextureNode::create()
    {
        m_texture = nullptr;

        dx::Device *pDevice = rcDevice()->getDevice();
        dx::Texture * pTexture;

        if (FAILED(D3DXCreateTexture(pDevice, m_width, m_height, 1, 0, D3DFMT_A8, D3DPOOL_MANAGED, &pTexture)))
        {
            LOG_ERROR(_T("create texture faild!"));
            return false;
        }

        m_texture = new Texture(pTexture, D3DFMT_A8, m_width, m_height);
        pTexture->Release();

        LOG_INFO(_T("FontTextureNode::create : new texture w=%d, h=%d, dw=%d, dh=%d"), m_width, m_height, m_dw, m_dh);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    FontTextureMgr::FontTextureMgr()
    { }

    FontTextureMgr::~FontTextureMgr()
    { }

    FontTextureNodePtr FontTextureMgr::getTexNode(int fontSize)
    {
        if(!m_pool.empty())
        {
            FontTextureNodePtr ptr = m_pool.back();
            if (!ptr->empty()) return ptr;
        }

        FontTextureNodePtr ptr = createNode(fontSize);
        if (!ptr) return NULL;
        m_pool.push_back(ptr);

        return ptr;
    }


    FontTextureNodePtr FontTextureMgr::createNode(int fontSize)
    {
        FontTextureNodePtr ptr(new FontTextureNode(
            FontConfig::MaxTextureWidth, FontConfig::MaxTextureHeight, fontSize, fontSize));

        if (!ptr->valid()) return NULL;

        return ptr;
    }
}//namespace Lazy