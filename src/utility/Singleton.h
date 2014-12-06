#pragma once

#include <cassert>

namespace Lazy
{
    template<typename T>
    class Singleton
    {
    public:
        Singleton()
        {
            assert(s_pInstance == nullptr);
        }

        ~Singleton()
        {
            s_pInstance = nullptr;
        }

        static bool initInstance()
        {
            if(s_pInstance == nullptr)
            {
                s_pInstance = new T();
                return true;
            }
            return false;
        }

        static void finiInstance()
        {
            if(s_pInstance != nullptr)
            {
                delete s_pInstance;
                s_pInstance = nullptr;
            }
        }

        static bool hasInstance()
        {
            return s_pInstance != nullptr;
        }

        static T * instance()
        {
            assert(s_pInstance);
            return s_pInstance;
        }

    private:
        static T * s_pInstance;
    };

#define IMPLEMENT_SINGLETON(CLASS) \
    CLASS * ::Lazy::Singleton<CLASS>::s_pInstance = nullptr

}// end namespace Lazy
