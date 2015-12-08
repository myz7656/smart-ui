#ifndef __win_asset_manager_h__
#define __win_asset_manager_h__

#include <set>
#include <map>
#include <string>

#include "base/singleton.h"

#include "win/image.h"

namespace win
{
    class LocalAssetLoader;
    class EmbedAssetLoader;

    class AssetManager
    {
    public:
        class Loader
        {
        public:
            virtual ~Loader() = 0;

            virtual bool LoadBinary(const wchar_t* name, byte** buffer, unsigned* size) = 0;
            virtual bool LoadImage(const wchar_t* name, Image* image) = 0;
        };

        AssetManager();
        ~AssetManager();

        void RegisterExternalLoader(Loader* loader);
        void UnRegisterExternalLoader(Loader* loader);

        bool LoadBinary(const wchar_t* name, byte** buffer, unsigned* size);
        bool LoadImage(const wchar_t* name, Image* image);
        void Clear();

    private:
        void RegisterDefaultLoader();
        void UnRegisterDefaultLoader();

    private:
        typedef std::map<std::wstring, Image*> _image_maps;

        std::set<Loader*> loaders_;
        _image_maps       img_cache_map_;

        LocalAssetLoader* local_loader_;
        EmbedAssetLoader* embed_loader_;
    };

    typedef base::Singleton<AssetManager> AssetManagerSingleton;
}

#endif