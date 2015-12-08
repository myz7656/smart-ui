#ifndef __base_task_center_h__
#define __base_task_center_h__

#include <queue>
#include <map>

#include "base/locker.h"
#include "base/task.h"
#include "base/message_pump.hpp"
#include "base/singleton.h"

namespace base
{
    template<template<typename Processor> class Pump>
    class TaskCenter
    {
    public:
        template<typename T> friend class MessagePump;

        TaskCenter();
        ~TaskCenter();

        int  Run();
        bool Quit(int code);

        LONG PostTask(Task* task);
        LONG PostDelayTask(Task* task, unsigned delay_time, bool immortal = false);

        void DiscardTask(LONG id);

    private:
        void DoTask();
        void DoDelayTask(unsigned* next_delay_time);

    private:
        struct PendingTask
        {
            PendingTask() {}
            PendingTask(Task* task, unsigned delay_time, bool immortal, LONG task_id)
                : task_(task)
                , time_delay_(delay_time)
                , immortal_(immortal)
                , task_id_(task_id)
                , time_post_(TimeTicks::Now()) {}

            ~PendingTask() {}

            bool operator< (const PendingTask& task) const
            {
                return task.TimeDeal() < this->TimeDeal();
            }

            __int64 TimePost() const { return time_post_; }
            __int64 TimeDeal() const { return time_post_ + time_delay_ * TimeTicks::MICROSECONDS_PER_MILLISECOND; }

            Task*    task_;
            unsigned time_delay_;
            bool     immortal_;
            LONG     task_id_;

            __int64 time_post_;
        };

        LONG AddToTaskQueue(Task* task);
        LONG AddToDelayTaskQueue(Task* task, unsigned delay_time, bool immortal, LONG id = 0L);
        void ReloadDelayTaskQueue(std::queue<PendingTask>& tasks);
        unsigned GetNextDelayTime();

        bool DiscardTasks();
        bool DiscardDelayTasks();

        void RunTask(PendingTask pending_task);
        bool IsDiscardedTask(PendingTask& pending_task);
        void Discard(PendingTask& pending_task);

        LONG GetState();
        void SetState(LONG state);

    private:
        CriticalSectionMutex             mutex_;
        std::queue<PendingTask>          task_queue_;
        std::priority_queue<PendingTask> delay_task_queue_;
        std::map<LONG, LONG>             discard_queue_;

        enum State
        {
            STATE_DEFAULT = 0L,
            STATE_RUNNING = 1L,
            STATE_STOPED  = 2L
        };
        LONG                       run_state_;
        Pump<TaskCenter>           pump_;

        LONG                       task_id_;
    };

    typedef TaskCenter<MessagePump> TaskCenterUI;
}

#endif