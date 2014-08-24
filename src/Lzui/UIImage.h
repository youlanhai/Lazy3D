#pragma once

#include "UIWidget.h"

namespace Lazy
{

    /** 图像控件。*/
    class LZUI_API Image : public Widget
    {
    public:
        MAKE_UI_HEADER(Image)
        Image(void);

        ~Image(void);

        void create(
            const tstring & image,
            int x,
            int y,
            int w,
            int h);

        virtual void render(IUIRender * pDevice) override;

        ///设置背景图像名称
        void setImage(const tstring & image);
        const tstring & getImage(void) const { return m_image; }

        void setColor(uint32 color){ m_color = color; }
        uint32 getColor() const { return m_color; }

        virtual bool setProperty(LZDataPtr config, const tstring & key, LZDataPtr val) override;

    protected:
        TexturePtr      m_texture;
        tstring         m_image;        ///<图片名称
        uint32          m_color;        ///<混合颜色
    };

}//namespace Lazy