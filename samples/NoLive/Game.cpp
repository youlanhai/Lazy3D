//Game.cpp
#include "Game.h"

#include "SampleGun.h"
#include "PlasterGun.h"
#include "Player.h"

#include "resource.h"


#ifdef _DEBUG
#   pragma comment(lib, "../bin/LZ3DEngine_d")
#   pragma comment(lib, "../bin/LZDataLoader_d")
#else
#   pragma comment(lib, "../bin/LZ3DEngine")
#   pragma comment(lib, "../bin/LZDataLoader")
#endif
//////////////////////////////////////////////////////////////////////////


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    LZUtility::ChangeCurDirectory();

    INIT_LOG("nolive_log.txt");
    
    RefPtr<CGame> pGame = new CGame();
    if(pGame->create(hInstance,L"NoLive-LZ3DEngie        WASD移动，鼠标控制摄像机，左键射击",
        800,600,false))
    {
        pGame->mainLoop();
    }
    pGame = NULL;

    UNLOAD_LOG();
    return 0;
}


///////////////////class CGame start/////////////////////////

/*注册窗口类消息。请实现该消息来修改窗口风格。*/
void CGame::onRegisterClass(WNDCLASSEX *pwc)
{
    pwc->hIcon = LoadIcon(m_hInstance,MAKEINTRESOURCE(IDI_LZ));
    pwc->hIconSm = pwc->hIcon;
}

/*创建设备消息。实现该方法来修改设备。*/
void CGame::onCreateDevice(D3DPRESENT_PARAMETERS * pParam)
{

}

CGame::CGame()
{
	m_bGameStart=false;
}

CGame::~CGame()
{

}

void CGame::clear()
{
	WRITE_LOG("开始释放资源...");
    __super::clear();

    CBoundingBox::destoryMeshBox();

    WRITE_LOG("资源释放完毕。");
}


/*游戏初始化*/
bool CGame::init(void)
{
    WRITE_LOG("游戏资源初始化开始...");

    __super::init();
	//在此添加初始化代码
	srand(timeGetTime());

    WRITE_LOG("初始化字体...");
	m_pFont=new CFont2D();
	if(!m_pFont->initFont(m_pd3dDevice,L"宋体",15,0))
	{
		WRITE_LOG("初始化字体失败！");
	}

    m_pTexMgr->init(m_pd3dDevice, "gun/textures.dat");
	
    WRITE_LOG("初始化摄像机...");
	m_pCamera=new CCamera(CCamera::FIRST);
	m_pCamera->setPos(D3DXVECTOR3(0,100,-500));
    m_pCamera->setSpeed(200.0f,200.0f,200.0f);
    m_pCamera->setNearFar(1.0f,5000.0f);
	
    WRITE_LOG("初始化FPS系统...");
	m_pFPS=new CFPS();
	m_pFPS->init();

    WRITE_LOG("初始化键盘...");
	m_pKeyboard=new CKeyboard();


    WRITE_LOG("加载配置文件gun/gun.dat，初始化枪支mesh管理器...");
    if(!IGun::initMesh(m_pd3dDevice,"gun/gun.dat"))
    {
        WRITE_LOG("初始化枪支mesh管理器失败！配置文件为gun/gun.dat 。");
        return false;
    }

    WRITE_LOG("初始化地形...");
    //m_pSquare = new CSquare();
    //m_pSquare->init(m_pd3dDevice,50,50,100,100);
    m_pSquare = new CTerrain();
    if(!m_pSquare->init(m_pd3dDevice,"res/heightmap.raw",256,256,50,8.0f,false))
    {
        return false;
    }
    m_pSquare->setTexture(m_pTexMgr->get("gun/desert.bmp"));

    WRITE_LOG("初始化立方体...");
    m_pCube=new CCube();
    m_pCube->init(m_pd3dDevice,100.0f,100.0f,100.0f);

    WRITE_LOG("初始化枪支...");
    m_pGun = new CSampleGun();
    m_pGun->init(m_pd3dDevice,0,D3DXVECTOR3(2.5f, -4.0f,7.0f));//3.5f, -3.5f,5.0f
	//m_pGun = new CPlasterGun();
    //m_pGun->init(m_pd3dDevice,2,D3DXVECTOR3(3.0f, -3.5f,5.0f));

    {//玩家
        WRITE_LOG("初始化玩家...");
        m_pPlayer = new CPlayer();
        m_pPlayer->setPos(D3DXVECTOR3(0,50,-100));
        m_pPlayer->setSpeed(200.0f,200.0f,200.0f);
    }

    WRITE_LOG("初始化靶心...");
    m_pTarget = new C2DFace();
    m_pTarget->init(m_pd3dDevice,m_nWidth/2.0f,m_nHeight/2.0f,0.0f,32.0f,32.0f);


    WRITE_LOG("加载资源res/snowflake.dds，初始化雪花系统...");
    m_pSnow = new CSnow(3000,m_pSquare);
    if(!m_pSnow->Init(m_pd3dDevice,"res/snowflake.dds"))
    {
        WRITE_LOG("初始化雪花系统失败！");
        return false;
    }

    WRITE_LOG("初始化雾化系统...");
    m_pSceneFog = new CSceneFog();
    m_pSceneFog->init(m_pd3dDevice,CSceneFog::FT_LINEAR|CSceneFog::FT_PIXEL);


    WRITE_LOG("加载资源res/flare_alpha.dds，初始化粒子枪...");
    m_pParticleGun = new CParticleGun();
    m_pParticleGun->setTerrain(m_pSquare);
    if(!m_pParticleGun->Init(m_pd3dDevice,"res/flare_alpha.dds"))
    {
        WRITE_LOG("初始化粒子枪失败！");
        return false;
    }

    CBoundingBox::createMeshBox(m_pd3dDevice);

    WRITE_LOG("加载res/mesh.dat，初始化精灵管理器...");
    m_pSprManager = new CSpritManager();
    if(!m_pSprManager->init(m_pd3dDevice,"res/mesh.dat"))
    {
        WRITE_LOG("加载res/mesh.dat，初始化精灵管理器失败！");
        return false;
    }
    for (size_t i=0;i<4; ++i)
    {
        CSprit *spr = new CSprit();
        spr->init(m_pd3dDevice,i,i,m_pSprManager->getMeshManager());
        
        D3DXVECTOR3 pos;
        pos.x = i*1000.0f - 1600.0f;
        pos.z = i*500.0f;
        pos.y = m_pSquare->getPhysicalHeight(pos.x,pos.z);
        spr->setPos(pos);
        spr->setShowBox(true);

        m_pSprManager->add(spr);
            
    }

    ShowCursor(false);

    m_bGameStart=true;
    m_pd3dDevice->SetRenderState(D3DRS_AMBIENT,D3DCOLOR_XRGB(100,100,100));

    WRITE_LOG("游戏资源初始化完毕。");
	return true;
}

//更新
void CGame::update()
{
	m_pFPS->updateFPS();
	if(!m_bGameStart)
	{
		return;
    }
    //////////////////////////////////////////////////////////////////////////
    static float fOldUpdateTime=0;
    float curTime=(float)timeGetTime();
    float fElapse = (curTime - fOldUpdateTime)/1000.0f;
	if(fElapse < 0.003f)
	{
		return;
	}
    m_pKeyboard->update(fElapse);
    updateCamera(0.0f);//实时更新

	fOldUpdateTime=curTime;
    //在此添加更新代码

    __super::update();

    m_pGun->update(fElapse);
    if (m_pKeyboard->isKeyDown(VK_LBUTTON))
    {
        m_pGun->fire();
        m_pParticleGun->AddParticle();
    }
    m_pParticleGun->updateGunInfo(m_pPlayer->getLook(),m_pPlayer->getPos());
    m_pParticleGun->Update(fElapse);

    D3DXVECTOR3 pos = m_pPlayer->getPos()+m_pPlayer->getLook()*20.0f;
    //pos.y += 100.0f;
    m_pSnow->SetCenterPos(pos);
    m_pSnow->Update(fElapse);

}
void CGame::updateCamera(float fElap)
{
    float fCurTime = (float)timeGetTime();
    static float fLastTime = fCurTime;
    float fElapse = (fCurTime - fLastTime)/1000.0f;
    fLastTime = fCurTime;
    if (fElapse <= 0.0f)
    {
        fElapse = 0.0001f;
    }
    //////////////////////////////////////////////////////////////////////////
    //更新玩家
    if (m_pCamera->getCameraType() != CCamera::FREE)
    {
        //左右旋转
        if(m_pKeyboard->isKeyDown('A'))
        {
            //m_pPlayer->addUpAngle(-1.0f);
            m_pPlayer->moveRight(-fElapse);
        }
        else if(m_pKeyboard->isKeyDown('D'))
        {
            //m_pPlayer->addUpAngle(1.0f);
            m_pPlayer->moveRight(fElapse);
        }
        //前进后退
        if(m_pKeyboard->isKeyDown('W'))
        {
            m_pPlayer->moveLook(fElapse);
        }
        else if(m_pKeyboard->isKeyDown('S'))
        {
            m_pPlayer->moveLook(-fElapse);
        }

        //防止走入地下
        D3DXVECTOR3 pos = m_pPlayer->getPos();
        pos.y = 50.0f;
        m_pPlayer->setPos(pos);

        CPoint ptOld(m_nWidth/2,m_nHeight/2);
        CPoint ptCur;
        GetCursorPos(&ptCur);
        ScreenToClient(m_hWnd,&ptCur);
        float angleX = D3DX_PI/m_nWidth*0.5f;
        float angleY = D3DX_PI/m_nHeight*0.5f;
        
        m_pPlayer->addAngleY((ptCur.x-ptOld.x)*angleX);
        m_pPlayer->addAngleRight((ptCur.y-ptOld.y)*angleY);
        m_pPlayer->updateMatrix();

        ClientToScreen(m_hWnd,&ptOld);
        SetCursorPos(ptOld.x,ptOld.y);
    }

    //////////////////////////////////////////////////////////////////////////
    //更新摄像机
	if(m_pKeyboard->isKeyDown('1'))
	{
		m_pCamera->setCamareType(CCamera::FIRST);
	}
	else if(m_pKeyboard->isKeyDown('2'))
	{
		m_pCamera->setCamareType(CCamera::THIRD);
	}
	else if (m_pKeyboard->isKeyDown('3'))
	{
		m_pCamera->setCamareType(CCamera::FREE);
	}
	
	if (m_pCamera->getCameraType() == CCamera::FREE)
	{
		//左右旋转
		if(m_pKeyboard->isKeyDown('A'))
		{
			m_pCamera->rotationUp(-1.0f*fElapse);
		}
		else if(m_pKeyboard->isKeyDown('D'))
		{
			m_pCamera->rotationUp(1.0f*fElapse);
		}
		//前进后退
		if(m_pKeyboard->isKeyDown('W'))
		{
			m_pCamera->moveLook(fElapse);
		}
		else if(m_pKeyboard->isKeyDown('S'))
		{
			m_pCamera->moveLook(-fElapse);
		}
		//上下旋转
		if(m_pKeyboard->isKeyDown(VK_UP))
		{
			m_pCamera->rotationRight(-0.5f*fElapse);
		}
		else if(m_pKeyboard->isKeyDown(VK_DOWN))
		{
			m_pCamera->rotationRight(0.5f*fElapse);
		}
		//绕看的方向旋转
		if(m_pKeyboard->isKeyDown(VK_LEFT))
		{
			m_pCamera->rotationLook(0.5f*fElapse);
		}
		else if(m_pKeyboard->isKeyDown(VK_RIGHT))
		{
			m_pCamera->rotationLook(-0.5f*fElapse);
		}
	}

	//摄像机跟随
	D3DXVECTOR3 vecPos=m_pPlayer->getPos();
	float ph = 30;
	if (m_pCamera->getCameraType() == CCamera::FIRST)
    {
        vecPos.y = m_pSquare->getPhysicalHeight(vecPos.x,vecPos.z)+ph;
        m_pPlayer->setPos(vecPos);
		m_pCamera->setPos(vecPos);
		m_pCamera->setLook(m_pPlayer->getLook());
		m_pCamera->setUp(m_pPlayer->getUp());
		m_pCamera->setRight(m_pPlayer->getRight());
	}
	else if (m_pCamera->getCameraType() == CCamera::THIRD)
	{
		D3DXVECTOR3 vecLook=m_pPlayer->getLook();
		D3DXVec3Normalize(&vecLook,&vecLook);
		D3DXVECTOR3 vecCamPos=m_pPlayer->getPos()-vecLook*ph;
		vecCamPos.y += ph;
		m_pCamera->setPos(vecCamPos);
		m_pCamera->setLook(vecPos-vecCamPos);
		m_pCamera->setUp(m_pPlayer->getUp());
		m_pCamera->setRight(m_pPlayer->getRight());
	}
	else
	{

	}

}
//渲染
void CGame::render()
{
	m_pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,100),1.0f,0);
	if(SUCCEEDED(m_pd3dDevice->BeginScene()))
	{
		//添加渲染代码
		m_pCamera->set3DCamera(m_pd3dDevice);

#if 1
        CLight light;
        light.SetDirectionLight1(m_pd3dDevice,
            D3DXVECTOR3(0.0f,-1.0f,0.0f),
            D3DXCOLOR(0.8f,0.8f,0.8f,1.0f));
#else
        {
            D3DXVECTOR3 vecPos = m_pCamera->getPos();
            vecPos.y += m_pSquare->getPhysicalHeight(vecPos.x,vecPos.y)+200.0f;
            //设置灯光
		    CLight light;
		    light.SetPointLight(m_pd3dDevice,
			    vecPos,
			    D3DXCOLOR(1.0f,1.0f,1.0f,1.0f));
        }
#endif
        __super::render();

        //设置材质
        CMaterial::setMaterial(m_pd3dDevice,
            D3DXCOLOR(0.8f,0.8f,0.8f,0.8f),
            D3DXCOLOR(0.1f,0.1f,0.1f,1.0f),
            D3DXCOLOR(0.0f,0.0f,0.0f,0.0f),
            D3DXCOLOR(0.0f,0.0f,0.0f,0.0f),
            5.0f);

        m_pSceneFog->setFog();
        //渲染地面
        m_pSquare->render(m_pd3dDevice);
        m_pSceneFog->closeFog();

        //世界矩阵
        D3DXMATRIXA16 matWord;

        //设置材质
        CMaterial::setMaterial(m_pd3dDevice,
            D3DXCOLOR(0.8f,0.0f,0.8f,1.0f),
            D3DXCOLOR(0.8f,0.0f,0.8f,1.0f));

		D3DXMatrixRotationAxis(&matWord,
			&D3DXVECTOR3(1.0f,1.0f,1.0f),
			timeGetTime()/1000.0f);
		D3DXMATRIXA16 matTrans;
		D3DXMatrixTranslation(&matTrans,
            0.0f,
            m_pSquare->getPhysicalHeight(0,0)+100.0f,
            0.0f);
		matWord *= matTrans;

		m_pd3dDevice->SetTransform(D3DTS_WORLD,&matWord);
        m_pd3dDevice->SetTexture(0,0);
        //渲染立方体
		m_pCube->render(m_pd3dDevice);

        m_pSprManager->render();

        m_pPlayer->getRotationMatrix(matWord);
        m_pGun->render(m_pd3dDevice,&matWord);

        m_pParticleGun->Render(m_pd3dDevice);

        m_pSnow->Render(m_pd3dDevice);

        renderTarget();

		renderText();
		/////////////////////////
		m_pd3dDevice->EndScene();
	}
	else
	{
		WRITE_LOG("绘制场景(m_pd3dDevice->BeginScene())失败!");
	}
	m_pd3dDevice->Present(NULL,NULL,NULL,NULL);
    if(m_pFPS->getFPS()>50.0f)
    {
        Sleep(1);
    }
}

void CGame::renderText()
{
	char buffer[128];
	sprintf_s(buffer,128,"fps:%.3f",m_pFPS->getFPS());
	RECT rc;
	rc.left=rc.top=0;
	rc.right=500;
	rc.bottom=20;
	m_pFont->setRect(&rc);
	m_pFont->setColor(0xffff0000);
    m_pFont->drawText(LZUtility::CharToWChar(NULL,buffer));
}

void CGame::renderTarget()//绘制靶心
{
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);


    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

#if 0
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );

    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
#endif

    m_pTarget->render(m_pd3dDevice,m_pTexMgr->get("gun/Crosshair.dds"));

    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE);
}
///////////////////class CGame end/////////////////////////