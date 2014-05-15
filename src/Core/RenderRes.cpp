#include "stdafx.h"
#include "RenderRes.h"

#include "../utility/Utility.h"

cRenderResMgr::cRenderResMgr()
{
    m_factory = NULL;
}

cRenderResMgr::~cRenderResMgr(void)
{
    clear();
}

bool cRenderResMgr::init(const std::wstring &)
{
    return true;
}

RenderResPtr cRenderResMgr::get(const std::wstring & fileName, int type)
{
    if (fileName.empty()) return nullptr;
    
    RenderResPtr pRes;
    bool found ;

    m_locker.lock();
    ResIterator it = m_resourses.find(fileName);
    if (it == m_resourses.end())
    {
        found = false;
        pRes = nullptr;
        m_resourses[fileName] = pRes;
    }
    else
    {
        found = true;
        pRes = it->second;
    }
    m_locker.unlock();

    if (found)  return pRes;
    
    pRes = load(fileName, type);

    m_locker.lock();
    m_resourses[fileName] = pRes;
    m_locker.unlock();

    return pRes;
}

void cRenderResMgr::remove(const std::wstring & fileName)
{
    m_locker.lock();

    ResIterator it = m_resourses.find(fileName);
    if (it != m_resourses.end())
    {
        m_resourses.erase(it);
    }

    m_locker.unlock();
}

void cRenderResMgr::clear(void)
{
    m_locker.lock();

    m_resourses.clear();

    m_locker.unlock();
}

RenderResPtr cRenderResMgr::load(const std::wstring & fname, int type)
{
    assert(m_factory != NULL && "cRenderResMgr::load");

    std::wstring realPath = fname;
    if (type != REST_FONT)
    {
        if (!Lazy::getfs()->getRealPath(realPath, fname))
        {
            LOG_ERROR(L"Resource '%s' was not found!", fname.c_str());
            return NULL;
        }
    }

    RenderResPtr pRes = m_factory->createObj(type);
    if (pRes && !pRes->load(realPath))
    {
        LOG_ERROR(L"Load resource '%s' failed!", realPath.c_str());
        return nullptr;
    }

    return pRes;
}

