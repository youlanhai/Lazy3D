#pragma once

namespace Lazy
{

    /** 面板可以包含子控件，实现所有UI资源的统一管理，
    *  每一个应用程序最好只包含一个面板对象。
    */
    class LZUI_API CForm : public IControl
    {
    public:
        MAKE_UI_HEADER(CForm, uitype::Form);

        /** 构造函数*/
        CForm(void);

        /** 析构函数*/
        ~CForm();

        virtual void create(int id, const tstring & image, int x, int y);

        virtual void render(IUIRender * pDevice) override;

        ///子控件超出区域部分，不会被绘制。
        void enableClip(bool clip){ m_bClip = clip; }
        bool canClip() const { return m_bClip; }

        virtual void setImage(const tstring & image) override;

        void ajustToImageSize();

        virtual void loadFromStream(LZDataPtr root) override;
        virtual void saveToStream(LZDataPtr root) override;

    protected:
        TexturePtr      m_texture;
        bool            m_bClip;///<启用裁减
    };

}//namespace Lazy