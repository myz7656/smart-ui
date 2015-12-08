#include "base/locker.h"

namespace base
{
    CriticalSectionMutex::CriticalSectionMutex()
    {
        ::InitializeCriticalSection(&critical_section_);
    }

    CriticalSectionMutex::~CriticalSectionMutex()
    {
        ::DeleteCriticalSection(&critical_section_);
    }

    void CriticalSectionMutex::Acquire()
    {
        ::EnterCriticalSection(&critical_section_);
    }

    void CriticalSectionMutex::Release()
    {
        ::LeaveCriticalSection(&critical_section_);
    }


    SpinMutex::SpinMutex()
        : locked_(0L) {}

    SpinMutex::~SpinMutex() {}

    void SpinMutex::Acquire()
    {
        while (InterlockedExchange(&locked_, 1L) == 1L)
            Sleep(0);
    }

    void SpinMutex::Release()
    {
        InterlockedExchange(&locked_, 0L);
    }
}