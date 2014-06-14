#include "stdafx.h"
#include "Device.h"

namespace Lazy
{
    /*static*/ bool IDevice::s_deviceRunning = true;
    /*static*/ std::set<IDevice*> IDevice::s_devicePool;

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
        if (!s_deviceRunning) return;

        s_devicePool.insert(pDevice);
    }

    /*static*/ void IDevice::unregDevice(IDevice *pDevice)
    {
        if (!s_deviceRunning) return;

        s_devicePool.erase(pDevice);
    }

    /*static*/ void IDevice::lostAllDevice()
    {
        for (IDevice * p : s_devicePool)
        {
            if (p) p->onLostDevice();
        }
    }

    /*static*/ void IDevice::resetAllDevice()
    {
        for (IDevice * p : s_devicePool)
        {
            if (p) p->onResetDevice();
        }
    }

    /*static*/ void IDevice::closeAllDevice()
    {
        s_deviceRunning = false;

        for (IDevice * p : s_devicePool)
        {
            if (p) p->onCloseDevice();
        }

        s_devicePool.clear();
    }

}