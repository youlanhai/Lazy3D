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

    void UIProxy::loadFromStream(LZDataPtr config)
    {
        IControl::loadFromStream(config);

        loadHost(config->readString(L"hostFile"));
    }

    void UIProxy::saveToStream(LZDataPtr config)
    {
        if (!m_layoutFile.empty())
            config->writeString(L"hostFile", m_layoutFile);

        saveToStream(config);
    }

    bool UIProxy::loadHost(const tstring & layoutFile)
    {
        m_layoutFile = layoutFile;
        m_host = loadUIFromFile(layoutFile);

        return bool(m_host);
    }

}