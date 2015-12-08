#ifndef __base_event_h__
#define __base_event_h__

#include <windows.h>
#include <tchar.h>

namespace base
{
    class Event
    {
    public:
        Event()
        {
            event_ = ::CreateEvent(0, TRUE, FALSE, NULL);
        }
        Event(LPCTSTR name)
        {
            event_ = ::CreateEvent(0, TRUE, FALSE, name);
        }

        ~Event()
        {
            if (event_)
            {
                ::CloseHandle(event_);
            }
        }

        void Set()
        {
            if (event_)
            {
                ::SetEvent(event_);
            }
        }

        void Reset()
        {
            if (event_)
            {
                ::ResetEvent(event_);
            }
        }

        DWORD Wait(DWORD timeout = 0)
        {
            return ::WaitForSingleObject(event_, timeout);
        }

    private:
        HANDLE event_;
    };
}

#endif