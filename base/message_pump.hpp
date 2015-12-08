#ifndef __base_message_pump_hpp__
#define __base_message_pump_hpp__

#include "base/message_pump.h"
#include "util/string.h"

namespace base
{
    static const wchar_t kWndClass[] = L"MessagePumpWindow";

    static const int WM_TASK = (WM_USER + 1);

    template<typename Processor>
    MessagePump<Processor>::MessagePump()
        : have_task_(0L)
        , have_timer_task_(0L)
    {
        InitMessageWnd();
    }

    template<typename Processor>
    MessagePump<Processor>::~MessagePump()
    {
        UnInitMessageWnd();
    }

    template<typename Processor>
    int MessagePump<Processor>::Run(Processor* processor)
    {
        state_.processor = processor;
        state_.should_quit = false;
        state_.code = 0;

        RunLoop();

        return state_.code;
    }

    template<typename Processor>
    void MessagePump<Processor>::Quit(int code)
    {
        PostQuitMessage(code);
        state_.should_quit = true;
    }

    template<typename Processor>
    bool MessagePump<Processor>::ScheduleTask()
    {
        if (InterlockedExchange(&have_task_, 1L))
        {
            return false;
        }

        PostMessage(message_hwnd_, WM_TASK, reinterpret_cast<WPARAM>(this), 0);
        return true;
    }

    template<typename Processor>
    bool MessagePump<Processor>::ScheduleDelayTask(unsigned delay)
    {
        if (InterlockedExchange(&have_timer_task_, 1L))
        {
            return false;
        }

        if (delay < USER_TIMER_MINIMUM)
        {
            delay = USER_TIMER_MINIMUM;
        }

        SetTimer(message_hwnd_, reinterpret_cast<UINT_PTR>(this), delay, NULL);
        return true;
    }

    template<typename Processor>
    void MessagePump<Processor>::InitMessageWnd()
    {
        HINSTANCE hinst = GetModuleHandle(NULL);

        WNDCLASSEX wc = { 0 };
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = WndProcThunk;
        wc.hInstance = hinst;
        wc.lpszClassName = kWndClass;
        RegisterClassEx(&wc);

        message_hwnd_ = CreateWindow(kWndClass, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, hinst, 0);
    }

    template<typename Processor>
    void MessagePump<Processor>::UnInitMessageWnd()
    {
        DestroyWindow(message_hwnd_);
        UnregisterClass(kWndClass, GetModuleHandle(NULL));
    }

    template<typename Processor>
    LRESULT CALLBACK MessagePump<Processor>::WndProcThunk(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
    {
        switch(message)
        {
        case WM_TASK:
            reinterpret_cast<MessagePump*>(wparam)->HandleTaskMessage();
            break;

        case WM_TIMER:
            reinterpret_cast<MessagePump*>(wparam)->HandleTimerMessage();
            break;
        }
        return DefWindowProc(hwnd, message, wparam, lparam);
    }

    template<typename Processor>
    void MessagePump<Processor>::RunLoop()
    {
        while (true)
        {
            bool more_work = ProcessNextWindowMessage();
            if (state_.should_quit)
            {
                break;
            }

            state_.processor->DoTask();
            if (state_.should_quit)
            {
                break;
            }

            unsigned next_delay_time = 0;
            state_.processor->DoDelayTask(&next_delay_time);
            if (state_.should_quit)
            {
                break;
            }

            if (more_work)
            {
                continue;
            }

            MsgWaitForMultipleObjectsEx(0, NULL, next_delay_time, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        }
    }

    template<typename Processor>
    bool MessagePump<Processor>::ProcessNextWindowMessage()
    {
        bool sent_messages_in_queue = false;
        DWORD queue_status = GetQueueStatus(QS_SENDMESSAGE);
        if (HIWORD(queue_status) & QS_SENDMESSAGE)
        {
            sent_messages_in_queue = true;
        }

        MSG msg;
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            return ProcessWindowMessage(msg);
        }

        return sent_messages_in_queue;
    }

    template<typename Processor>
    bool MessagePump<Processor>::ProcessWindowMessage(const MSG& msg)
    {
        if (msg.message == WM_QUIT)
        {
            state_.should_quit = true;
            state_.code = msg.wParam;
            return false;
        }

        DispatchWindowMessage(msg);
        return true;
    }

    template<typename Processor>
    void MessagePump<Processor>::DispatchWindowMessage(const MSG& msg)
    {
        WillProcessMessage(msg);

        TranslateMessage(&msg);
        DispatchMessage(&msg);

        DidProcessMessage(msg);
    }

    template<typename Processor>
    void MessagePump<Processor>::WillProcessMessage(const MSG& msg)
    {
    }

    template<typename Processor>
    void MessagePump<Processor>::DidProcessMessage(const MSG& msg)
    {
    }

    template<typename Processor>
    void MessagePump<Processor>::HandleTaskMessage()
    {
        InterlockedExchange(&have_task_, 0L);
    }

    template<typename Processor>
    void MessagePump<Processor>::HandleTimerMessage()
    {
        KillTimer(message_hwnd_, reinterpret_cast<UINT_PTR>(this));
        InterlockedExchange(&have_timer_task_, 0L);
    }
}

#endif