#include "StdAfx.h"
#include "MemoryCheck.h"
#include "Misc.h"
#include "Log.h"
#include <sstream>

namespace Lazy
{

    /////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_SINGLETON(MemoryChecker);

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

        m_lock.lock();
        for (auto it : m_objects)
        {
            ++counter[it->getTypeName()];
        }
        m_lock.unlock();

        for (auto it : counter)
        {
            std::ostringstream ss;
            ss << "object:\t" << it.second << "\t" << it.first;
            OutputDebugStringA(ss.str().c_str());
        }
    }


}//namespace Lazy