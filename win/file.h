#ifndef __win_file_h__
#define __win_file_h__

#include <windows.h>
#include <tchar.h>

namespace win
{
    class File
    {
    public:
        static BOOL GetFileSize(LPCTSTR lpszFile, __int64 *size);

        static BOOL ReadFile(LPCTSTR lpszFile, BYTE *pBuf, DWORD dwSize);

        static BOOL WriteFile(LPCTSTR lpszFile, BYTE *pBuf, DWORD dwSize);

        static BOOL CreateDirectory(LPCTSTR lpszDirectory);

        static BOOL DeleteDirectory(LPCTSTR lpszDirectory);

        static BOOL ClearDirectory(LPCTSTR lpszDirectory);
    };
}

#endif