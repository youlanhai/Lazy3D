#pragma once

namespace Lazy
{
    class PyLazyData : public PyBase
    {
        LZPY_DEF(PyLazyData);

    public:
        PyLazyData();
        ~PyLazyData();

        LZPY_DEF_METHOD(newOne);

        LZPY_DEF_METHOD(read);
        LZPY_DEF_METHOD(write);

        LZPY_DEF_METHOD(addChild);
        LZPY_DEF_METHOD(getChildren);
        LZPY_DEF_METHOD(getFirstChild);

        LZPY_DEF_METHOD(load);
        LZPY_DEF_METHOD(save);

        LZPY_DEF_METHOD(getTag);
        LZPY_DEF_METHOD(setTag);

        LZPY_DEF_METHOD(getValue);
        LZPY_DEF_METHOD(setValue);

        LZPY_DEF_METHOD(readBool);
        LZPY_DEF_METHOD(writeBool);

        LZPY_DEF_METHOD(readInt);
        LZPY_DEF_METHOD(writeInt);

        LZPY_DEF_METHOD(readHex);
        LZPY_DEF_METHOD(writeHex);

        LZPY_DEF_METHOD(readFloat);
        LZPY_DEF_METHOD(writeFloat);

        LZPY_DEF_METHOD(readString);
        LZPY_DEF_METHOD(writeString);

        LZDataPtr m_data;
    };

    object make_object(LZDataBase * ptr);
    bool parse_object(LZDataBase *& ptr, object o);

    object make_object(LZDataPtr ptr);
    bool parse_object(LZDataPtr & ptr, object o);

}//end namespace Lazy