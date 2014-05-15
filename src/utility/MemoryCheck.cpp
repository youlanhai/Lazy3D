#include "StdAfx.h"
#include "MemoryCheck.h"
#include "Misc.h"
#include "Log.h"
#include <sstream>

namespace Lazy
{

    /////////////////////////////////////////////////////////////////////////////


    MemoryChecker * MemoryChecker::s_instance = nullptr;


    /*static*/ MemoryChecker * MemoryChecker::getInstance()
    {
        if (s_instance == nullptr)
        {
            s_instance = new MemoryChecker();
            LOG_INFO(L"MemoryChecker created.");
        }
        return s_instance;
    }

    /*static*/ void MemoryChecker::deleteInstance()
    {
        if (s_instance != nullptr)
        {
            delete s_instance;
            s_instance = nullptr;
            LOG_INFO(L"MemoryChecker deleted.");
        }
    }

    MemoryChecker::MemoryChecker()
    {
        //不要调用任何东西，容易出现递归调用
    }

    MemoryChecker::~MemoryChecker()
    {
        LOG_INFO(L"MemoryChecker will exit soon.");

        dumpAll();
    }

    void MemoryChecker::dumpAll()
    {
        std::map<std::string, int> counter;

        for (auto it : m_objects)
        {
            ++counter[it->getTypeName()];
        }


        for (auto it : counter)
        {
            std::ostringstream ss;
            ss << "object:\t" << it.second << "\t" << it.first;
            OutputDebugStringA(ss.str().c_str());
        }
    }


}//namespace Lazy