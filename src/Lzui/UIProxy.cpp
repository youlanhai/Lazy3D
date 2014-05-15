#include "stdafx.h"
#include "UIProxy.h"

namespace Lazy
{

    UIProxy::UIProxy()
    {
    }


    UIProxy::~UIProxy()
    {
    }

    bool UIProxy::loadFromStream(LZDataPtr config)
    {
        if (!IControl::loadFromStream(config)) return false;

        if (!loadHost(config->readString(L"hostFile"))) return false;

        return true;
    }

    bool UIProxy::saveToStream(LZDataPtr config)
    {
        if (!m_layoutFile.empty())
            config->writeString(L"hostFile", m_layoutFile);

        return saveToStream(config);
    }

    bool UIProxy::loadHost(const tstring & layoutFile)
    {
        m_layoutFile = layoutFile;
        m_host = loadUIFromFile(layoutFile);

        return bool(m_host);
    }

}