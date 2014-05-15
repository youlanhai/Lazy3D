#include "stdafx.h"
#include "Snow.h"
#include "TerrinMap.h"

cSnow::cSnow(int maxAmount, const std::wstring & texture, I3DObject* source)
    : cParticleSystem(maxAmount, texture)
    , m_pSource(source)
    , m_rangeMin(-1000.0f, 100.0f, -1000.0f)
    , m_rangeMax(1000.0f, 500.0f, 1000.0f)
{
    m_Size = 5.0f;
    m_visible = true;
}

cSnow::~cSnow(void)
{
}

void cSnow::setRange(const Math::Vector3 & min_, const Math::Vector3 & max_)
{
    m_rangeMin = min_;
    m_rangeMax = max_;
}

///重置一个粒子的属性
void cSnow::resetParticle(Attribute *attribute) 
{
    attribute->live = true;
    attribute->age = 0.0f;
    attribute->lifeTime = 1000.0f;
    D3DXVECTOR3 pos(0, 1000, 0); 
    if (m_pSource)
    {
        pos = m_pSource->getPos();
    }
    attribute->position = pos + getRandomVector(&m_rangeMin, &m_rangeMax);

    attribute->speed.x = getRandomFloat(-20.0f, -20.0f);
    attribute->speed.y = getRandomFloat(-50.0f, -150.0f);
    attribute->speed.z = getRandomFloat(-20.0f, -20.0f);

    attribute->color = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
}

void cSnow::updateParticle(Attribute *pAttr, float fElapse) 
{
    pAttr->position += pAttr->speed * fElapse;

    TerrainMap *pMap = TerrainMap::instance();
    if (pMap->isUserfull())
    {
        if (pAttr->position.y < pMap->getHeight(pAttr->position.x, pAttr->position.z))
        {
            resetParticle(pAttr);
        }
    }
    else
    {
        if(pAttr->position.y < -1000.0f)
        {
            resetParticle(pAttr);
        }
    }
}

void cSnow::setRenderState(IDirect3DDevice9 *pDevice)
{
    __super::setRenderState(pDevice);
    pDevice->SetRenderState(D3DRS_POINTSIZE_MAX, floatToDWORD(30.0f));
}

void cSnow::render(IDirect3DDevice9 *pDevice)
{
    if (!m_visible)
    {
        return;
    }
    __super::render(pDevice);
}

void cSnow::update(float fElapse)
{
    if (!m_visible)
    {
        return;
    }
    __super::update(fElapse);
}