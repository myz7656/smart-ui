#include "win/asset_loader.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

#define SWFIT_RES_TYPE (L"SWRES")

namespace win
{
    LocalAssetLoader::LocalAssetLoader()
    {
        Gdiplus::GdiplusStartup(&gdi_token_, &gdiplus_startup_input_, NULL);
        Init();
    }

    LocalAssetLoader::~LocalAssetLoader()
    {
        Gdiplus::GdiplusShutdown(gdi_token_);
    }

    void LocalAssetLoader::Init()
    {
        CString path;
        ::GetModuleFileName(NULL, path.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
        path.ReleaseBuffer();
        int pos = path.ReverseFind('\\');
        if (pos != -1)
        {
            path = path.Left(pos);
        }

        workspace_ = (LPCWSTR)path;
        workspace_.append(L"\\res\\");
    }

    bool LocalAssetLoader::LoadBinary(const wchar_t* name, byte** buffer, unsigned* size)
    {
        if (!name || !*name || !buffer)
        {
            return false;
        }

        std::wstring filename = GetFilePath(name);
        *buffer = 0;
        if (size)
        {
            *size = 0;
        }

        __int64 i64Size = 0;
        if (File::GetFileSize(filename.c_str(), &i64Size) && i64Size > 0)
        {
            DWORD dwReadSize = 0;
            *buffer = new byte[(unsigned)i64Size];
            if (*buffer)
            {
                memset(*buffer, 0, (size_t)i64Size);
                dwReadSize = (DWORD)i64Size;
            }
            else
            {
                return false;
            }

            if (!File::ReadFile(filename.c_str(), *buffer, dwReadSize))
            {
                delete[] *buffer;
                *buffer = 0;
                return false;
            }
            else
            {
                if (size)
                {
                    *size = dwReadSize;
                }
                return true;
            }
        }
        return false;
    }

    bool LocalAssetLoader::LoadImage(const wchar_t* name, Image* image)
    {
        if (!name || !*name || !image)
        {
            return false;
        }

        std::wstring strPath = GetFilePath(name);

        Gdiplus::Image * gdiplus_image = Gdiplus::Image::FromFile(strPath.c_str());
        if (!gdiplus_image)
        {
            return false;
        }

        BOOL created = image->CreateFromImage(gdiplus_image);
        delete gdiplus_image;

        return !!created;
    }

    std::wstring LocalAssetLoader::GetWorkspace()
    {
        return workspace_;
    }

    std::wstring LocalAssetLoader::GetFilePath(const wchar_t* name)
    {
        return workspace_ + name;
    }


    EmbedAssetLoader::EmbedAssetLoader() 
    {
        Gdiplus::GdiplusStartup(&gdi_token_, &gdiplus_startup_input_, NULL);
    }

    EmbedAssetLoader::~EmbedAssetLoader()
    {
        Gdiplus::GdiplusShutdown(gdi_token_);
    }

    bool EmbedAssetLoader::LoadBinary(const wchar_t* name, byte** buffer, unsigned* size)
    {
        if (!name || !*name || !buffer)
        {
            return false;
        }

        return LoadResource((HMODULE)&__ImageBase, name, SWFIT_RES_TYPE, buffer, size);
    }

    bool EmbedAssetLoader::LoadImage(const wchar_t* name, Image* image)
    {
        if (!name || !*name || !image)
        {
            return false;
        }

        HMODULE hModule = (HMODULE)&__ImageBase;
        HRSRC hRsrc = ::FindResource(hModule, name, SWFIT_RES_TYPE);
        if (NULL == hRsrc)
            return false;

        DWORD dwSize = ::SizeofResource(hModule, hRsrc);
        if (0 == dwSize)
            return false;

        HGLOBAL hGlobal = ::LoadResource(hModule, hRsrc);
        if (NULL == hGlobal)
            return false;

        IStream* pstm = NULL;
        CreateStreamOnHGlobal(hGlobal, FALSE, &pstm);

        Gdiplus::Image* gdiplus_image = NULL;

        if (pstm)
        {
            gdiplus_image = Gdiplus::Image::FromStream(pstm);
            pstm->Release();
        }
        ::FreeResource(hGlobal);

        BOOL created = false;
        if (gdiplus_image)
        {
            created = image->CreateFromImage(gdiplus_image);
            delete gdiplus_image;
        }

        return !!created;
    }

    bool EmbedAssetLoader::LoadResource(HMODULE module, const wchar_t* name, const wchar_t* type, byte** buffer, unsigned* size)
    {
        HRSRC hRsrc = ::FindResource(module, name, type);
        if (NULL == hRsrc)
            return false;

        DWORD dwSize = ::SizeofResource(module, hRsrc); 
        if (0 == dwSize)
            return false;

        HGLOBAL hGlobal = ::LoadResource(module, hRsrc); 
        if (NULL == hGlobal)
            return false;

        LPVOID pBuf = ::LockResource(hGlobal); 
        if (NULL == pBuf)
            return false;

        *buffer = new byte[dwSize];
        if (NULL == *buffer)
        {
            ::FreeResource(hGlobal);
            return false;
        }

        memset(*buffer, 0, dwSize);
        memcpy(*buffer, pBuf, dwSize);

        if (size)
            *size = dwSize;

        ::FreeResource(hGlobal);
        return true;
    }
}