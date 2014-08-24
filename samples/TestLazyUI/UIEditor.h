#pragma once

#include "Lzui/Lzui.h"

namespace Lazy
{

    class UIEditor : public Widget
    {
    public:
        MAKE_UI_HEADER(UIEditor);

        UIEditor();
        ~UIEditor();

        bool init();

    };

}