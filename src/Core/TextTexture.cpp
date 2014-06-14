
#include "stdafx.h"
#include "Res.h"

#include "../Render/Texture.h"
#include "../Font/Font.h"

#include "TextTexture.h"
#include "App.h"


bool clearTextrue(LPDIRECT3DTEXTURE9 pTexture, DWORD lvl, BYTE value)
{
    if (!pTexture)
    {
        return false;
    }

    D3DSURFACE_DESC desc;
    if (FAILED(pTexture->GetLevelDesc(lvl, &desc)))
    {
        return false;
    }

    D3DLOCKED_RECT lockRC;
    if (FAILED(pTexture->LockRect(lvl, &lockRC, 0, 0)))
    {
        return false;
    }

    for (UINT i=0; i<desc.Height; ++i)
    {
        memset((BYTE*)(lockRC.pBits) + lockRC.Pitch * i, value, lockRC.Pitch);
    }

    pTexture->UnlockRect(lvl);
    return true;
}

int near2Size(int size)
{
    int t = 1;

    for (int i=1; i<32; ++i)
    {
        t = 1<<i;
        if(size < t)
        {
            return t;
        }
    }

    return t;
}

//////////////////////////////////////////////////////////////////////////


cTextTexture::cTextTexture(void)
{
    m_pTexture = NULL;
} 

cTextTexture::~cTextTexture(void)
{
    SafeRelease(m_pTexture);
} 

void cTextTexture::init(const std::wstring & text, const std::wstring & font, DWORD color, int width, int height)
{
    m_pTexture = NULL;
    m_text = text;
    m_font = font;
    m_color = color;
    m_width = width;
    m_height = height;
}

void cTextTexture::setText(const std::wstring & text)
{
    m_text = text;
    getTextTextureFactory()->updateText(this);
}

void cTextTexture::setColor(DWORD cr)
{
    m_color = cr;
    getTextTextureFactory()->updateText(this);
}

void cTextTexture::drawText()
{
    ID3DXFont* pFont = getResMgr()->getFontEx(m_font);
    if (NULL == pFont)
    {
        return ;
    }
    CRect rc(0, 0, m_width, m_height);
    DWORD flag = DT_CENTER|DT_VCENTER|DT_SINGLELINE;

    DWORD mark[] = {0xff000000, 0x4fffffff, 0xafffffff, 0xffffffff, 0xffffffff, 0xffffffff};
    int n = sizeof(mark) / sizeof(DWORD);
    for (int i=0; i<n; ++i)
    {
        pFont->DrawText( NULL,  m_text.c_str(),  -1,  &rc,  flag, m_color & mark[i]);
        rc.expand(-1, -1);
    }
}

//////////////////////////////////////////////////////////////////////////
cTextTextureFactory* g_pFactory_ = NULL;
LZDLL_API cTextTextureFactory* getTextTextureFactory(void)
{
    return g_pFactory_;
}

cTextTextureFactory::cTextTextureFactory(IDirect3DDevice9* pDevice)
{
    g_pFactory_ = this;
    m_pd3dDevice = pDevice;
    m_pd3dDevice->AddRef();
}

cTextTextureFactory::~cTextTextureFactory()
{
    SafeRelease(m_pd3dDevice);
}

cTextTexture* cTextTextureFactory::createObj(const std::wstring & text, const std::wstring & font, DWORD color, int width, int height)
{
    IDirect3DTexture9 *pTex = NULL;
    if(FAILED(m_pd3dDevice->CreateTexture(
        width, 
        height, 
        1, 
        D3DUSAGE_RENDERTARGET,
        D3DFMT_A8R8G8B8,
        D3DPOOL_DEFAULT,
        &pTex,
        NULL)))
    {
        return NULL;
    }

    cTextTexture *p = new cTextTexture();
    p->init(text, font, color, width, height);
    p->setTexture(pTex);
    updateText(p);
    return p;
}

void cTextTextureFactory::updateText(cTextTexture* p)
{
    if (NULL==p || NULL==p->getTexture())
    {
        return ;
    }

    IDirect3DSurface9   *pRenderSurface=NULL, *pBackSurface=NULL;
    p->getTexture()->GetSurfaceLevel(0, &pRenderSurface);
    m_pd3dDevice->GetRenderTarget(0, &pBackSurface);

    m_pd3dDevice->SetRenderTarget(0, pRenderSurface);
    m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
    if(SUCCEEDED(m_pd3dDevice->BeginScene()))
    {
        Matrix4x4   oldProjectMat;
        m_pd3dDevice->GetTransform(D3DTS_PROJECTION, &oldProjectMat);
        Matrix4x4 matProj;
        D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/4.0f, p->m_width/(float)p->m_height, 1, 100);
        m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
        //////////////////////////////////////////////////////////////////////////
        p->drawText();
        //////////////////////////////////////////////////////////////////////////
        m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &oldProjectMat); 
        m_pd3dDevice->EndScene();
    }
    m_pd3dDevice->SetRenderTarget(0, pBackSurface);
    SafeRelease(pRenderSurface);
}

//////////////////////////////////////////////////////////////////////////
TextTextureEx::TextTextureEx()
    : m_color(0xff000000)
    , m_pTexture(0)
    , m_dirty(true)
    , m_width(0)
    , m_height(0)
{

}
TextTextureEx::~TextTextureEx()
{
    SafeRelease(m_pTexture);
}

void TextTextureEx::setText(const std::wstring & text)
{
    if (m_text == text)
    {
        return;
    }
    m_text = text;
    m_dirty = true;
}

void TextTextureEx::setColor(DWORD cr)
{
    if (m_color == cr)
    {
        return;
    }
    m_color = cr;
    m_dirty = true;
}

void TextTextureEx::setFont(const std::wstring & font)
{
    if (m_font == font)
    {
        return;
    }

    m_font = font;
    m_dirty = true;
}


void TextTextureEx::loadTexture()
{
    if (!m_dirty)
    {
        return;
    }

    m_dirty = false;//尽管会创建失败，但也要设置为false。

    Lazy::FontPtr font = Lazy::FontMgr::instance()->getFont(m_font);
    if (!font)
    {
        clearTextrue(m_pTexture, 0, 0);

        Lazy::debugMessageA("ERROR : get font %s faild!", m_font.c_str());
        return;
    }

    m_width = (m_text.size() + 1) * font->getHeight();
    m_height = font->getHeight();
    
    if (m_pTexture)
    {
        D3DSURFACE_DESC desc;
        if(FAILED(m_pTexture->GetLevelDesc(0, &desc)))
        {
            clearTextrue(m_pTexture, 0, 0);

            debugMessage(_T("ERROR : get texture desc faild!"));
            return;
        }
        if(m_width > int(desc.Width) || m_height > int(desc.Height))
        {
            SafeRelease(m_pTexture);
        }
    }

    if (!m_pTexture)
    {

#if 1
        int texWidth = near2Size(m_width);
        int texHeight = near2Size(m_height);
#else
        int texWidth = m_width;
        int texHeight = m_height + 10;
#endif

        LPDIRECT3DDEVICE9 pDevice = getApp()->getDevice();
        if(FAILED(D3DXCreateTexture( pDevice, texWidth, texHeight, 
            1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTexture)))
        {
            debugMessage(_T("ERROR : create texture for %s faild"), m_text.c_str());
            return ;
        }

        debugMessage(_T("create topboard texture (w=%d, h=%d) : %s"), texWidth, texHeight, m_text.c_str());
    }

    clearTextrue(m_pTexture, 0, 0);
    font->fillTextToTexture(m_pTexture, 0, 0, m_text, m_color);
}