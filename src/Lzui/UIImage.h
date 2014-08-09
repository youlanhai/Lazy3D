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

        ///设置背景图像名称
        virtual void setImage(const tstring & image) { m_image = image; }
        virtual const tstring & getImage(void) const { return m_image; }

    protected:
        TexturePtr      m_texture;
        tstring         m_image;        ///<图片名称
    };

}//namespace Lazy