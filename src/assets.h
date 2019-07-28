#ifndef ASSETS_H
#define ASSETS_H

namespace assets
{
    enum class AssetLoadMode
    {
        NONE,
        SPLASH_LOADED,
        GAME_ASSETS_LOADED,
        FINISHED
    };

    struct AssetState
    {
        AssetLoadMode load_mode;
        
        OnLoadAssets *on_load_assets;
        OnAssetsLoaded *on_assets_loaded;
    };
}

#endif
