#pragma once

namespace Lazy
{

    /** 图像控件。*/
    class LZUI_API CImage : public Widget
    {
    public:
        MAKE_UI_HEADER(CImage)
        CImage(void);

        ~CImage(void);

        void create(
            int id,
            const tstring & image,
            int x,
            int y,
            int w,
            int h);

        virtual void render(IUIRender * pDevice) override;
        virtual void setImage(const tstring & image) override;

    protected:
        TexturePtr m_texture;
    };

}//namespace Lazy