#ifndef __base_locker_h__
#define __base_locker_h__

#include <windows.h>

namespace base
{
    class CriticalSectionMutex
    {
    public:
        CriticalSectionMutex();
        ~CriticalSectionMutex();

        void Acquire();
        void Release();

    private:
        CRITICAL_SECTION critical_section_;
    };

    class SpinMutex
    {
    public:
        SpinMutex();
        ~SpinMutex();

        void Acquire();
        void Release();

    private:
        LONG locked_;
    };

    class EmptyMutex
    {
    public:
        EmptyMutex() {}
        ~EmptyMutex() {}

        void Acquire() {}
        void Release() {}
    };


    template<typename Mutex = CriticalSectionMutex>
    class ThreadGuard
    {
    public:
        ThreadGuard(Mutex& mutex)
            : mutex_(mutex)
        {
            mutex_.Acquire();
        }

        ~ThreadGuard()
        {
            mutex_.Release();
        }

    private:
        Mutex& mutex_;
    };
}

#endif