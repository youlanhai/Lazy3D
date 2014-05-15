#pragma once

namespace Lazy
{

    ///按钮类
    class LZUI_API CButton : public IControl
    {
    public:
        MAKE_UI_HEADER(CButton, uitype::Button)

        CButton(void);

        ~CButton(void);

        virtual void create(
            int id,
            const tstring & caption,
            const tstring & image,
            int x,
            int y,
            int w,
            int h);

        virtual void update(float fElapse);

        virtual void render(IUIRender * pDevice);

        /** 设置按钮状态。请尽量调用该方法设置按钮状态，而不是给成员变量m_state直接赋值。*/
        virtual void setBtnState(int state);
        int getBtnState(void){ return m_state; }

        void setStateColor(DWORD state, DWORD cr);
        DWORD getStateColor(DWORD state);

        /** 启禁用颜色绘制。*/
        virtual void enableColor(bool e){ m_colorEnable = e; }

        /** 启禁用位置漂移*/
        virtual void enablePosMove(bool e){ m_posMoveEnable = e; }

        /** 启禁用用户绘制。如果启用，则颜色绘制和位置漂移会被禁止。*/
        virtual void enableUserDraw(bool d);

        virtual void setText(const tstring & text) override;
        virtual void setFont(const tstring & font) override;
        virtual void setColor(uint32 color) override;

    protected:
        virtual bool onEvent(const SEvent & event) override;

        virtual void renderImage(IUIRender * pDevice, CRect rc);
        virtual void renderText(IUIRender * pDevice, CRect rc);

        /** 获得图形名称。如果名称中含有%d，则程序根据当前按钮状态，自动解析为合适的名称。*/
        virtual tstring getRealImage(void);

    protected:
        CColorFade      m_crFade;
        bool            m_colorEnable;
        bool            m_posMoveEnable;

        int             m_state;
        DWORD           m_stateColor[5];
        FontPtr         m_fontPtr;
        TextSpritePtr   m_textSprite;
    };


}//namespace Lazy