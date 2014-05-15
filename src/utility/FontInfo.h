#pragma once

namespace Lazy
{

    ///字体信息 将字体文件名、字体大小等信息，转换成字体名
    ///字体名称中的信息必须以#连接
    struct FontInfo
    {
        tstring name;   ///<字体名简称
        int     height; ///<字体高度
        int     bold;   ///<是否加粗

        FontInfo();

        FontInfo(const tstring & fontName);

        ///从字符串格式化
        void from(const tstring & fontName);

        ///格式化到字符串
        void to(tstring & fontName);

        ///字体信息是否合法
        bool valid() const;
    };
}

