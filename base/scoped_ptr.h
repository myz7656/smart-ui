#ifndef __base_scoped_ptr_h__
#define __base_scoped_ptr_h__

#include "base/def.h"

namespace base
{
    template <typename T>
    class ScopedPtr
    {
    public:
        typedef T element_type;

        explicit ScopedPtr(T* p = 0)
            : ptr(p) {}

        ~ScopedPtr()
        {
            typedef char type_must_be_complete[sizeof(T)];
            delete ptr;
        }

        void reset(T* p = 0)
        {
            typedef char type_must_be_complete[sizeof(T)];

            if (ptr != p)
            {
                T* obj = ptr;
                ptr = p;
                delete obj;
            }
        }

        T& operator*() const
        {
            return *ptr;
        }

        T* operator->() const
        {
            return ptr;
        }

        T* get() const
        {
            return ptr;
        }

        void swap(ScopedPtr& b)
        {
            T* tmp = b.ptr;
            b.ptr = ptr;
            ptr = tmp;
        }

        T* release()
        {
            T* tmp = ptr;
            ptr = 0;
            return tmp;
        }

        T** accept()
        {
            if (ptr)
            {
                delete ptr;
                ptr = 0;
            }
            return &ptr;
        }

        T** use()
        {
            return &ptr;
        }

    private:
        T* ptr;

    private:
        DISABLE_COPY_AND_ASSIGN(ScopedPtr)
    };



    template<typename T>
    class ScopedArray 
    {
    public:
        typedef T element_type;

        explicit ScopedArray(T* p = 0)
            : ptr(p) {}

        ~ScopedArray()
        {
            typedef char type_must_be_complete[sizeof(T)];
            delete[] ptr;
        }

        void reset(T* p = 0)
        {
            typedef char type_must_be_complete[sizeof(T)];
            if (ptr != p) 
            {
                T* arr = ptr;
                ptr = p;
                delete [] arr;
            }
        }

        T& operator[](int i) const
        {
            return ptr[i];
        }

        T* get() const
        {
            return ptr;
        }

        void swap(ScopedArray & b)
        {
            T* tmp = b.ptr;
            b.ptr = ptr;
            ptr = tmp;
        }

        T* release()
        {
            T* tmp = ptr;
            ptr = 0;
            return tmp;
        }

        T** accept()
        {
            if (ptr)
            {
                delete [] ptr;
                ptr = 0;
            }

            return &ptr;
        }

    private:
        T* ptr;

    private:
        DISABLE_COPY_AND_ASSIGN(ScopedArray)
    };
}

#endif