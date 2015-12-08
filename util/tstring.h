#ifndef __util_t_string_h__
#define __util_t_string_h__

#include <string> 
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <tchar.h>

namespace util
{
    template<typename T>
    class TString
    {
    public:
        typedef std::basic_string<T, std::char_traits<T>, std::allocator<T> > value_type;
        typedef typename value_type::size_type size_type;

        TString() {}

        TString(const value_type &str)
        {
            str_ = str;
        }

        TString(const TString &obj)
        {
            str_ = obj.str_;
        }

        TString & operator=(const value_type &str)
        {
            str_ = str;
            return *this;
        }

        TString & operator=(const TString &obj)
        {
            if (&obj != this)
            {
                str_ = obj.str_;
            }

            return *this;
        }

    public:
        const T *C_Str()
        {
            return str_.c_str();
        }

        void Trim()
        {
            if (str_.size() > 0)
            {
                std::basic_string<T>::iterator c;

                for (c = str_.begin(); c != str_.end() && _istspace(*c++); );
                str_.erase(str_.begin(), --c);

                for (c = str_.end(); c != str_.begin() && _istspace(*--c); );
                str_.erase(++c, str_.end());
            }
        }

        size_type Tokenize(const value_type &sep, size_type pos, value_type &token)
        {
            token.clear();
            if (str_.empty() || str_.size() <= pos)
            {
                return str_.npos;
            }

            value_type s = str_.substr(pos);

            size_type p = 0;
            size_type find = 0;
            while (token.empty())
            {
                find = s.find(sep.c_str(), p);
                if (find == s.npos)
                {
                    token = s.substr(p);
                    return s.npos;
                }
                else
                {
                    token = s.substr(p, find - p);
                    p += sep.size();
                }
            }

            return find + pos;
        }

    private:
        value_type str_;
    };
}

#endif