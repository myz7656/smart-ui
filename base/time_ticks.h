#ifndef __base_time_ticks_h__
#define __base_time_ticks_h__

#include <windows.h>
#include <math.h>

#include "base/singleton.h"

namespace base
{
    class TimeTicks
    {
    public:
        static const __int64 MILLISECONDS_PER_SECOND = 1000;
        static const __int64 MICROSECONDS_PER_MILLISECOND = 1000;
        static const __int64 MICROSECONDS_PER_SECOND = MICROSECONDS_PER_MILLISECOND * MILLISECONDS_PER_SECOND;

        TimeTicks() {}

        ~TimeTicks() {}

        static __int64 Now()
        {
            LARGE_INTEGER qfc;
            QueryPerformanceCounter(&qfc);

            __int64 frequency = Frequency();

            qfc.QuadPart *= MICROSECONDS_PER_SECOND;
            qfc.QuadPart /= frequency;

            return qfc.QuadPart;
        }

        static __int64 Ticks()
        {
            LARGE_INTEGER qfc;
            QueryPerformanceCounter(&qfc);

            return qfc.QuadPart;
        }

        static __int64 Frequency()
        {
            LARGE_INTEGER frequency;
            QueryPerformanceFrequency(&frequency);

            return frequency.QuadPart;
        }

        static __int64 MicrosecondToMillisecond(__int64 microsecond)
        {
            double millis = ((double)microsecond / (double)MICROSECONDS_PER_MILLISECOND);
            millis = ceil(millis);
            return static_cast<__int64>(millis);
        }

        static __int64 MillisecondToMicrosecond(__int64 millisecond)
        {
            return static_cast<__int64>(millisecond * MICROSECONDS_PER_MILLISECOND);
        }
    };
}

#endif