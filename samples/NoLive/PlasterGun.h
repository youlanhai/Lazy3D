#pragma once
#include "../lz3dengine/gun.h"
#include "../LZ3DEngine/Square.h"

class CPlasterGun : public IGun
{
public:
	CPlasterGun(void);
	~CPlasterGun(void);

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
