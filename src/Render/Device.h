#pragma once

namespace Lazy
{

    /** dx设备。当dx设备丢失时，会调用onLostDevice。设备重置之后，调用onResetDevice。
     *  派生类需要实现这两个接口。
     *  当派生类对象实例化之后，会自动注册到设备列表；
     *  对象析构之后，会自动从设备列表中移除。
     */
    class IDevice
    {
    public:
        IDevice();
        virtual ~IDevice();

        /** 渲染设备创建时回调*/
        virtual void onCreateDevice() {}

        /** 渲染设备关闭时回调。需要在此回调中释放所有（包括Managed）显存资源。*/
        virtual void onCloseDevice() {}

        /** 渲染设备丢失回调*/
        virtual void onLostDevice() = 0;

        /** 渲染设备重置后回调*/
        virtual void onResetDevice() = 0;
    };


    /** 设备管理器*/
    class DeviceMgr : public Singleton<DeviceMgr>
    {
    public:
        DeviceMgr();
        ~DeviceMgr();

        /** 渲染设备创建时回调*/
        void onCreateDevice();

        /** 渲染设备关闭时回调。需要在此回调中释放所有（包括Managed）显存资源。*/
        void onCloseDevice();

        /** 渲染设备丢失回调*/
        void onLostDevice();

        /** 渲染设备重置后回调*/
        void onResetDevice();

    private:
        std::set<IDevice*> m_devices;

        friend class IDevice;
    };
}
