#pragma once

#include "UIControl.h"

namespace Lazy
{

    /** 面板可以包含子控件，实现所有UI资源的统一管理，
    *  每一个应用程序最好只包含一个面板对象。
    */
    class LZUI_API Window : public Widget
    {
    public:
        MAKE_UI_HEADER(Window);

        /** 构造函数*/
        Window(void);

        /** 析构函数*/
        ~Window();

        virtual void create(const tstring & image, int x, int y);

        virtual void render(IUIRender * pDevice) override;

        ///子控件超出区域部分，不会被绘制。
        void setClipable(bool clip) { m_clipable = clip; }
        bool getClipable() const { return m_clipable; }

        void setImage(const tstring & image);
        const tstring & getImage() const;

        void ajustToImageSize();

        virtual void loadProperty(LZDataPtr root) override;
        virtual void saveProperty(LZDataPtr root) override;

    protected:
        tstring         m_image;
        TexturePtr      m_texture;
        bool            m_clipable;///<启用裁减
    };

}//namespace Lazy