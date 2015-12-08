#ifndef __base_singleton_h__
#define __base_singleton_h__

#include "base/locker.h"

namespace base
{
    template<typename T>
    struct DefaultSingletonTraits
    {
        static T* Create()
        {
            return new T();
        }
        static void Destroy(T* obj)
        {
            delete obj;
        }

        const static bool register_at_exit_ = true;
    };

    template<typename T>
    struct LeakySingletonTraits: public DefaultSingletonTraits<T>
    {
        static const bool register_at_exit_ = false;
    };

    template<typename T,
             typename SingletonTraits = DefaultSingletonTraits<T>,
             typename Mutex = CriticalSectionMutex>
    class Singleton
    {
    public:
        static T& Instance()
        {
            if (!instance_)
            {
                ThreadGuard<Mutex> guard(mutex_);
                if (!instance_)
                {
                    instance_ = SingletonTraits::Create();
                    if (instance_ && SingletonTraits::register_at_exit_)
                    {
                        atexit(DestroySingleton);
                    }
                }
            }
            return *instance_;
        }

    private:
        static void DestroySingleton()
        {
            SingletonTraits::Destroy(instance_);
            instance_ = 0;
        }

    private:
        static T* instance_;
        static Mutex mutex_;

    private:
        Singleton(){}
        ~Singleton(){}

        Singleton(const Singleton&);
        Singleton& operator=(const Singleton&);
    };

    template<typename T, typename SingletonTraits, typename Mutex>
    T* Singleton<T, SingletonTraits, Mutex>::instance_ = 0;

    template<typename T, typename SingletonTraits, typename Mutex>
    Mutex Singleton<T, SingletonTraits, Mutex>::mutex_;
}

#endif