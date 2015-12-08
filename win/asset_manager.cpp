#include "win/asset_manager.h"
#include "win/asset_loader.h"

namespace win
{
    AssetManager::Loader::~Loader() {}

    AssetManager::AssetManager()
    {
        RegisterDefaultLoader();
    }

    AssetManager::~AssetManager()
    {
        Clear();
        UnRegisterDefaultLoader();
    }

    void AssetManager::RegisterExternalLoader(Loader* loader)
    {
        if (loader)
        {
            loaders_.insert(loader);
        }
    }

    void AssetManager::UnRegisterExternalLoader(Loader* loader)
    {
        for (std::set<Loader*>::iterator iter = loaders_.begin(); iter != loaders_.end(); ++iter)
        {
            Loader* load = *iter;
            if (loader == load)
            {
                loaders_.erase(load);
                break;
            }
        }
    }

    bool AssetManager::LoadBinary(const wchar_t* name, byte** buffer, unsigned* size)
    {
        if (!name || !*name || !buffer)
        {
            return false;
        }

        bool loaded = false;
        for (std::set<Loader*>::iterator iter = loaders_.begin(); iter != loaders_.end(); ++iter)
        {
            Loader* loader = *iter;
            if (loader->LoadBinary(name, buffer, size))
            {
                loaded = true;
                break;
            }
        }

        return loaded;
    }

    bool AssetManager::LoadImage(const wchar_t* name, Image* image)
    {
        if (!name || !*name || !image)
        {
            return false;
        }

        _image_maps::iterator iter = img_cache_map_.find(std::wstring(name));
        if (iter != img_cache_map_.end())
        {
            image->Copy(iter->second);
            return true;
        }

        bool loaded = false;
        for (std::set<Loader*>::iterator iter = loaders_.begin(); iter != loaders_.end(); ++iter)
        {
            Loader* loader = *iter;
            if (loader->LoadImage(name, image))
            {
                loaded = true;
                break;
            }
        }

        if (loaded)
        {
            Image *cache = new Image;
            if (cache)
            {
                cache->Copy(image);
                img_cache_map_[std::wstring(name)] = cache;
            }
        }
        return loaded;
    }

    void AssetManager::Clear()
    {
        for (_image_maps::iterator iter = img_cache_map_.begin(); iter != img_cache_map_.end(); ++iter)
        {
            Image* image = iter->second;
            image->Release();
            delete image;
        }

        img_cache_map_.clear();
    }

    void AssetManager::RegisterDefaultLoader()
    {
        local_loader_ = new LocalAssetLoader;
        embed_loader_ = new EmbedAssetLoader;
        RegisterExternalLoader(local_loader_);
        RegisterExternalLoader(embed_loader_);
    }

    void AssetManager::UnRegisterDefaultLoader()
    {
        UnRegisterExternalLoader(local_loader_);
        UnRegisterExternalLoader(embed_loader_);

        if (local_loader_)
        {
            delete local_loader_;
            local_loader_ = 0;
        }
        
        if (embed_loader_)
        {
            delete embed_loader_;
            embed_loader_ = 0;
        }
    }
}