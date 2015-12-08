#ifndef __util_string_h__
#define __util_string_h__

#include <string>
#include <tchar.h>

namespace util
{
    class String
    {
    public:
        template<typename T>
        static bool IsEmpty(const T* text)
        {
            if (!text || !*text)
            {
                return true;
            }
            return false;
        }

        template<typename T>
        static std::string TToHex(T value)
        {
            std::string s;
            char sz[sizeof(T) * 8 / 4 + 1] = {0};
            for (int i = 0; i < sizeof(T) * 8 / 4; i++)
            {
                unsigned off = sizeof(T) * 8 - 4 * (i + 1);
                char p = (char)((value >> off) & 0xF);
                if (p <= 9 && p >= 0)
                {
                    p += '0';
                }
                else if (p >= 0xA && p <= 0xF)
                {
                    p += 'a';
                    p -= 10;
                }

                sz[i] = p;
            }

            s = sz;
            return s;
        }

        template<typename T>
        static T HexToT(const char *p, int n)
        {
            T t = 0;
            while (p && *p && n != 0)
            {
                t <<= 4;
                if (*p >= '0' && *p <= '9')
                {
                    t |= (*p - '0');
                }
                else if (*p >= 'a' && *p <= 'f')
                {
                    t |= (*p - 'a' + 10);
                }
                else if (*p >= 'A' && *p <= 'F')
                {
                    t |= (*p - 'A' + 10);
                }
                else
                {
                    return 0;
                }

                p++;
                n--;
            }

            return t;
        }

        static std::wstring UTF8ToUnicode(const char* text);
        static std::string UnicodeToUTF8(const wchar_t* text);
    };
}

#endif