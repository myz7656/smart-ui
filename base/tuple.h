#ifndef __base_tuple_h__
#define __base_tuple_h__

namespace base
{
    template<typename P>
    struct TupleTraits
    {
        typedef P ValueType;
        typedef P& RefType;
        typedef const P& ParamType;
    };

    template<typename P>
    struct TupleTraits<P&>
    {
        typedef P ValueType;
        typedef P& RefType;
        typedef P& ParamType;
    };



    struct Tuple0 {};

    template<typename A>
    struct Tuple1
    {
    public:
        typedef A TypeA;

        Tuple1() {}
        explicit Tuple1(typename TupleTraits<A>::ParamType a) : a(a) {}

        A a;
    };

    template<typename A, typename B>
    struct Tuple2
    {
    public:
        typedef A TypeA;
        typedef B TypeB;

        Tuple2() {}
        Tuple2(typename TupleTraits<A>::ParamType a,
               typename TupleTraits<B>::ParamType b)
            : a(a), b(b) {}

        A a;
        B b;
    };

    template<typename A, typename B, typename C>
    struct Tuple3
    {
    public:
        typedef A TypeA;
        typedef B TypeB;
        typedef C TypeC;

        Tuple3() {}
        Tuple3(typename TupleTraits<A>::ParamType a,
               typename TupleTraits<B>::ParamType b,
               typename TupleTraits<C>::ParamType c)
            : a(a), b(b), c(c){}

        A a;
        B b;
        C c;
    };

    template<typename A, typename B, typename C,
             typename D>
    struct Tuple4
    {
    public:
        typedef A TypeA;
        typedef B TypeB;
        typedef C TypeC;
        typedef D TypeD;

        Tuple4() {}
        Tuple4(typename TupleTraits<A>::ParamType a,
               typename TupleTraits<B>::ParamType b,
               typename TupleTraits<C>::ParamType c,
               typename TupleTraits<D>::ParamType d)
            : a(a), b(b), c(c), d(d) {}

        A a;
        B b;
        C c;
        D d;
    };

    template<typename A, typename B, typename C,
             typename D, typename E>
    struct Tuple5
    {
    public:
        typedef A TypeA;
        typedef B TypeB;
        typedef C TypeC;
        typedef D TypeD;
        typedef E TypeE;

        Tuple5() {}
        Tuple5(typename TupleTraits<A>::ParamType a,
               typename TupleTraits<B>::ParamType b,
               typename TupleTraits<C>::ParamType c,
               typename TupleTraits<D>::ParamType d,
               typename TupleTraits<E>::ParamType e)
            : a(a), b(b), c(c), d(d), e(e) {}

        A a;
        B b;
        C c;
        D d;
        E e;
    };

    template<typename A, typename B, typename C,
             typename D, typename E, typename F>
    struct Tuple6
    {
    public:
        typedef A TypeA;
        typedef B TypeB;
        typedef C TypeC;
        typedef D TypeD;
        typedef E TypeE;
        typedef F TypeF;

        Tuple6() {}
        Tuple6(typename TupleTraits<A>::ParamType a,
               typename TupleTraits<B>::ParamType b,
               typename TupleTraits<C>::ParamType c,
               typename TupleTraits<D>::ParamType d,
               typename TupleTraits<E>::ParamType e,
               typename TupleTraits<F>::ParamType f)
            : a(a), b(b), c(c), d(d), e(e), f(f) {}

        A a;
        B b;
        C c;
        D d;
        E e;
        F f;
    };

    template<typename A, typename B, typename C,
             typename D, typename E, typename F,
             typename G>
    struct Tuple7
    {
    public:
        typedef A TypeA;
        typedef B TypeB;
        typedef C TypeC;
        typedef D TypeD;
        typedef E TypeE;
        typedef F TypeF;
        typedef G TypeG;

        Tuple7() {}
        Tuple7(typename TupleTraits<A>::ParamType a,
               typename TupleTraits<B>::ParamType b,
               typename TupleTraits<C>::ParamType c,
               typename TupleTraits<D>::ParamType d,
               typename TupleTraits<E>::ParamType e,
               typename TupleTraits<F>::ParamType f,
               typename TupleTraits<G>::ParamType g)
            : a(a), b(b), c(c), d(d), e(e), f(f), g(g) {}

        A a;
        B b;
        C c;
        D d;
        E e;
        F f;
        G g;
    };

    template<typename A, typename B, typename C,
             typename D, typename E, typename F,
             typename G, typename H>
    struct Tuple8
    {
    public:
        typedef A TypeA;
        typedef B TypeB;
        typedef C TypeC;
        typedef D TypeD;
        typedef E TypeE;
        typedef F TypeF;
        typedef G TypeG;
        typedef H TypeH;

        Tuple8() {}
        Tuple8(typename TupleTraits<A>::ParamType a,
               typename TupleTraits<B>::ParamType b,
               typename TupleTraits<C>::ParamType c,
               typename TupleTraits<D>::ParamType d,
               typename TupleTraits<E>::ParamType e,
               typename TupleTraits<F>::ParamType f,
               typename TupleTraits<G>::ParamType g,
               typename TupleTraits<H>::ParamType h)
            : a(a), b(b), c(c), d(d), e(e), f(f), g(g), h(h) {}

        A a;
        B b;
        C c;
        D d;
        E e;
        F f;
        G g;
        H h;
    };



    inline Tuple0 MakeTuple()
    {
        return Tuple0();
    }

    template<typename A>
    inline Tuple1<A> MakeTuple(const A& a)
    {
        return Tuple1<A>(a);
    }

    template<typename A, typename B>
    inline Tuple2<A, B> MakeTuple(const A& a, const B& b)
    {
        return Tuple2<A, B>(a, b);
    }

    template<typename A, typename B, typename C>
    inline Tuple3<A, B, C> MakeTuple(const A& a, const B& b, const C& c)
    {
        return Tuple3<A, B, C>(a, b, c);
    }

    template<typename A, typename B, typename C,
             typename D>
    inline Tuple4<A, B, C, D> MakeTuple(const A& a, const B& b, const C& c,
                                        const D& d)
    {
        return Tuple4<A, B, C, D>(a, b, c, d);
    }

    template<typename A, typename B, typename C,
             typename D, typename E>
    inline Tuple5<A, B, C, D, E> MakeTuple(const A& a, const B& b, const C& c,
                                           const D& d, const E& e)
    {
        return Tuple5<A, B, C, D, E>(a, b, c, d, e);
    }

    template<typename A, typename B, typename C,
             typename D, typename E, typename F>
    inline Tuple6<A, B, C, D, E, F> MakeTuple(const A& a, const B& b, const C& c,
                                              const D& d, const E& e, const F& f)
    {
        return Tuple6<A, B, C, D, E, F>(a, b, c, d, e, f);
    }

    template<typename A, typename B, typename C,
             typename D, typename E, typename F,
             typename G>
    inline Tuple7<A, B, C, D, E, F, G> MakeTuple(const A& a, const B& b, const C& c,
                                                 const D& d, const E& e, const F& f,
                                                 const G& g)
    {
        return Tuple7<A, B, C, D, E, F, G>(a, b, c, d, e, f, g);
    }

    template<typename A, typename B, typename C,
             typename D, typename E, typename F,
             typename G, typename H>
    inline Tuple8<A, B, C, D, E, F, G, H> MakeTuple(const A& a, const B& b, const C& c,
                                                    const D& d, const E& e, const F& f,
                                                    const G& g, const H& h)
    {
        return Tuple8<A, B, C, D, E, F, G, H>(a, b, c, d, e, f, g, h);
    }
}

#endif