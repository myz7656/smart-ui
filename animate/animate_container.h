#ifndef __animate_animate_container_h__
#define __animate_animate_container_h__

#include <set>
#include <windows.h>

#include "animate/base.h"
#include "animate/def.h"

namespace animate
{
    class AnimateContainer
    {
    public:
        AnimateContainer();
        ~AnimateContainer();

        bool Start(Animate* animate);
        void Stop();

    protected:
        void PutAnimate(Animate* animate);
        void OnTimer();
        void Clear();

    private:
        typedef std::set<Animate*> _animates;

        _animates animates_;
        LONG      timer_id_;
    };
}

#endif