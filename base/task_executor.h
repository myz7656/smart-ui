#ifndef __base_task_executor_h__
#define __base_task_executor_h__

#include <map>
#include <list>
#include <windows.h>
#include <process.h>

#include "base/def.h"
#include "base/task.h"
#include "base/locker.h"
#include "base/event.h"

namespace base
{
    struct DefaultThreadFactory
    {
        typedef unsigned (__stdcall *LPTHREAD_FUNCTION)(void* lpThreadParameter);
        static HANDLE CreateThread(LPTHREAD_FUNCTION func, void* param, DWORD flag)
        {
            if (!func)
            {
                return 0;
            }

            return (HANDLE)_beginthreadex(0, 0, func, param, flag, 0);
        }

        static void DestroyThread(HANDLE handle, DWORD timeout)
        {
            if (!handle)
            {
                return;
            }

            DWORD wait = ::WaitForSingleObject(handle, timeout);
            if (wait == WAIT_TIMEOUT)
            {
                ::TerminateThread(handle, 0);
            }
            ::CloseHandle(handle);
        }
    };

    template<typename Factory,
             typename Mutex = CriticalSectionMutex,
             template<typename T, typename Allocator = std::allocator<T> > class Container = std::list>
    class TaskExecutor
    {
    public:
        static const int DEFAULT_MAX_THREAD_COUNT = 5;
        static const int DEFAULT_KEEP_THREAD_ALIVE_TIME = 500;

        enum ThreadState
        {
            THREAD_CREATED,
            THREAD_RUNNING,
            THREAD_WAITING,
            THREAD_TERMINATED
        };

        enum ThreadPoolState
        {
            THREAD_POOL_RUNNING,
            THREAD_POOL_STOP,
            THREAD_POOL_TERMINATED
        };

        TaskExecutor();
        TaskExecutor(int max_count);
        TaskExecutor(int max_count, int keep_alive_time);
        ~TaskExecutor();

        bool Execute(Task* task);
        bool Cancel(Task* task);
        void Clear();
        void Shutdown(DWORD timeout);
        bool IsIdle();
        DWORD WaitIdle(DWORD timeout = INFINITE);

    protected:
        struct ThreadContext
        {
            TaskExecutor* executor;
            LONG          id;
            DWORD         state;
            HANDLE        handler;
        };

        void StopAllThread(DWORD timeout);

        void SetThreadPoolState(DWORD state);
        DWORD GetThreadPoolState();

        Task* GetTask();

        LONG GetNextThreadId();
        void SetThreadState(LONG id, DWORD state);
        void StartNewThread();
        void RemoveThread(LONG id);
        bool HasIdleThread();

        static unsigned __stdcall TaskThread(void* param);
        void TaskThreadImpl(LONG id);

    private:
        int       max_count_;
        int       keep_alive_time_;
        LONG      current_thread_id_;
        LONG      state_;

        Mutex               thread_mutex_;
        std::map<LONG, ThreadContext*> thread_pool_;

        Event               task_event_;
        Container<Task*>    task_list_;

        Event               idle_event_;

    private:
        DISABLE_COPY_AND_ASSIGN(TaskExecutor)
    };
}

#endif