#pragma once


namespace Lazy
{
    ///ui渲染器
    class GUIRender : public IUIRender
    {
        HWND        m_hWnd;
        dx::Device *m_device;
        EffectPtr   m_uiEffect;
        EffectPtr   m_fontEffect;
        Math::Matrix4x4 m_matWorldViewProj;

        MAKE_UNCOPY(GUIRender)

    public:
        GUIRender(dx::Device *pDevice, HWND hWnd);
        ~GUIRender();

        virtual bool isClipEnable();
        virtual void setClipEnalbe(bool enalbe);
        virtual void getClipRect(CRect & rect);
        virtual void setClipRect(const CRect & rect);

        virtual void drawRect(const CRect & rc, uint32 color);
        virtual void drawRect(const CRect & rc, uint32 color, TexturePtr texture);
        virtual void drawRect(const CRect & rc, const UVRect & srcRc, uint32 color, TexturePtr texture);
        virtual void drawRectFrame(const CRect & rc, int edgeSize, uint32 color);

        virtual bool textRenderBegin() override;
        virtual void textRenderEnd() override;
        virtual void drawWord(const CRect & dest, const UVRect & src, uint32 color, dx::Texture *texture);

        virtual void renderBegin() override;
        virtual void renderEnd() override;

        void setUIShader(const tstring & file);
        void setFontShader(const tstring & file);

    private:
        void setCommonRenderState();
        void saveRenderState();
        void restoreRenderState();
    };
}//end namespace Lazy