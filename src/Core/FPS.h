#pragma once

#include "../utility/FPS.h"

using Lazy::Fps;

namespace Lazy
{
    class CLabel;
}

class LZDLL_API cFpsRender : public Fps
{
public:
    cFpsRender();
    ~cFpsRender();
    
    void render();

    void show(bool show);
    bool visible() const;
    void toggle();

private:
    RefPtr<Lazy::CLabel> m_label;
};
