#pragma once

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
            IF_ASSERT(s_pInstance == nullptr)
            {
                s_pInstance = new T();
                return true;
            }
            return false;
        }

        static void finiInstance()
        {
            IF_ASSERT(s_pInstance != nullptr)
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

#define DECLARE_SINGLETON(CLASS) \
    CLASS * ::Lazy::Singleton<CLASS>::s_pInstance = nullptr

    template<typename T>
    class SimpleSingleton
    {
    public:
        SimpleSingleton()
        {}

        ~SimpleSingleton()
        {}

        static T * instance()
        {
            static T s;
            return &s;
        }
    };

}// end namespace Lazy
