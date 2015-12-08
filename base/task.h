#ifndef __base_task_h__
#define __base_task_h__

#include "base/tuple.h"
#include "base/dispatcher.h"

namespace base
{
    class Task
    {
    public:
        Task();
        virtual ~Task();

        virtual void Run() = 0;
    };



    template<typename Object, typename Method, typename Params>
    class MethodTask : public Task
    {
    public:
        MethodTask(Object *obj, Method method, const Params &params)
            : obj_(obj), method_(method), params_(params)
        {}

        virtual void Run()
        {
            DispatchToMethod(obj_, method_, params_);
        }

    private:
        Object  *obj_;
        Method   method_;
        Params   params_;
    };

    template<typename Object, typename Method>
    inline Task* NewMethodTask(Object* obj, Method method)
    {
        return new MethodTask<Object, Method, Tuple0>(
                obj, method, MakeTuple());
    }

    template<typename Object, typename Method,
             typename A>
    inline Task* NewMethodTask(Object* obj, Method method,
                               const A& a)
    {
        return new MethodTask<Object, Method, Tuple1<A> >(
            obj, method, MakeTuple(a));
    }

    template<typename Object, typename Method,
             typename A, typename B>
    inline Task* NewMethodTask(Object* obj, Method method,
                               const A& a, const B& b)
    {
        return new MethodTask<Object, Method, Tuple2<A, B> >(
            obj, method, MakeTuple(a, b));
    }

    template<typename Object, typename Method,
             typename A, typename B, typename C>
    inline Task* NewMethodTask(Object* obj, Method method,
                               const A& a, const B& b, const C& c)
    {
        return new MethodTask<Object, Method, Tuple3<A, B, C> >(
            obj, method, MakeTuple(a, b, c));
    }

    template<typename Object, typename Method,
             typename A, typename B, typename C,
             typename D>
    inline Task* NewMethodTask(Object* obj, Method method,
                               const A& a, const B& b, const C& c,
                               const D& d)
    {
        return new MethodTask<Object, Method, Tuple4<A, B, C, D> >(
            obj, method, MakeTuple(a, b, c, d));
    }

    template<typename Object, typename Method,
             typename A, typename B, typename C,
             typename D, typename E>
    inline Task* NewMethodTask(Object* obj, Method method,
                               const A& a, const B& b, const C& c,
                               const D& d, const E& e)
    {
        return new MethodTask<Object, Method, Tuple5<A, B, C, D, E> >(
            obj, method, MakeTuple(a, b, c, d, e));
    }

    template<typename Object, typename Method,
             typename A, typename B, typename C,
             typename D, typename E, typename F>
    inline Task* NewMethodTask(Object* obj, Method method,
                               const A& a, const B& b, const C& c,
                               const D& d, const E& e, const F& f)
    {
        return new MethodTask<Object, Method, Tuple6<A, B, C, D, E, F> >(
            obj, method, MakeTuple(a, b, c, d, e, f));
    }

    template<typename Object, typename Method,
             typename A, typename B, typename C,
             typename D, typename E, typename F,
             typename G>
    inline Task* NewMethodTask(Object* obj, Method method,
                               const A& a, const B& b, const C& c,
                               const D& d, const E& e, const F& f,
                               const G& g)
    {
        return new MethodTask<Object, Method, Tuple7<A, B, C, D, E, F, G> >(
            obj, method, MakeTuple(a, b, c, d, e, f, g));
    }

    template<typename Object, typename Method,
             typename A, typename B, typename C,
             typename D, typename E, typename F,
             typename G, typename H>
    inline Task* NewMethodTask(Object* obj, Method method,
                               const A& a, const B& b, const C& c,
                               const D& d, const E& e, const F& f,
                               const G& g, const H& h)
    {
        return new MethodTask<Object, Method, Tuple8<A, B, C, D, E, F, G, H> >(
            obj, method, MakeTuple(a, b, c, d, e, f, g, h));
    }
}

#endif