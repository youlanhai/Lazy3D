#pragma once

#include "../Math/Vector.h"
#include "../Math/Matrix.h"
#include "UIDefine.h"

namespace Lazy
{

    /** 带纹理颜色顶点*/
    struct UIVertex
    {
        Vector3 position;
        uint32 color;
        Vector2 uv;

        static DWORD FVF;
        static int SIZE;
    };

    ///ui渲染器
    class GUIRender : public IUIRender
    {
        HWND        m_hWnd;
        dx::Device *m_device;
        EffectPtr   m_textureShader;
        EffectPtr   m_colorShader;
        EffectPtr   m_fontShader;
        Matrix      m_matWorldViewProj;
        bool        m_bClip;
        CRect       m_rcClip;

        MAKE_UNCOPY(GUIRender)

    public:
        GUIRender(dx::Device *pDevice, HWND hWnd);
        ~GUIRender();

        virtual bool isClipEnable() const;
        virtual void setClipEnalbe(bool enalbe);

        virtual void getClipRect(CRect & rect) const;
        virtual void setClipRect(const CRect & rect);

        virtual void drawRect(const CRect & rc, uint32 color);
        virtual void drawRect(const CRect & rc, uint32 color, TexturePtr texture);
        virtual void drawRect(const CRect & rc, const UVRect & srcRc, uint32 color, TexturePtr texture);
        virtual void drawRectFrame(const CRect & rc, int edgeSize, uint32 color);
        virtual void drawWord(const CRect & dest, const UVRect & src, uint32 color, TexturePtr texture);

        virtual void renderBegin() override;
        virtual void renderEnd() override;

        void setTextureShader(const tstring & file);
        void setColorShader(const tstring & file);
        void setFontShader(const tstring & file);

    private:
        void setCommonRenderState();
        void saveRenderState();
        void restoreRenderState();
    };
}//end namespace Lazy