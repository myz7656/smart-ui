#ifndef __base_task_center_hpp__
#define __base_task_center_hpp__

#include <iostream>

#include "base/task_center.h"
#include "base/time_ticks.h"

namespace base
{
    template<template<typename Processor> class Pump>
    TaskCenter<Pump>::TaskCenter()
        : run_state_(STATE_DEFAULT)
        , task_id_(0L) {}

    template<template<typename Processor> class Pump>
    TaskCenter<Pump>::~TaskCenter()
    {
        DiscardTasks();
        DiscardDelayTasks();
    }

    template<template<typename Processor> class Pump>
    int TaskCenter<Pump>::Run()
    {
        if (GetState() != STATE_DEFAULT)
        {
            return 0;
        }

        SetState(STATE_RUNNING);

        int code = pump_.Run(this);

        SetState(STATE_STOPED);

        return code;
    }

    template<template<typename Processor> class Pump>
    bool TaskCenter<Pump>::Quit(int code)
    {
        if (GetState() == STATE_RUNNING)
        {
            pump_.Quit(code);
        }

        return true;
    }

    template<template<typename Processor> class Pump>
    LONG TaskCenter<Pump>::PostTask(Task* task)
    {
        if (GetState() == STATE_STOPED)
        {
            return 0L;
        }

        LONG id = AddToTaskQueue(task);
        if (id != 0L)
        {
            pump_.ScheduleTask();
        }

        return id;
    }

    template<template<typename Processor> class Pump>
    LONG TaskCenter<Pump>::PostDelayTask(Task* task, unsigned delay_time, bool immortal)
    {
        if (GetState() == STATE_STOPED)
        {
            return 0L;
        }

        LONG id = AddToDelayTaskQueue(task, delay_time, immortal);
        if (id != 0L)
        {
            pump_.ScheduleTask();
        }

        return id;
    }

    template<template<typename Processor> class Pump>
    void TaskCenter<Pump>::DiscardTask(LONG id)
    {
        if (id == 0L)
        {
            return;
        }

        ThreadGuard<CriticalSectionMutex> guard(mutex_);
        discard_queue_[id] = id;
    }

    template<template<typename Processor> class Pump>
    LONG TaskCenter<Pump>::AddToTaskQueue(Task* task)
    {
        if (!task)
        {
            return 0L;
        }

        ThreadGuard<CriticalSectionMutex> guard(mutex_);

        LONG id = ::InterlockedIncrement(&task_id_);
        task_queue_.push(PendingTask(task, 0, false, id));

        return id;
    }

    template<template<typename Processor> class Pump>
    LONG TaskCenter<Pump>::AddToDelayTaskQueue(Task* task, unsigned delay_time, bool immortal, LONG task_id)
    {
        if (!task)
        {
            return 0L;
        }

        ThreadGuard<CriticalSectionMutex> guard(mutex_);
        if (task_id == 0L)
        {
            task_id = ::InterlockedIncrement(&task_id_);
        }
        delay_task_queue_.push(PendingTask(task, delay_time, immortal, task_id));

        return task_id;
    }

    template<template<typename Processor> class Pump>
    void TaskCenter<Pump>::ReloadDelayTaskQueue(std::queue<PendingTask>& tasks)
    {
        ThreadGuard<CriticalSectionMutex> guard(mutex_);
        __int64 now = TimeTicks::Now();
        do 
        {
            if (delay_task_queue_.empty())
            {
                break;
            }

            PendingTask pending_task = delay_task_queue_.top();
            if (pending_task.TimeDeal() > now)
            {
                break;
            }

            delay_task_queue_.pop();
            if (IsDiscardedTask(pending_task))
            {
                Discard(pending_task);
            }
            else
            {
                tasks.push(pending_task);
            }

        } while (true);
    }

    template<template<typename Processor> class Pump>
    unsigned TaskCenter<Pump>::GetNextDelayTime()
    {
        ThreadGuard<CriticalSectionMutex> guard(mutex_);
        if (delay_task_queue_.empty())
        {
            return INFINITE;
        }

        __int64 now = TimeTicks::Now(), next = 0;

        PendingTask pending_task = delay_task_queue_.top();
        next = pending_task.TimeDeal();

        __int64 wait = 0;
        if (next > now)
        {
            wait = next - now;
        }

        return (unsigned)TimeTicks::MicrosecondToMillisecond(wait);
    }

    template<template<typename Processor> class Pump>
    LONG TaskCenter<Pump>::GetState()
    {
        return InterlockedExchangeAdd(&run_state_, 0L);
    }

    template<template<typename Processor> class Pump>
    void TaskCenter<Pump>::SetState(LONG state)
    {
        InterlockedExchange(&run_state_, state);
    }

    template<template<typename Processor> class Pump>
    bool TaskCenter<Pump>::DiscardTasks()
    {
        if (GetState() == STATE_RUNNING)
        {
            return false;
        }

        ThreadGuard<CriticalSectionMutex> guard(mutex_);
        while (!task_queue_.empty())
        {
            PendingTask pending_task = task_queue_.front();
            task_queue_.pop();

            delete pending_task.task_;
        }

        return true;
    }

    template<template<typename Processor> class Pump>
    bool TaskCenter<Pump>::DiscardDelayTasks()
    {
        if (GetState() == STATE_RUNNING)
        {
            return false;
        }

        ThreadGuard<CriticalSectionMutex> guard(mutex_);
        while (!delay_task_queue_.empty())
        {
            PendingTask pending_task = delay_task_queue_.top();
            delay_task_queue_.pop();

            delete pending_task.task_;
        }

        return true;
    }

    template<template<typename Processor> class Pump>
    void TaskCenter<Pump>::RunTask(PendingTask pending_task)
    {
        if (pending_task.task_)
        {
            pending_task.task_->Run();
            if (!pending_task.immortal_)
            {
                delete pending_task.task_;
            }
        }
    }

    template<template<typename Processor> class Pump>
    bool TaskCenter<Pump>::IsDiscardedTask(PendingTask& pending_task)
    {
        ThreadGuard<CriticalSectionMutex> guard(mutex_);
        std::map<LONG, LONG>::iterator iter = discard_queue_.find(pending_task.task_id_);
        if (iter != discard_queue_.end())
        {
            return true;
        }
        return false;
    }

    template<template<typename Processor> class Pump>
    void TaskCenter<Pump>::Discard(PendingTask& pending_task)
    {
        ThreadGuard<CriticalSectionMutex> guard(mutex_);
        std::map<LONG, LONG>::iterator iter = discard_queue_.find(pending_task.task_id_);
        if (iter != discard_queue_.end())
        {
            discard_queue_.erase(iter);
        }

        if (pending_task.task_)
        {
            delete pending_task.task_;
            pending_task.task_ = 0;
        }
    }

    template<template<typename Processor> class Pump>
    void TaskCenter<Pump>::DoTask()
    {
        std::queue<PendingTask> tasks;
        {
            ThreadGuard<CriticalSectionMutex> guard(mutex_);
            task_queue_.swap(tasks);
        }
        do
        {
            if (tasks.empty())
            {
                break;
            }

            PendingTask pending_task = tasks.front();
            tasks.pop();
            if (IsDiscardedTask(pending_task))
            {
                Discard(pending_task);
            }
            else
            {
                RunTask(pending_task);
            }

        } while (true);
    }

    template<template<typename Processor> class Pump>
    void TaskCenter<Pump>::DoDelayTask(unsigned* next_delay_time)
    {
        std::queue<PendingTask> tasks;
        ReloadDelayTaskQueue(tasks);

        do
        {
            if (tasks.empty())
            {
                break;
            }

            PendingTask pending_task = tasks.front();
            tasks.pop();

            RunTask(pending_task);

            if (pending_task.immortal_)
            {
                AddToDelayTaskQueue(pending_task.task_, pending_task.time_delay_, pending_task.immortal_, pending_task.task_id_);
            }

        } while (true);

        if (next_delay_time)
        {
            *next_delay_time = GetNextDelayTime();
        }
    }
}

#endif