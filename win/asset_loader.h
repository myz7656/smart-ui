#ifndef __win_asset_loader_h__
#define __win_asset_loader_h__

#include <stdio.h>
#include <vector>

#include "win/file.h"
#include "win/image.h"
#include "win/asset_manager.h"

#include <atlstr.h>
#include <map>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

namespace win
{
    class LocalAssetLoader : public AssetManager::Loader
    {
    public:
        LocalAssetLoader();
        ~LocalAssetLoader();

        virtual bool LoadBinary(const wchar_t* name, byte** buffer, unsigned* size);
        virtual bool LoadImage(const wchar_t* name, Image* image);

    private:
        void Init();
        std::wstring GetWorkspace();
        std::wstring GetFilePath(const wchar_t* name);

    private:
        Gdiplus::GdiplusStartupInput gdiplus_startup_input_;
        ULONG_PTR gdi_token_;

        std::wstring workspace_;
    };

    class EmbedAssetLoader : public AssetManager::Loader
    {
    public:
        EmbedAssetLoader();
        ~EmbedAssetLoader();

        virtual bool LoadBinary(const wchar_t* name, byte** buffer, unsigned* size);
        virtual bool LoadImage(const wchar_t* name, Image* image);

    protected:
        bool LoadResource(HMODULE module, const wchar_t* name, const wchar_t* type, byte** buffer, unsigned* size);

    private:
        Gdiplus::GdiplusStartupInput gdiplus_startup_input_;
        ULONG_PTR gdi_token_;
    };
}

#endif