
#include "stdafx.h"

#include "AnimModel.h"
#include "App.h"
#include "../utility/Misc.h"

//////////////////////////////////////////////////////////////////////////
AnimalModel::AnimalModel()
	: m_pAnimControler(NULL)
    , m_elapsedTime(0)
    , m_elapse(0)
    , m_loop(true)
    , m_speed(1.0f)
{
}

AnimalModel::~AnimalModel(void)
{
    SafeRelease(m_pAnimControler);
    m_pSkinMesh = NULL;
}

bool AnimalModel::load(const std::wstring & name)
{ 
    iModel::load(name);

    m_pSkinMesh = Lazy::SkinMeshMgr::instance()->get(name);
    if (!m_pSkinMesh)
    {
        return false;
    }

    setAABB(m_pSkinMesh->getAABB());
    m_pAnimControler = m_pSkinMesh->cloneAnimaCtrl();
    return true; 
}

/** 更新*/
void AnimalModel::update(float elapse)
{
    m_elapse = elapse*m_speed;
}

/** 渲染*/
void AnimalModel::render(IDirect3DDevice9* pDevice)
{
    if (m_pSkinMesh)
    {
        Matrix4x4 mat;
        getCombinedMatrix(mat);

        advanceAnimation();
        m_pSkinMesh->setWorldMatrix(mat);
        m_pSkinMesh->render();
    }

    iModel::render(pDevice);
}

/** 步进动画*/
void AnimalModel::advanceAnimation(void)
{
    if (m_pAnimControler)
    {
        if (m_loop)
        {
            m_pAnimControler->AdvanceTime(m_elapse, NULL);
        }
        else
        {
            LPD3DXANIMATIONSET pASTrack;
            m_pAnimControler->GetTrackAnimationSet(0, &pASTrack);
            float dt = (float)pASTrack->GetPeriod();
            //DebugMsg("(%f, %f, %lf)", m_elapsedTime, m_elapse, dt);
            if (m_elapsedTime <= dt)
            {
                if (m_elapsedTime + m_elapse > dt)
                {
                    m_elapse = dt - m_elapsedTime;
                    m_elapsedTime = dt;
                }
                else 
                {
                    m_elapsedTime += m_elapse;
                }
                m_pAnimControler->AdvanceTime(m_elapse, NULL);
            }
        }
    }
}

LPD3DXANIMATIONSET AnimalModel::getAnimSet(int n) const
{
    if (n>=0 && n<getActionCount())
    {
        LPD3DXANIMATIONSET pAnimSet = NULL;
        m_pAnimControler->GetAnimationSet(n, &pAnimSet);
        return pAnimSet;
    }
    return NULL;
}

LPD3DXANIMATIONSET AnimalModel::getAnimSet(const std::wstring & name) const
{
    LPD3DXANIMATIONSET pAnimSet = NULL;

    std::string name2;
    Lazy::wcharToChar(name2, name);
    m_pAnimControler->GetAnimationSetByName(name2.c_str(), &pAnimSet);
    return pAnimSet;
}

LPD3DXANIMATIONSET AnimalModel::findAnimSet(const std::wstring & name) const
{
    int n = getActionCount();

    std::wstring animName;
    LPD3DXANIMATIONSET pAnimSet = NULL;
    for(int i=0; i<n; ++i)
    {
        pAnimSet = getAnimSet(i);
        if (pAnimSet)
        {
            Lazy::charToWChar(animName, pAnimSet->GetName());

            if (animName.find(name) != name.npos) return pAnimSet;
        }
    }

    return NULL;
}

/** 播放动画*/
bool AnimalModel::playAction(const std::wstring & name, bool loop)
{
    //DebugMsg("playAction name=%s, loop=%d", name.c_str(), loop);
    m_loop = loop;
    return playAction(getAnimSet(name));
}

bool AnimalModel::playAction(LPD3DXANIMATIONSET pAnimSet)
{
    if (NULL == pAnimSet)
    {
        return false;
    }
    if(FAILED(m_pAnimControler->SetTrackAnimationSet(0, pAnimSet)))
    {
        return false;
    }
    m_elapsedTime = 0;
    return true;
}

/** 停止动画*/
void AnimalModel::stopAction(void)
{
    if(!playAction(findAnimSet(L"idle")))
    {
        playAction(getAnimSet(0));
    }
}

/** 获得动画个数*/
int AnimalModel::getActionCount(void) const
{
    return m_pAnimControler->GetNumAnimationSets();
}

/** 获得动画名称*/
void AnimalModel::getActionName(std::wstring & name, int index) const
{
    LPD3DXANIMATIONSET pAnimSet = getAnimSet(index);

    Lazy::charToWChar(name, pAnimSet->GetName());
}
