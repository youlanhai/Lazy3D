﻿#pragma once

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
        int getBtnState(void) { return m_state; }

        void setStateColor(uint32 state, uint32 cr);
        uint32 getStateColor(uint32 state);

        /** 启禁用颜色绘制。*/
        void setColorFadable(bool e) { m_colorFadable = e; }
        bool getColorFadable() const { return m_colorFadable; }

        /** 启禁用位置漂移*/
        void setPosMovable(bool e) { m_posMovable = e; }
        bool getPosMovable() const { return m_posMovable; }

        virtual void setText(const tstring & text) override;
        virtual void setFont(const tstring & font) override;
        virtual void setColor(uint32 color) override;

        void setTextAlign(uint32 align) { m_textAlign = align; }
        uint32 getTextAlign() const { return m_textAlign; }

        virtual void loadFromStream(LZDataPtr config) override;
        virtual void saveToStream(LZDataPtr config) override;

    protected:
        virtual bool onEvent(const SEvent & event) override;

        virtual void renderImage(IUIRender * pDevice, CRect rc);
        virtual void renderText(IUIRender * pDevice, CRect rc);

        /** 获得图形名称。如果名称中含有%d，则程序根据当前按钮状态，自动解析为合适的名称。*/
        virtual tstring getRealImage(void);

    protected:
        uint32          m_textAlign;
        CColorFade      m_crFade;
        bool            m_colorFadable;
        bool            m_posMovable;

        int             m_state;
        uint32           m_stateColor[5];
        FontPtr         m_fontPtr;
        TextSpritePtr   m_textSprite;
    };


}//namespace Lazy