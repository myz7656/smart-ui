#include "util/string.h"
#include <windows.h>

namespace util
{
    std::wstring String::UTF8ToUnicode(const char* text)
    {
        std::wstring str_text;
        if (!text || !*text)
        {
            return str_text;
        }

        int len = ::MultiByteToWideChar(CP_UTF8, 0, text, (int)strlen(text), 0, 0);
        if (len > 0)
        {
            wchar_t* wsz_text = new wchar_t[len + 1];
            if (wsz_text && ::MultiByteToWideChar(CP_UTF8, 0, text, (int)strlen(text), wsz_text, len) != 0 )
            {
                wsz_text[len] = 0;
                str_text = wsz_text;
            }

            if (wsz_text)
                delete[] wsz_text;
        }

        return str_text;
    }
    std::string String::UnicodeToUTF8(const wchar_t* text)
    {
        std::string str_text;
        if (!text || !*text)
        {
            return str_text;
        }

        int len = ::WideCharToMultiByte(CP_UTF8, 0, text, (int)wcslen(text), 0, 0, 0, 0);
        if (len > 0)
        {
            char* sz_text = new char[len + 1];
            if (sz_text && ::WideCharToMultiByte(CP_UTF8, 0, text, (int)wcslen(text), sz_text, len, 0, 0) != 0)
            {
                sz_text[len] = 0;
                str_text = sz_text;
            }

            if (sz_text)
                delete[] sz_text;
        }

        return str_text;
    }
}