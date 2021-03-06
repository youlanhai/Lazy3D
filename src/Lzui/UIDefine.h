﻿#pragma once

#define LUI_EDITOR

#include "../utility/UtilConfig.h"
#include "../utility/SmartPtr.h"
#include "../utility/DataSection/DataSection.h"
#include "../utility/Tools2d.h"
#include "../utility/VisitPool.h"

#include "../Render/Config.h"
#include "../Render/RenderDevice.h"
#include "../Render/Texture.h"
#include "../Render/Effect.h"

#include "../Render/Font.h"


namespace Lazy
{

    class Widget;

    typedef RefPtr<Widget> WidgetPtr;
    typedef Widget* (*UICreateFun)(void);

#define MAKE_UI_HEADER(CLASS, BASE)                                     \
    typedef BASE Base;                                                  \
    static Widget * createSelf(){ return new CLASS(); }                 \
    static const tchar * getTypeS() { return _T(#CLASS); }              \
    virtual const tchar * getType(void) const { return _T(#CLASS); }    \
    virtual object createScriptSelf();

    typedef RefPtr<class ITextSprite> TextSpritePtr;


    class GUIMgr;
    LZUI_API GUIMgr* getGUIMgr(void); ///获得gui管理器


    void setDefaultFont(const tstring & font);
    const tstring & getDefaultFont();
    FontPtr getDefaultFontPtr();


    /** 按钮状态*/
    namespace ButtonState
    {
        const uint32 normal = 0;      //< 正常
        const uint32 active = 1;      //< 活动
        const uint32 down = 2;      //< 按下
        const uint32 disable = 3;      //< 禁用
        const uint32 hot = 4;      //< 热点
    }

    ///ui消息类型。
    namespace GuiMsg
    {
        const uint32 mouseEnter = 7;
        const uint32 mouseLeave = 8;
        const uint32 mouseLeftDrag = 10;
        const uint32 mouseRightDrag = 11;

        const uint32 getSelected = 12;
        const uint32 lostSelected = 13;
        const uint32 getActived = 15;
        const uint32 lostActived = 16;

        const uint32 buttonClick = 20;
    }

    ///相对坐标系下使用的对齐方式
    namespace RelativeAlign
    {
        const uint32 left = 1; ///<左对齐
        const uint32 hcenter = 2; ///<水平居中
        const uint32 right = 4; ///<右对齐
        const uint32 top = 8; ///<顶对齐
        const uint32 vcenter = 16; ///<垂直居中
        const uint32 bottom = 32; ///<底对齐
        const uint32 center = hcenter | vcenter;///< 完全居中
    }


    //////////////////////////////////////////////////////////////////////////

    class IUIRender
    {
    public:
        virtual bool isClipEnable() const = 0;
        virtual void setClipEnalbe(bool enalbe) = 0;

        virtual void getClipRect(CRect & rect) const = 0;
        virtual void setClipRect(const CRect & rect) = 0;

        virtual void renderBegin() = 0;
        virtual void renderEnd() = 0;

        virtual void drawRect(const CRect & rc, uint32 color) = 0;
        virtual void drawRect(const CRect & rc, uint32 color, TexturePtr texture) = 0;
        virtual void drawRect(const CRect & rc, const UVRect & srcRc, uint32 color, TexturePtr texture) = 0;
        virtual void drawWord(const CRect & dest, const UVRect & src, uint32 color, TexturePtr texture) = 0;

        virtual void drawRectFrame(const CRect & rc, int edgeSize, uint32 color) = 0;
    };


    //////////////////////////////////////////////////////////////////////////

    typedef RefPtr<class ISprite> SpritePtr;

    class ISprite : public IBase
    {
    public:
        ISprite();
        virtual ~ISprite();

        virtual void update(float elapse) = 0;
        virtual void render(IUIRender * pDevice, const CPoint & world) = 0;

        virtual void setPosition(int x, int y) { m_position.set(x, y); }
        const CPoint & getPosition() const { return m_position; }

        virtual void setSize(int w, int h) { m_size.set(w, h); }
        const CSize & getSize() const { return m_size; }

        void getRect(CRect & rc) const;
        CRect getRect() const;

    protected:
        CPoint m_position;
        CSize  m_size;
    };


    //////////////////////////////////////////////////////////////////////////

    /** 颜色渐变。
    给定当前目标颜色、渐变时间，程序会在每次update时重新计算当前颜色，通过getCurColor方法，
    取得当前颜色。
    渐变速度：vi = (di-si)/t。每次渐变增量为dc = vi*elapse。
    */
    class LZUI_API CColorFade
    {
    public:
        /** 构造函数*/
        CColorFade(void);

        /** 重置参数*/
        void reset(COLORREF crCur, COLORREF crDest, float time = -1.0f);

        /** 渐变。*/
        void update(float fElapse);

        /** 设置目标颜色。每次调用此函数的时候，会重新计算渐变速度，重置参数。*/
        void setDestColor(COLORREF cr);

        /** 获得当前颜色。该颜色为所需的渐变色。*/
        COLORREF getCurColor(void) { return m_crCur; }

        ///获得当前颜色
        void getCurColor(COLORREF cr) { m_crCur = cr; }

    private:
        COLORREF    m_crDest;   //< 目标颜色
        COLORREF    m_crCur;    //< 当前颜色
        float       m_time;     //< 渐变所用时间
        float       m_elapse;   //< 已用时间
        float       m_v[3];     //< rgb分量渐变速度。
    };

    class LZUI_API CPosFade
    {
    public:
        CPosFade(void);

        void reset(CPoint cur, CPoint dest, float speed = -1.0f);

        void setDest(CPoint dest, float speed = -1.0f);

        void update(float fElapse);

        void setCur(CPoint cur) { m_cur = cur; }
        void setSpeed(float v) { m_speed = v; }

        CPoint getCur(void) { return m_cur; }
        int fadeOver(void) { return m_fadeOver; }
    private:
        CPoint  m_dest;
        CPoint  m_cur;
        float   m_speed;
        bool    m_dirvx; //速度方向。用于标记移动是否结束
        bool    m_dirvy;
        float   m_vx;
        float   m_vy;
        int     m_fadeOver; //渐变完毕。-1：未完，0：刚好完毕，1：完毕
    };


    class WidgetChildren : public VisitPool<WidgetPtr>
    {
    public:
        WidgetChildren(Widget *pOwner);

        void update(float elapse);
        void render(IUIRender * pDevice);

    private:
        Widget *m_pOwner;
    };

    namespace misc
    {
        bool readPosition(CPoint & pos, LZDataPtr ptr, const tstring & tag);
        bool writePosition(const CPoint & pos, LZDataPtr ptr, const tstring & tag);

        bool readSize(CSize & pos, LZDataPtr ptr, const tstring & tag);
        bool writeSize(const CSize & pos, LZDataPtr ptr, const tstring & tag);

        bool readVector2(Vector2 & vec, LZDataPtr ptr, const tstring & tag);
        bool writeVector2(Vector2 & vec, LZDataPtr ptr, const tstring & tag);
    }


}//end namespace Lazy

