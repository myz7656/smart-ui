#ifndef __base_message_pump_h__
#define __base_message_pump_h__

#include <windows.h>

#include "base/def.h"

namespace base
{
    template<typename Processor>
    class MessagePump
    {
    public:
        MessagePump();
        ~MessagePump();

        int  Run(Processor* processor);
        void Quit(int code);

        bool ScheduleTask();
        bool ScheduleDelayTask(unsigned delay);

    private:
        static LRESULT CALLBACK WndProcThunk(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
        void InitMessageWnd();
        void UnInitMessageWnd();

        void RunLoop();
        bool ProcessNextWindowMessage();
        bool ProcessWindowMessage(const MSG& msg);
        void DispatchWindowMessage(const MSG& msg);

        void WillProcessMessage(const MSG& msg);
        void DidProcessMessage(const MSG& msg);

        void HandleTaskMessage();
        void HandleTimerMessage();

    private:
        struct RunState
        {
            Processor* processor;

            bool should_quit;
            int code;
        };

        RunState state_;
        HWND     message_hwnd_;
        LONG     have_task_;
        LONG     have_timer_task_;

    private:
        DISABLE_COPY_AND_ASSIGN(MessagePump)
    };
}

#endif