#include "animate/animate_container.h"

#include "base/task_center.hpp"
#include "base/time_ticks.h"


namespace animate
{
    AnimateContainer::AnimateContainer()
        : timer_id_(0L) {}

    AnimateContainer::~AnimateContainer() {}

    bool AnimateContainer::Start(Animate* animate)
    {
        if (animate)
        {
            PutAnimate(animate);
        }

        if (timer_id_ != 0L)
        {
            return true;
        }

        int interval = DEFAULT_TIMER_INTERVAL;
        base::Task* delay_task = base::NewMethodTask(this, &AnimateContainer::OnTimer);
        if (!delay_task)
        {
            return false;
        }

        timer_id_ = base::Singleton<base::TaskCenterUI>::Instance().PostDelayTask(delay_task, (unsigned)interval, true);
        return true;
    }

    void AnimateContainer::Stop()
    {
        if (timer_id_ != 0L)
        {
            base::Singleton<base::TaskCenterUI>::Instance().DiscardTask(timer_id_);
            timer_id_ = 0L;
        }

        Clear();
    }

    void AnimateContainer::PutAnimate(Animate* animate)
    {
        if (animate)
        {
            unsigned interval = animate->Interval();
            unsigned total = animate->Total();
            if (interval > DEFAULT_TIMER_INTERVAL)
            {
                interval = interval / DEFAULT_TIMER_INTERVAL * DEFAULT_TIMER_INTERVAL;
            }
            else
            {
                interval = DEFAULT_TIMER_INTERVAL;
            }
            animate->SetInterval(interval);
            if (interval > total)
            {
                animate->SetTotal(total);
            }
            animates_.insert(animate);
        }
    }

    void AnimateContainer::OnTimer()
    {
        _animates::iterator iter = animates_.begin();
        while (iter != animates_.end())
        {
            Animate* animate = *iter;
            animate->Step();
            if (animate->IsFinished())
            {
                iter = animates_.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }

    void AnimateContainer::Clear()
    {
        _animates::iterator iter = animates_.begin();
        while (iter != animates_.end())
        {
            Animate* animate = *iter;
            delete animate;

            ++iter;
        }
    }
}