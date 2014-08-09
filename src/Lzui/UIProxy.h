#pragma once

#include "UIWidget.h"

namespace Lazy
{
    ///维持了一个外部的ui
    class UIProxy : public Widget
    {
    public:
        MAKE_UI_HEADER(UIProxy);

        UIProxy();
        ~UIProxy();

        virtual void loadFromStream(LZDataPtr config) override;
        virtual void saveToStream(LZDataPtr config) override;

        bool loadHost(const tstring & layoutFile);

        Widget* getHost() const { return m_host; }

    private:
        tstring     m_layoutFile;
        Widget*  m_host;
    };

}//end namespace Lazy