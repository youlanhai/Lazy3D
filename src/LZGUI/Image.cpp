#include "Image.h"
#include "GUIMgr.h"
#include "../Render/Texture.h"

CImage::CImage()
{
}

CImage::CImage(
               int id, 
               PControl parent, 
               stdstring image, 
               int x, 
               int y, 
               int w, 
               int h)
{
    create(id, parent, image, x, y, w, h);
}

CImage::~CImage(void)
{
    //getGuiMgr()->remove(this);
}

void CImage::create(
                  int id, 
                  PControl parent, 
                  stdstring image, 
                  int x, 
                  int y, 
                  int w, 
                  int h)
{
    m_texUV[0][0] = 0.0f;
    m_texUV[0][1] = 0.0f;
    m_texUV[1][0] = 1.0f;
    m_texUV[1][1] = 0.0f;
    m_texUV[2][0] = 1.0f;
    m_texUV[2][1] = 1.0f;
    m_texUV[3][0] = 0.0f;
    m_texUV[3][1] = 1.0f;
    //getGuiMgr()->add(this);
    init(id, parent, image, x, y);
    m_bkImage = image;
    m_size.set(w, h);
    if (id <= 0)
    {
        enableHandleMsg(false);
    }
}

void CImage::update(float fElapse)
{

}

void CImage::render(IDirect3DDevice9 * pDevice)
{
    if (!isVisible())
    {
        return;
    }
    updateVertex();
    //设置顶点缓冲区
    
    LPDIRECT3DTEXTURE9 pTex = Lazy::TextureMgr::instance()->getTexture(Lazy::charToWChar(m_bkImage));
    pDevice->SetTexture(0, pTex);
    pDevice->SetFVF(UIVertex::FVF);//设置灵活顶点格式
    pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (void*)m_vertexs, UIVertex::SIZE);
}

void CImage::updateVertex(void)
{
    CRect rc = getClientRect();
    rc = localToGlobal(rc);
    m_vertexs[0].set(float(rc.left), float(rc.top), 0, 0.8f, 
        m_crColor, m_texUV[0][0], m_texUV[0][1]);
    m_vertexs[1].set(float(rc.left), float(rc.bottom), 0, 0.8f, 
        m_crColor, m_texUV[3][0], m_texUV[3][1]);
    m_vertexs[2].set(float(rc.right), float(rc.top), 0, 0.8f, 
        m_crColor, m_texUV[1][0], m_texUV[1][1]);
    m_vertexs[3].set(float(rc.right), float(rc.bottom), 0, 0.8f, 
        m_crColor, m_texUV[2][0], m_texUV[2][1]);
}

///设置顶点的纹理UV。索引从左上角开始，顺时针转。
void CImage::setTexUV(int index, float u, float v)
{
    if (index>=0 && index<4)
    {
        m_texUV[index][0] = u;
        m_texUV[index][1] = v;
    }
}
//////////////////////////////////////////////////////////////////////////