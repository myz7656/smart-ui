#ifndef __base_task_executor_hpp__
#define __base_task_executor_hpp__

#include "base/task_executor.h"

#include <iostream>

namespace base
{
    template<typename Factory,
             typename Mutex,
             template<typename, typename> class Container>
    TaskExecutor<Factory, Mutex, Container>::TaskExecutor()
        : max_count_(DEFAULT_MAX_THREAD_COUNT)
        , keep_alive_time_(DEFAULT_KEEP_THREAD_ALIVE_TIME)
        , current_thread_id_(0L)
        , state_(THREAD_POOL_RUNNING)
    {
        idle_event_.Set();
    }

    template<typename Factory,
             typename Mutex,
             template<typename, typename> class Container>
    TaskExecutor<Factory, Mutex, Container>::TaskExecutor(int max_count)
        : max_count_(max_count)
        , keep_alive_time_(DEFAULT_KEEP_THREAD_ALIVE_TIME)
        , current_thread_id_(0L)
        , state_(THREAD_POOL_RUNNING)
    {
        idle_event_.Set();
    }

    template<typename Factory,
             typename Mutex,
             template<typename, typename> class Container>
    TaskExecutor<Factory, Mutex, Container>::TaskExecutor(int max_count, int keep_alive_time)
        : max_count_(max_count)
        , keep_alive_time_(keep_alive_time)
        , current_thread_id_(0L)
        , state_(THREAD_POOL_RUNNING)
    {
        idle_event_.Set();
    }

    template<typename Factory,
             typename Mutex,
             template<typename, typename> class Container>
    TaskExecutor<Factory, Mutex, Container>::~TaskExecutor() {}

    template<typename Factory,
             typename Mutex,
             template<typename, typename> class Container>
    bool TaskExecutor<Factory, Mutex, Container>::Execute(Task* task)
    {
        if (!task || GetThreadPoolState() != THREAD_POOL_RUNNING)
        {
            return false;
        }

        thread_mutex_.Acquire();

        task_list_.push_back(task);
        task_event_.Set();

        int count = thread_pool_.size();
        bool idle = HasIdleThread();

        thread_mutex_.Release();
        if (!idle && count < max_count_)
        {
            StartNewThread();
        }

        idle_event_.Reset();

        return true;
    }

    template<typename Factory,
             typename Mutex,
             template<typename T, typename Allocator = std::allocator<T> > class Container>
    bool TaskExecutor<Factory, Mutex, Container>::Cancel(Task* task)
    {
        if (!task)
        {
            return false;
        }

        ThreadGuard<Mutex> guard(thread_mutex_);
        for (Container<Task*>::iterator iter = task_list_.begin(); iter != task_list_.end(); ++iter)
        {
            if (*iter == task)
            {
                task_list_.erase(iter);
                return true;
            }
        }

        if (IsIdle())
        {
            idle_event_.Set();
        }

        return false;
    }

    template<typename Factory,
             typename Mutex,
             template<typename T, typename Allocator = std::allocator<T> > class Container>
    void TaskExecutor<Factory, Mutex, Container>::Clear()
    {
        ThreadGuard<Mutex> guard(thread_mutex_);
        for (Container<Task*>::iterator iter = task_list_.begin(); iter != task_list_.end(); ++iter)
        {
            delete *iter;
        }
        task_list_.clear();

        if (IsIdle())
        {
            idle_event_.Set();
        }
    }

    template<typename Factory,
             typename Mutex,
             template<typename, typename> class Container>
    void TaskExecutor<Factory, Mutex, Container>::Shutdown(DWORD timeout)
    {
        SetThreadPoolState(THREAD_POOL_STOP);
        Clear();
        StopAllThread(timeout);
        SetThreadPoolState(THREAD_POOL_TERMINATED);
    }

    template<typename Factory,
             typename Mutex,
             template<typename, typename> class Container>
    void TaskExecutor<Factory, Mutex, Container>::StopAllThread(DWORD timeout)
    {
        std::map<LONG, ThreadContext*> temp;
        {
            ThreadGuard<Mutex> guard(thread_mutex_);
            temp.swap(thread_pool_);
        }

        for (std::map<LONG, ThreadContext*>::iterator iter = temp.begin(); iter != temp.end(); ++iter)
        {
            Factory::DestroyThread(iter->second->handler, timeout);
            delete iter->second;
        }

        if (IsIdle())
        {
            idle_event_.Set();
        }
    }

    template<typename Factory,
             typename Mutex,
             template<typename, typename> class Container>
    void TaskExecutor<Factory, Mutex, Container>::SetThreadPoolState(DWORD state)
    {
        ::InterlockedExchange(&state_, state);
    }

    template<typename Factory,
             typename Mutex,
             template<typename, typename> class Container>
    DWORD TaskExecutor<Factory, Mutex, Container>::GetThreadPoolState()
    {
        return ::InterlockedExchangeAdd(&state_, 0L);
    }

    template<typename Factory,
             typename Mutex,
             template<typename, typename> class Container>
    Task* TaskExecutor<Factory, Mutex, Container>::GetTask()
    {
        Task* task = 0;
        ThreadGuard<Mutex> guard(thread_mutex_);
        if (task_list_.size() > 0)
        {
            task = task_list_.front();
            task_list_.pop_front();
        }
        if (!task)
        {
            task_event_.Reset();
        }
        return task;
    }

    template<typename Factory,
             typename Mutex,
             template<typename, typename> class Container>
    LONG TaskExecutor<Factory, Mutex, Container>::GetNextThreadId()
    {
        return ::InterlockedIncrement(&current_thread_id_);
    }

    template<typename Factory,
             typename Mutex,
             template<typename, typename> class Container>
    void TaskExecutor<Factory, Mutex, Container>::SetThreadState(LONG id, DWORD state)
    {
        ThreadGuard<Mutex> guard(thread_mutex_);
        std::map<LONG, ThreadContext*>::iterator iter = thread_pool_.find(id);
        if (iter != thread_pool_.end())
        {
            iter->second->state = state;
        }
    }

    template<typename Factory,
             typename Mutex,
             template<typename, typename> class Container>
    void TaskExecutor<Factory, Mutex, Container>::StartNewThread()
    {
        ThreadContext* context = new ThreadContext;
        if (!context)
        {
            return;
        }

        context->executor = this;
        context->id = GetNextThreadId();
        context->state = THREAD_CREATED;

        context->handler = Factory::CreateThread(&TaskExecutor::TaskThread, (void*)context, CREATE_SUSPENDED);
        if (context->handler)
        {
            thread_mutex_.Acquire();
            thread_pool_[context->id] = context;
            thread_mutex_.Release();

            ::ResumeThread(context->handler);
        }
        else
        {
            delete context;
        }
    }

    template<typename Factory,
             typename Mutex,
             template<typename, typename> class Container>
    void TaskExecutor<Factory, Mutex, Container>::RemoveThread(LONG id)
    {
        ThreadContext* context = 0;
        {
            ThreadGuard<Mutex> guard(thread_mutex_);
            std::map<LONG, ThreadContext*>::iterator iter = thread_pool_.find(id);
            if (iter != thread_pool_.end())
            {
                context = iter->second;
                thread_pool_.erase(iter);
            }
        }
        if (context)
        {
            ::CloseHandle(context->handler);
            delete context;
        }

        if (IsIdle())
        {
            idle_event_.Set();
        }
    }

    template<typename Factory,
             typename Mutex,
             template<typename, typename> class Container>
    bool TaskExecutor<Factory, Mutex, Container>::HasIdleThread()
    {
        int count = 0;
        ThreadGuard<Mutex> guard(thread_mutex_);
        for (std::map<LONG, ThreadContext*>::iterator iter = thread_pool_.begin();
                    iter != thread_pool_.end(); ++iter)
        {
            if (iter->second->state == THREAD_WAITING)
            {
                ++count;
            }
        }
        return count > 0;
    }

    template<typename Factory,
             typename Mutex,
             template<typename, typename> class Container>
    unsigned TaskExecutor<Factory, Mutex, Container>::TaskThread(void* param)
    {
        ThreadContext* context = (ThreadContext*)param;
        if (!context)
        {
            return 0;
        }

        TaskExecutor* executor = context->executor;
        LONG id = context->id;

        executor->TaskThreadImpl(id);

        return 0;
    }

    template<typename Factory,
             typename Mutex,
             template<typename, typename> class Container>
    void TaskExecutor<Factory, Mutex, Container>::TaskThreadImpl(LONG id)
    {
        SetThreadState(id, THREAD_RUNNING);
        do
        {
            Task* task = GetTask();
            if (task)
            {
                task->Run();
                delete task;
            }
            else
            {
                SetThreadState(id, THREAD_WAITING);

                if (WAIT_TIMEOUT == task_event_.Wait(keep_alive_time_))
                {
                    break;
                }
            }

        } while (true);

        SetThreadState(id, THREAD_TERMINATED);

        RemoveThread(id);
    }

    template<typename Factory,
             typename Mutex,
             template<typename, typename> class Container>
    bool TaskExecutor<Factory, Mutex, Container>::IsIdle()
    {
        if (GetThreadPoolState() != THREAD_POOL_RUNNING)
        {
            return true;
        }

        ThreadGuard<Mutex> guard(thread_mutex_);
        if (task_list_.size() > 0)
        {
            return false;
        }

        for (std::map<LONG, ThreadContext*>::iterator iter = thread_pool_.begin();
            iter != thread_pool_.end(); ++iter)
        {
            if (iter->second->state == THREAD_RUNNING)
            {
                return false;
            }
        }

        return true;
    }

    template<typename Factory,
             typename Mutex,
             template<typename, typename> class Container>
    DWORD TaskExecutor<Factory, Mutex, Container>::WaitIdle(DWORD timeout)
    {
        return idle_event_.Wait(timeout);
    }
}

#endif