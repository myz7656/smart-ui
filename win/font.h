#ifndef __win_font_h__
#define __win_font_h__

#include <windows.h>
#include <atlstr.h>
#include <Strsafe.h>
#include <set>

#define FONT_ENUM_BEGIN(name)\
        if (strName.CompareNoCase(name) == 0)\
        {

#define FONT_ENUM_ITEM(value)\
            if (IsFontExist(value))\
            {\
                return value;\
            }

#define FONT_ENUM_END()\
            return strName;\
        }


class FontEnum
{
public:
    bool Enum(LPCTSTR lpszName)
    {
        LOGFONT lFont = {0};
        lFont.lfCharSet = DEFAULT_CHARSET;

        face_name_ = lpszName;

        if (lpszName)
        {
            StringCchCopy( (LPTSTR)lFont.lfFaceName, LF_FACESIZE, lpszName);
        }

        HDC hDc = ::GetDC(NULL);
        int nRet = ::EnumFontFamiliesEx(hDc, &lFont, (FONTENUMPROC)EnumFontFamExProc, (LPARAM)this, 0);

        ::ReleaseDC(NULL, hDc);

        return nRet == 0;
    }

private:
    static int CALLBACK EnumFontFamExProc(
        const LOGFONT    *lpelfe,
        const TEXTMETRIC *lpntme,
        DWORD      FontType,
        LPARAM     lParam)
    {
        FontEnum* pThis = (FontEnum*)lParam;
        if (pThis)
        {
            return pThis->OnCompare(lpelfe->lfFaceName);
        }

        return 1;
    }

    int OnCompare(LPCTSTR lpszFaceName)
    {
        if (face_name_.CompareNoCase(lpszFaceName) == 0)
        {
            return 0;
        }

        return 1;
    }

private:
    CString face_name_;
};

class FontHelper
{
public:
    static HFONT CreateTextFont(LPCTSTR lpszName, INT nSize, BOOL bBold, BOOL bItalic, BOOL bUnderline)
    {
        LOGFONT lfFont = {0};
        ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lfFont);

        CString strName = Convert(lpszName);
        _tcsncpy_s(lfFont.lfFaceName, strName, LF_FACESIZE - 1);

        lfFont.lfHeight     = -nSize;
        lfFont.lfWeight     = (bBold ? FW_BOLD : FW_NORMAL);
        lfFont.lfUnderline  = bUnderline;
        lfFont.lfItalic     = bItalic;

        return ::CreateFontIndirect(&lfFont);
    }

    static Gdiplus::Font* CreateGdiplusFont(LPCTSTR lpszName, INT nSize, BOOL bBold, BOOL bItalic, BOOL bUnderline)
    {
        CString strName = Convert(lpszName);
        INT style = Gdiplus::FontStyleRegular;

        if (bBold && bItalic)
        {
            style = Gdiplus::FontStyleBoldItalic;
        }
        else if (bBold)
        {
            style = Gdiplus::FontStyleBold;
        }
        else if (bItalic)
        {
            style = Gdiplus::FontStyleItalic;
        }

        if (bUnderline)
        {
            style = Gdiplus::FontStyleUnderline;
        }

        return new Gdiplus::Font(strName, (Gdiplus::REAL)nSize, style, Gdiplus::UnitPixel);
    }

    static BOOL IsFontExist(LPCTSTR lpszName)
    {
        CRegKey RegOp;
        TCHAR   szValue[ MAX_PATH ] = { 0 };
        DWORD   dwSize = MAX_PATH;

        if (ERROR_SUCCESS == RegOp.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"), KEY_READ) &&
            ERROR_SUCCESS == RegOp.QueryStringValue(lpszName, szValue, &dwSize))
        {
            return TRUE;
        }
    
        return FALSE;
    }

    static CString Convert(const CString &strName)
    {
        FontEnum font_enum;
        if (font_enum.Enum(strName))
        {
            return strName;
        }

        return GetDefaultFontName();
    }

    static CString GetDefaultFontName()
    {
        static CString strDefaultFont;

        if (!strDefaultFont.IsEmpty())
        {
            return strDefaultFont;
        }

        strDefaultFont = _T("Î¢ÈíÑÅºÚ");

        FontEnum font_enum;
        if (font_enum.Enum(strDefaultFont))
        {
            return strDefaultFont;
        }

        strDefaultFont = _T("ËÎÌå");
        return strDefaultFont;
    }

    static CString ConvertHelper(const CString &strName)
    {
        FONT_ENUM_BEGIN(_T("Î¢ÈíÑÅºÚ"))
            FONT_ENUM_ITEM(_T("Î¢ÈíÑÅºÚ (TrueType)"))
            FONT_ENUM_ITEM(_T("Microsoft YaHei (TrueType)"))
            FONT_ENUM_ITEM(_T("Microsoft YaHei (TrueType) & Microsoft YaHei UI (TrueType)"))
        FONT_ENUM_END()

        FONT_ENUM_BEGIN(_T("ËÎÌå"))
            FONT_ENUM_ITEM(_T("ËÎÌå & ÐÂËÎÌå (TrueType)"))
            FONT_ENUM_ITEM(_T("SimSun & NSimSun (TrueType)"))
        FONT_ENUM_END()

        return strName;
    }
};

#endif