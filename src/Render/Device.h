#pragma once

namespace Lazy
{

    ///dx设备。当dx设备丢失时，会调用onLostDevice。设备重置之后，调用onResetDevice。
    ///派生类需要实现这两个接口。当派生类对象实例化之后，会自动注册到设备列表；对象析构之后，
    ///会自动从设备列表中移除。
    class IDevice
    {
    public:

        IDevice();
        virtual ~IDevice();

        ///设备丢失回调
        virtual void onLostDevice() = 0;

        ///设备重置后回调
        virtual void onResetDevice() = 0;


        ///注册设备
        static void regDevice(IDevice *pDevice);

        ///取消注册
        static void unregDevice(IDevice *pDevice);

    public:

        static void lostDevice();
        static void resetDevice();

    private:
        static std::list<IDevice*> s_devicePool;
    };

}