#pragma once

namespace Lazy
{
    ///资源类型
    namespace rt
    {
        const int texture = 1;
        const int mesh = 3;
        const int effect = 4;
    };

    ///资源接口
    class IResource : public IBase
    {
    public:
        IResource(const tstring & source);
        virtual ~IResource();

        /// 加载资源
        virtual bool load() = 0;

        ///获得资源类型
        virtual int getType(void) const = 0;

        ///资源名
        const tstring & source() const { return m_source; }

    protected:
        tstring m_source; ///< 资源名
    };

    typedef RefPtr<IResource> ResourcePtr;

    ///资源类型工厂
    struct IResourceFactory : public IBase
    {
        virtual ResourcePtr create(int type, const std::wstring & source) = 0;
    };

    typedef RefPtr<IResourceFactory> ResourceFactoryPtr;
}