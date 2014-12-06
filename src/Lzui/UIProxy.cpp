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

    bool UIProxy::setProperty(LZDataPtr config, const tstring & key, LZDataPtr val)
    {
        if (key == L"hostFile")
            loadHost(val->asString());
        else
            return Widget::setProperty(config, key, val);

        return true;
    }

    bool UIProxy::loadHost(const tstring & layoutFile)
    {
        m_layoutFile = layoutFile;
        m_host = loadUIFromFile(layoutFile);

        return m_host != nullptr;
    }

}