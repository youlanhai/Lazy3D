#include "stdafx.h"
#include "Device.h"

namespace Lazy
{
    IDevice::IDevice()
    {
        DeviceMgr::instance()->m_devices.insert(this);
    }


    IDevice::~IDevice()
    {
        if (DeviceMgr::hasInstance())
            DeviceMgr::instance()->m_devices.erase(this);
    }

    //////////////////////////////////////////////////////////////////////
    ///
    //////////////////////////////////////////////////////////////////////
    IMPLEMENT_SINGLETON(DeviceMgr);

    DeviceMgr::DeviceMgr()
    {}

    DeviceMgr::~DeviceMgr()
    {}

    void DeviceMgr::onCreateDevice()
    {
        for (IDevice * p : m_devices)
            p->onCreateDevice();
    }

    void DeviceMgr::onCloseDevice()
    {
        for (IDevice * p : m_devices)
            p->onCloseDevice();
    }

    void DeviceMgr::onLostDevice()
    {
        for (IDevice * p : m_devices)
            p->onLostDevice();
    }

    void DeviceMgr::onResetDevice()
    {
        for (IDevice * p : m_devices)
            p->onResetDevice();
    }
}