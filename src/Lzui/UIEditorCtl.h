#pragma once

#include "UIWidget.h"

namespace Lazy
{
    namespace DragType
    {
        const int none   = 0;
        const int center = 1;
        const int left   = 1 << 2;
        const int top    = 1 << 3;
        const int right  = 1 << 4;
        const int bottom = 1 << 5;
    }

    ///ui编辑器用的控件
    class EditorCtl : public Widget
    {
    public:
        MAKE_UI_HEADER(EditorCtl);

        EditorCtl();
        ~EditorCtl();

        virtual void update(float fElapse) override;
        virtual void render(IUIRender * pDevice) override;

        virtual bool onEvent(const SEvent & event) override;

        virtual void onDrag(const CPoint & delta, const CPoint & point) override;

        int getEdgeSize() const { return m_edgeSize; }
        void setEdgeSize(int size) { m_edgeSize = size; }
    private:

        int m_dragType;
        int m_edgeSize;

        uint32 m_color;
        uint32 m_bgColor;
    };

}//end namespace Lazy