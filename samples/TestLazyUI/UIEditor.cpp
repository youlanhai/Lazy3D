#include "stdafx.h"
#include "UIEditor.h"

namespace Lazy
{

    UIEditor::UIEditor()
    {
    }


    UIEditor::~UIEditor()
    {
    }

    bool UIEditor::init()
    {
        if (!loadFromFile(L"layout/editor/uieditor.lzd"))
            return false;

        return true;
    }
}