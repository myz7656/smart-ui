#include "win/file.h"

namespace win
{
    BOOL File::GetFileSize(LPCTSTR lpszFile, __int64 *size)
    {
        if (!lpszFile || !size)
        {
            return FALSE;
        }

        *size = 0;

        WIN32_FILE_ATTRIBUTE_DATA attr;
        if (!GetFileAttributesEx(lpszFile, GetFileExInfoStandard, &attr))
        {
            return FALSE;
        }

        ULARGE_INTEGER s;
        s.HighPart = attr.nFileSizeHigh;
        s.LowPart = attr.nFileSizeLow;
        *size = s.QuadPart;

        return TRUE;
    }

    BOOL File::ReadFile(LPCTSTR lpszFile, BYTE *pBuf, DWORD dwSize)
    {
        if (!lpszFile || !pBuf)
        {
            return FALSE;
        }

        memset(pBuf, 0, dwSize);
        HANDLE hFile = ::CreateFile(lpszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            return FALSE;
        }

        DWORD dwRead = 0;
        if (::ReadFile(hFile, pBuf, dwSize, &dwRead, NULL) && (dwRead == dwSize))
        {
            ::CloseHandle(hFile);
            return TRUE;
        }
        else
        {
            ::CloseHandle(hFile);
        }

        return FALSE;
    }

    BOOL File::WriteFile(LPCTSTR lpszFile, BYTE *pBuf, DWORD dwSize)
    {
        if (!lpszFile || !pBuf)
        {
            return FALSE;
        }

        HANDLE hFile = ::CreateFile(lpszFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            return FALSE;
        }

        DWORD dwWrite = 0;
        if (::WriteFile(hFile, pBuf, dwSize, &dwWrite, NULL) && (dwWrite == dwSize))
        {
            ::CloseHandle(hFile);
            return TRUE;
        }
        else
        {
            ::CloseHandle(hFile);
        }

        return FALSE;
    }

    BOOL File::CreateDirectory(LPCTSTR lpszDirectory)
    {
        if (!lpszDirectory || _tcslen(lpszDirectory) <= 3)
        {
            return FALSE;
        }

        TCHAR szPath[ MAX_PATH ] = { 0 };
        LPCTSTR lpszTmp = lpszDirectory + 3;
        BOOL bRet = FALSE;
        while (TRUE)
        {
            lpszTmp = ::_tcsstr(lpszTmp,  _T("\\"));
            if (!lpszTmp)
            {
                bRet = TRUE;
                break;
            }
            else
            {
                lpszTmp = lpszTmp + 1;
            }

            ::memset(szPath, 0, MAX_PATH * sizeof(TCHAR));
            ::memcpy_s(szPath, MAX_PATH - sizeof(TCHAR), lpszDirectory, (lpszTmp - lpszDirectory) * sizeof(TCHAR));
            if (INVALID_FILE_ATTRIBUTES == ::GetFileAttributes(szPath))
            {
                bRet = ::CreateDirectory(szPath, NULL);
                if (!bRet)
                {
                    break;
                }
            }
        }

        return bRet;
    }

    BOOL File::DeleteDirectory(LPCTSTR lpszDirectory)
    {
        if (ClearDirectory(lpszDirectory))
        {
            return ::RemoveDirectory(lpszDirectory);
        }

        return FALSE;
    }

    BOOL File::ClearDirectory(LPCTSTR lpszDirectory)
    {
        if (!lpszDirectory)
        {
            return FALSE;
        }

        size_t s = _tcslen(lpszDirectory);
        if (*(lpszDirectory + s - 1) != _T('\\'))
        {
            return FALSE;
        }

        TCHAR szPath[ MAX_PATH ] = { 0 };
        _tcscpy_s(szPath, MAX_PATH - sizeof(TCHAR), lpszDirectory);

        TCHAR szTmp[ MAX_PATH ] = { 0 };
        ::_tcscpy_s(szTmp, MAX_PATH - sizeof(TCHAR), lpszDirectory);
        ::_tcscat_s(szTmp, MAX_PATH - sizeof(TCHAR), _T("*.*"));

        WIN32_FIND_DATA findData;
        HANDLE hFind = ::FindFirstFile(szTmp, &findData);
        if (INVALID_HANDLE_VALUE == hFind)
        {
            return FALSE;
        }

        BOOL bRet = TRUE;
        do 
        {
            if (_tcscmp(findData.cFileName, _T(".")) == 0 ||
                _tcscmp(findData.cFileName, _T("..")) == 0)
            {
                continue;
            }

            ::memset(szTmp, 0, MAX_PATH * sizeof(TCHAR));
            ::_tcscpy_s(szTmp, MAX_PATH - sizeof(TCHAR), lpszDirectory);
            ::_tcscat_s(szTmp, MAX_PATH - sizeof(TCHAR), findData.cFileName);

            if (FILE_ATTRIBUTE_DIRECTORY == (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                size_t l = _tcslen(szTmp);
                if (*(szTmp + l - 1) != _T('\\'))
                {
                    ::_tcscat_s(szTmp, MAX_PATH - sizeof(TCHAR), _T("\\"));
                }

                bRet = ClearDirectory(szTmp);
                if (!bRet)
                {
                    break;
                }

                bRet = ::RemoveDirectory(szTmp);
                if (!bRet)
                {
                    break;
                }

                continue;
            }

            bRet = ::DeleteFile(szTmp);
            if (!bRet)
            {
                break;
            }
        } while (::FindNextFile(hFind, &findData));

        ::FindClose(hFind);
        return bRet;
    }
}