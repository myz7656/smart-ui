#ifndef __base_dispatcher_h__
#define __base_dispatcher_h__

#include "base/tuple.h"

namespace base
{
    template<typename Object, typename Method>
    inline void DispatchToMethod(Object* obj, Method method, const Tuple0& arg)
    {
        (obj->*method)();
    }

    template<typename Object, typename Method,
             typename A>
    inline void DispatchToMethod(Object* obj, Method method, const Tuple1<A>& arg)
    {
        (obj->*method)(arg.a);
    }

    template<typename Object, typename Method,
             typename A, typename B>
    inline void DispatchToMethod(Object* obj, Method method, const Tuple2<A, B>& arg)
    {
        (obj->*method)(arg.a, arg.b);
    }

    template<typename Object, typename Method,
             typename A, typename B, typename C>
    inline void DispatchToMethod(Object* obj, Method method, const Tuple3<A, B, C>& arg)
    {
        (obj->*method)(arg.a, arg.b, arg.c);
    }

    template<typename Object, typename Method,
             typename A, typename B, typename C,
             typename D>
    inline void DispatchToMethod(Object* obj, Method method, const Tuple4<A, B, C, D>& arg)
    {
        (obj->*method)(arg.a, arg.b, arg.c, arg.d);
    }

    template<typename Object, typename Method,
             typename A, typename B, typename C,
             typename D, typename E>
    inline void DispatchToMethod(Object* obj, Method method, const Tuple5<A, B, C, D, E>& arg)
    {
        (obj->*method)(arg.a, arg.b, arg.c, arg.d, arg.e);
    }

    template<typename Object, typename Method,
             typename A, typename B, typename C,
             typename D, typename E, typename F>
    inline void DispatchToMethod(Object* obj, Method method, const Tuple6<A, B, C, D, E, F>& arg)
    {
        (obj->*method)(arg.a, arg.b, arg.c, arg.d, arg.e, arg.f);
    }

    template<typename Object, typename Method,
             typename A, typename B, typename C,
             typename D, typename E, typename F,
             typename G>
    inline void DispatchToMethod(Object* obj, Method method, const Tuple7<A, B, C, D, E, F, G>& arg)
    {
        (obj->*method)(arg.a, arg.b, arg.c, arg.d, arg.e, arg.f, arg.g);
    }

    template<typename Object, typename Method,
             typename A, typename B, typename C,
             typename D, typename E, typename F,
             typename G, typename H>
    inline void DispatchToMethod(Object* obj, Method method, const Tuple8<A, B, C, D, E, F, G, H>& arg)
    {
        (obj->*method)(arg.a, arg.b, arg.c, arg.d, arg.e, arg.f, arg.g, arg.h);
    }
}

#endif