#ifndef __base_type_list_h__
#define __base_type_list_h__

template <typename T, typename U>
struct TypeList
{
    typedef T Head;
    typedef U Tail;
};

struct NullType
{
};

#define TYPELIST1(T1)                                       TypeList<T1, NullType>
#define TYPELIST2(T1, T2)                                   TypeList<T1, TYPELIST1(T2)>
#define TYPELIST3(T1, T2, T3)                               TypeList<T1, TYPELIST2(T2, T3)>
#define TYPELIST4(T1, T2, T3, T4)                           TypeList<T1, TYPELIST3(T2, T3, T4)>
#define TYPELIST5(T1, T2, T3, T4, T5)                       TypeList<T1, TYPELIST4(T2, T3, T4, T5)>
#define TYPELIST6(T1, T2, T3, T4, T5, T6)                   TypeList<T1, TYPELIST5(T2, T3, T4, T5, T6)>
#define TYPELIST7(T1, T2, T3, T4, T5, T6, T7)               TypeList<T1, TYPELIST6(T2, T3, T4, T5, T6, T7)>
#define TYPELIST8(T1, T2, T3, T4, T5, T6, T7, T8)           TypeList<T1, TYPELIST7(T2, T3, T4, T5, T6, T7, T8)>
#define TYPELIST9(T1, T2, T3, T4, T5, T6, T7, T8, T9)       TypeList<T1, TYPELIST8(T2, T3, T4, T5, T6, T7, T8, T9)>
#define TYPELIST10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) TypeList<T1, TYPELIST9(T2, T3, T4, T5, T6, T7, T8, T9, T10)>

template <typename TList, unsigned int index>
struct TypeAt;

template <typename Head, typename Tail>
struct TypeAt<TypeList<Head, Tail>, 0>
{
    typedef Head Result;
};

template <typename Head, typename Tail, unsigned int index>
struct TypeAt<TypeList<Head, Tail>, index>
{
    typedef typename TypeAt<Tail, index - 1>::Result Result;
};

#endif