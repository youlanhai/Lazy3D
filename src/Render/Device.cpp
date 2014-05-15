#include "stdafx.h"
#include "Device.h"

namespace Lazy
{

    /*static*/ std::list<IDevice*> IDevice::s_devicePool;

    IDevice::IDevice()
    {
        regDevice(this);
    }


    IDevice::~IDevice()
    {
        unregDevice(this);
    }

    /*static*/ void IDevice::regDevice(IDevice *pDevice)
    {
        if (std::find(s_devicePool.begin(), s_devicePool.end(), pDevice) == s_devicePool.end())
            s_devicePool.push_back(pDevice);
    }
    /*static*/ void IDevice::unregDevice(IDevice *pDevice)
    {
        std::list<IDevice*>::iterator it = std::find(s_devicePool.begin(), s_devicePool.end(), pDevice);
        if (it != s_devicePool.end()) s_devicePool.erase(it);
    }

    /*static*/ void IDevice::lostDevice()
    {
        for (IDevice * p : s_devicePool)
        {
            if (p) p->onLostDevice();
        }
    }

    /*static*/ void IDevice::resetDevice()
    {
        for (IDevice * p : s_devicePool)
        {
            if (p) p->onResetDevice();
        }
    }

}