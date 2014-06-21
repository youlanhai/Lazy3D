#include "stdafx.h"
#include "DataStream.h"

namespace Lazy
{
    const size_t BasicSize = sizeof(void*) ;
    const size_t LimitSize = 1024 * 1024 * 1024;

    DataStream::DataStream()
        : pos_(0)
        , stream_(BasicSize)
    {
    }


    DataStream::~DataStream()
    {
    }

    void DataStream::clear()
    {
        pos_ = 0;
        stream_.clear();
    }

    bool DataStream::writeToFile(FILE *pFile)
    {
        assert(pFile);

        if (empty()) return false;

        return fwrite(&stream_[0], pos_, 1, pFile) == 1;
    }

    bool DataStream::readFromFile(FILE *pFile, size_t n)
    {
        assert(pFile);

        stream_.resize(pos_ + n);
        if (!fread(&stream_[pos_], n, 1, pFile) != 1) return false;

        pos_ += n;
        return true;
    }

    dschar DataStream::get()
    {
        if (empty()) return 0;
        return stream_[pos_++];
    }

    void DataStream::append(dschar ch)
    {
        if (empty()) stream_.resize(pos_ + BasicSize);
        stream_[pos_++] = ch;
    }

    void DataStream::gets(void *dest, size_t len)
    {
        if (len > LimitSize)
            throw(std::invalid_argument("the size is too large!"));

        if (stream_.size() - pos_ < len)
            throw(std::runtime_error("does't has enough stream!"));

        memcpy(dest, &stream_[pos_], len);
        pos_ += len;
    }

    void DataStream::appends(const void *src, size_t len)
    {
        if (len > LimitSize)
            throw(std::invalid_argument("the size is too large!"));

        if (stream_.size() - pos_ < len)
            stream_.resize(pos_ + len + BasicSize);
        memcpy(&stream_[pos_], src, len);
        pos_ += len;
    }

    void DataStream::skip(size_t len)
    {
        pos_ += len;

        if (pos_ >= stream_.size())
        {
            pos_ = stream_.size();
        }
    }
}