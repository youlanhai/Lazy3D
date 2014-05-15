#pragma once

#include "Base.h"
#include "RenderObj.h"
#include "..\utility\UtilConfig.h"

class LZDLL_API cTextTexture : public IBase
{
public:
    ~cTextTexture(void);

    void setText(const std::wstring & text);
    const std::wstring & getText(void){ return m_text; }

    void setColor(DWORD cr);

    IDirect3DTexture9* getTexture(void){ return m_pTexture; }

protected:

    cTextTexture();

    void init(const std::wstring & text, const std::wstring & font, DWORD color, int width, int height);

    void setTexture(IDirect3DTexture9* pTex){ m_pTexture = pTex; }

    void drawText();

protected:

    IDirect3DTexture9   *m_pTexture;
    std::wstring         m_text;
    std::wstring         m_font;
    DWORD               m_color;
    int                 m_width;
    int                 m_height;

    friend class cTextTextureFactory;
};

class LZDLL_API cTextTextureFactory : public IBase
{
public: 
    cTextTextureFactory(IDirect3DDevice9* pDevice);

    ~cTextTextureFactory();

    cTextTexture* createObj(const std::wstring & text, const std::wstring & font, DWORD color, int width, int height);

    void updateText(cTextTexture* p);
protected:
    IDirect3DDevice9    *m_pd3dDevice;
};

LZDLL_API cTextTextureFactory* getTextTextureFactory(void);


class LZDLL_API TextTextureEx : public IBase
{
public:
    TextTextureEx();
    ~TextTextureEx();

    void setText(const std::wstring & text);
    void setColor(DWORD cr);
    void setFont(const std::wstring & font);

    LPDIRECT3DTEXTURE9 getTexture() const { return m_pTexture; }

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

    const std::wstring & getText() const { return m_text; }
    const std::wstring & getFont() const { return m_font; }
    DWORD getColor() const { return m_color; }

    void loadTexture();

private:
    std::wstring     m_text;
    std::wstring     m_font;
    DWORD           m_color;
    LPDIRECT3DTEXTURE9 m_pTexture;
    bool            m_dirty;
    int             m_width;
    int             m_height;
};