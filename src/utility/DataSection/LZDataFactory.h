#pragma once

#include "LZDataBase.h"

namespace Lazy
{
    ///配置文件类工厂
    class LZDataFactory
    {
    public:
        LZDataFactory();
        virtual ~LZDataFactory();

        static LZDataPtr create(DataType type);
    };

    //以下方法线程安全

    ///打开配置文件
    LZDataPtr openSection(const tstring & file, bool createIfMiss = false, DataType type = DataType::Default);

    ///保存配置文件
    bool saveSection(LZDataPtr root, const tstring & file);

    ///清除缓存的配置文件
    void clearSectionCache();


}//namespace Lazys