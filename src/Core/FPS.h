/************************************************************************/
/*FPS.h                                                            */
/*帧频率计数器：获得游戏的帧刷新率fps。                                       */
/************************************************************************/
#pragma once

#include "../utility/FPS.h"

using Lazy::Fps;


class LZDLL_API cFpsRender : public Fps
{
public:
    cFpsRender();
    ~cFpsRender();
    
    void render();
};