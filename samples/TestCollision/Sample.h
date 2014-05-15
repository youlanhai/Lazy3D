#pragma once
#include "..\lz3dengine\renderobj.h"

class Sample : public IRenderObj
{
public:
    Sample(void);
    ~Sample(void);

    virtual void update(float fElapse);
    virtual void render(IDirect3DDevice9 * pDevice);

};
