#ifndef __animate_base_h__
#define __animate_base_h__

#include <set>

namespace animate
{
    class Animate
    {
    public:
        class Observer
        {
        public:
            virtual void OnAnimateChanged(Animate* animate) = 0;
        };

        Animate();
        virtual ~Animate();

        void RegisterObserver(Observer *observer);
        void UnRegisterObserver(Observer *observer);

        unsigned Current();
        unsigned Interval();
        unsigned Total();
        bool IsFinished();

        void SetInterval(unsigned interval);
        void SetTotal(unsigned total);

        bool Step();

    protected:
        void NotifyObservers();
        void SetCurrent(unsigned current);
        void SetFinished();
        void RunStep();

        virtual void DoStep() = 0;

    private:
        typedef std::set<Observer*>  _observers;

        _observers  observers_;

        unsigned    frame_elapse_;
        unsigned    current_;
        unsigned    interval_;
        unsigned    total_;

        bool        finished_;
    };
}

#endif