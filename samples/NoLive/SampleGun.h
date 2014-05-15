#pragma once

#include "../LZ3DEngine/Gun.h"
#include "../LZ3DEngine/Square.h"

class CSampleGun : public IGun
{
public:
    CSampleGun(void);
    ~CSampleGun(void);

    bool init(IDirect3DDevice9 * pDevice,int nType,D3DXVECTOR3 & pos);
    void clear();
    void update(float fElapse);
    void render(IDirect3DDevice9 * pDevice,D3DXMATRIXA16 * matWord);
    bool canFire();
    bool isRunning(void);
    void fire() ;
private:
    CSquare m_square;
	CSquare m_target;
    
};
