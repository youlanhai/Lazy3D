#pragma once

#include "../utility/FPS.h"

namespace Lazy
{
    class Label;

    class LZDLL_API FpsRender : public Fps
    {
    public:
        FpsRender();
        ~FpsRender();

        void render();

        void show(bool show);
        bool visible() const;
        void toggle();

    private:
        Label* m_label;
    };

} // end namespace Lazy
