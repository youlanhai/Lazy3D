#pragma once

#include "UIControl.h"

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

        ///加载布局。
        virtual void loadProperty(LZDataPtr config) override;

        ///保存布局
        virtual void saveProperty(LZDataPtr config) override;

    protected:
        TexturePtr      m_texture;
        tstring         m_image;        ///<图片名称
    };

}//namespace Lazy