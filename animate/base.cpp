#include "animate/base.h"
#include "animate/def.h"

#include <windows.h>

namespace animate
{
    Animate::Animate()
        : frame_elapse_(0)
        , current_(0)
        , interval_(0)
        , total_(INFINITE)
        , finished_(false) {}

    Animate::~Animate() {}

    void Animate::RegisterObserver(Observer* observer)
    {
        if (observer)
        {
            observers_.insert(observer);
        }
    }

    void Animate::UnRegisterObserver(Observer *observer)
    {
        for (_observers::iterator iter = observers_.begin(); iter != observers_.end(); ++iter)
        {
            if (observer == *iter)
            {
                observers_.erase(iter);
                break;
            }
        }
    }

    unsigned Animate::Current()
    {
        return current_;
    }

    unsigned Animate::Interval()
    {
        return interval_;
    }

    unsigned Animate::Total()
    {
        return total_;
    }

    bool Animate::IsFinished()
    {
        return finished_;
    }

    void Animate::SetInterval(unsigned interval)
    {
        interval_ = interval;
    }

    void Animate::SetTotal(unsigned total)
    {
        total_ = total;
    }

    bool Animate::Step()
    {
        if (IsFinished())
        {
            return false;
        }

        bool run_step = false;
        frame_elapse_ += DEFAULT_TIMER_INTERVAL;
        if (frame_elapse_ >= Interval())
        {
            frame_elapse_ = 0;
            run_step = true;
        }
        if (Current() + frame_elapse_ >= Total())
        {
            run_step = true;
        }

        if (run_step)
        {
            RunStep();
            return true;
        }

        return false;
    }

    void Animate::RunStep()
    {
        NotifyObservers();

        unsigned current = Current();
        unsigned total = Total();

        if (current == total)
        {
            SetFinished();
        }

        DoStep();

        current += Interval();
        if (current >= total)
        {
            current = total;
        }
        SetCurrent(current);
    }

    void Animate::NotifyObservers()
    {
        for(_observers::iterator iter = observers_.begin();
            iter != observers_.end();
            ++iter)
        {
            (*iter)->OnAnimateChanged(this);
        }
    }

    void Animate::SetCurrent(unsigned current)
    {
        current_ = current;
    }

    void Animate::SetFinished()
    {
        finished_ = true;
    }
}