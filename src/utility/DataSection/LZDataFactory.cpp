#include "StdAfx.h"
#include "LZDataFactory.h"
#include "lzd.h"
#include "xml.h"
#include "../FileTool.h"
#include "../ZLock.h"
#include "../Log.h"

namespace Lazy
{

    LZDataFactory::LZDataFactory(void)
    {
    }

    LZDataFactory::~LZDataFactory(void)
    {
    }

    /*static*/ LZDataPtr LZDataFactory::create(DataType type)
    {
        if (type == DataType::Lzd) return new lzd();
        else if (type == DataType::Xml) return new xml();
        return NULL;
    }


    namespace
    {
        typedef std::map < tstring, LZDataPtr> ConfigCache;
        static ConfigCache g_cache;

#ifdef USE_MULTI_THREAD
        ///如果开启了多线程加载。必须对容器m_resourses进行加锁。
        Lazy::ZCritical     g_cacheLocker;
#else
        Lazy::DummyLock     g_cacheLocker;
#endif

        DataType guessTypeByName(const tstring & name)
        {
            DataType dt = DataType::Default;

            size_t i = name.length();
            if (i >= 4)
            {
                i -= 4;
                if (0 == lzstricmp(&name[i], _T(".lzd")))
                    dt = DataType::Lzd;
                else if (0 == lzstricmp(&name[i], _T(".xml")))
                    dt = DataType::Xml;
            }

            return dt;
        }

    }

    void clearSectionCache()
    {
        g_cacheLocker.lock();

        g_cache.clear();

        g_cacheLocker.unlock();
    }

    LZDataPtr openSection(const tstring & file, bool createIfMiss, DataType type)
    {
        ZLockHolder holder(&g_cacheLocker);

        ConfigCache::iterator it = g_cache.find(file);
        bool ret = (it != g_cache.end());

        if(ret)return it->second;

        if (type == DataType::Default)
        {
            type = guessTypeByName(file);
            if (type == DataType::Default)
            {
                LOG_ERROR(L"Can't guess a type of this filename:%s", file.c_str());
                return nullptr;
            }
        }

        LZDataPtr root = LZDataFactory::create(type);

        if (!root->load(file) && !createIfMiss) root = nullptr;

        g_cache[file] = root;
        return root;
    }

    bool saveSection(LZDataPtr root, const tstring & file)
    {
        ZLockHolder holder(&g_cacheLocker);

        if (!root->save(file)) return false;

        g_cache[file] = root;
        return true;
    }

}//namespace Lazy